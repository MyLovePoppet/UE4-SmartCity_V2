// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "LoadLandscapeStaticMesh.generated.h"

UCLASS()
class SMARTCITY_API ALoadLandscapeStaticMesh : public AActor
{
	GENERATED_BODY()
	
public:	
    // Sets default values for this actor's properties
    ALoadLandscapeStaticMesh();

	protected:
        // Called when the game starts or when spawned
        virtual void BeginPlay() override;

	public:	
        // Called every frame
        virtual void Tick(float DeltaTime) override;

	//动态加载LandscapeTileMesh
	void LoadLandscapeMesh(int LevelNum);

	TArray<FVector> pTileShouldBeLoaded;

	//TileShouldBeLoaded: FVector(LevelNum, XNum, YNum)
	void GetTileShouldBeLoadedEachLevel(int LevelNum);
	bool LoadLandscapeMesh_OneTile(int LevelNum, int YNum, int XNum);


	public:
        UPROPERTY(VisibleAnywhere)
            UStaticMeshComponent* LandscapeMesh;

	int Frame_id = 0;

};