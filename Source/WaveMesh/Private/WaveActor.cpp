// Fill out your copyright notice in the Description page of Project Settings.


#include "WaveActor.h"

// Sets default values
AWaveActor::AWaveActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	RootComponent = mesh;
	mesh->bUseAsyncCooking = true; // New in UE4.17, multi-threaded PhysX cooking.

}

// Called when the game starts or when spawned
void AWaveActor::BeginPlay()
{
	Super::BeginPlay();
	this->Initialize();
}


// Called every frame
void AWaveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateMesh(DeltaTime);
}


void AWaveActor::Initialize()
{
	int MeshIndexSize = (this->XMax - 1) * (this->YMax - 1) * 4;

	this->Normals.Init(FVector(0, 0, 1), MeshIndexSize);
	this->Tangents.Init(FProcMeshTangent(0, 1, 0), MeshIndexSize);

	int midx;
	for (int my = 0; my < this->YMax - 1; my++) {
		for (int mx = 0; mx < this->XMax - 1; mx++) {
			midx = this->GetMeshIndex(mx, my);

			this->UV0.Add(FVector2D(0, 0));
			this->UV0.Add(FVector2D(0, 1));
			this->UV0.Add(FVector2D(1, 1));
			this->UV0.Add(FVector2D(1, 0));

			this->Triangles.Add(0 + midx * 4);
			this->Triangles.Add(1 + midx * 4);
			this->Triangles.Add(2 + midx * 4);
			this->Triangles.Add(0 + midx * 4);
			this->Triangles.Add(2 + midx * 4);
			this->Triangles.Add(3 + midx * 4);
		}
	}

	this->cx = this->XMax % 2 == 0 ? this->XMax / 2 : this->XMax / 2 + 1;
	this->cy = this->YMax % 2 == 0 ? this->YMax / 2 : this->YMax / 2 + 1;
	this->DistMax = FMath::Min(this->XMax, this->YMax) / 2.0;

	this->ElapsedTime = 0.0;
	this->IsCreated = false;
}

void AWaveActor::UpdateMesh(float DeltaTime)
{
	this->ElapsedTime += DeltaTime;

	TArray<FVector> Vertices;
	TArray<FLinearColor> VertexColors;

	float OffsetX = -this->SizeBase * this->cx;
	float OffsetY = -this->SizeBase * this->cy;

	// Calc per mesh
	FLinearColor Color = GetColor(this->ElapsedTime);
	float SizeBaseHalf = this->SizeBase / 2.0;
	float x0, x1, y0, y1;
	for (int my = 0; my < this->YMax - 1; my++) {
		for (int mx = 0; mx < this->XMax - 1; mx++) {
			x0 = this->SizeBase * (mx - this->cx) + SizeBaseHalf;
			x1 = x0 + this->SizeBase;
			y0 = this->SizeBase * (my - this->cy) + SizeBaseHalf;
			y1 = y0 + this->SizeBase;

			Vertices.Add(FVector(x0, y0, this->GetHeight(mx + 0, my + 0)));
			Vertices.Add(FVector(x0, y1, this->GetHeight(mx + 0, my + 1)));
			Vertices.Add(FVector(x1, y1, this->GetHeight(mx + 1, my + 1)));
			Vertices.Add(FVector(x1, y0, this->GetHeight(mx + 1, my + 0)));

			for (int c = 0; c < 4; c++) {
				VertexColors.Add(Color);
			}
		}
	}

	if (this->IsCreated) {
		mesh->UpdateMeshSection_LinearColor(0, Vertices, Normals, UV0, VertexColors, Tangents);
	}
	else {
		this->IsCreated = true;
		mesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, VertexColors, Tangents, this->IsCollision);
		mesh->ContainsPhysicsTriMeshData(this->IsCollision);
		mesh->CreateDynamicMaterialInstance(0, this->MaterialInterface);
	}
}

int32 AWaveActor::GetMeshIndex(int32 MeshXIdx, int32 MeshYIdx)
{
	// NOTE: No index size check.
	return MeshXIdx + (this->XMax - 1) * MeshYIdx;
}

FLinearColor AWaveActor::GetColor(float ElapsedTime)
{
	float red = FMath::Cos(ElapsedTime * 4) / 2.0 + 0.5;
	float green = FMath::Sin(ElapsedTime * 2) / 2.0 + 0.5;
	float blue = FMath::Cos(ElapsedTime) / 2.0 + 0.5;
	
	return FLinearColor(red, green, blue, 1.0);
}

float AWaveActor::GetHeight(int32 XIdx, int32 YIdx)
{
	// Distance
	float Dist = FMath::Sqrt(FMath::Pow(XIdx - this->cx, 2) + FMath::Pow(YIdx - this->cy, 2));
	float DistNorm = Dist / this->DistMax;

	// Žž‚ÆˆÊ’u‚É‰ž‚¶‚½ sin ‚É‚æ‚é‚‚³
	float Theta = DistNorm * 2.0 * PI + this->ElapsedTime;
	return FMath::Sin(Theta) * 100;
}
