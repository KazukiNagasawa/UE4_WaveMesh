// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "WaveActor.generated.h"

UCLASS()
class WAVEMESH_API AWaveActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWaveActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	/* Initialize */
	void Initialize();

	/* Update mesh */
	void UpdateMesh(float DeltaTime);

	/* Get mesh index */
	int32 GetMeshIndex(int32 MeshXIdx, int32 MeshYIdx);

	/* Get color */
	FLinearColor GetColor(float ElapsedTime);

	/* Get height from index */
	float GetHeight(int32 XIdx, int32 YIdx);

public:
	/* MaterialIntarface */
	UPROPERTY(BlueprintReadWrite, Category = "Variable", Meta = (ExposeOnSpawn = true))
		UMaterialInterface* MaterialInterface;

	/* X index max */
	UPROPERTY(BlueprintReadWrite, Category = "Variable", Meta = (ExposeOnSpawn = true))
		int XMax = 100;

	/* Y index max */
	UPROPERTY(BlueprintReadWrite, Category = "Variable", Meta = (ExposeOnSpawn = true))
		int YMax = 100;

	/* Width and height base size (cm) */
	UPROPERTY(BlueprintReadWrite, Category = "Variable", Meta = (ExposeOnSpawn = true))
		float SizeBase = 10;

	/* Collision */
	UPROPERTY(BlueprintReadWrite, Category = "Variable", Meta = (ExposeOnSpawn = true))
		bool IsCollision = true;

private:
	TArray<FVector> Normals;
	TArray<FProcMeshTangent> Tangents;
	TArray<FVector2D> UV0;
	TArray<int32> Triangles;

	int32 cx; // mesh Center index X
	int32 cy; // mesh Center index Y
	float DistMax;

	/* Elapsed */
	float ElapsedTime;

	/* Flags */
	bool IsCreated;

	UPROPERTY(VisibleAnyWhere)
		UProceduralMeshComponent* mesh;
};
