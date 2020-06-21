// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadLandscapeStaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "Misc/FileHelper.h"
#include "Engine/StaticMesh.h"
#include "Runtime/Core/Public/Serialization/MemoryReader.h"
#include "Runtime/Core/Public/Serialization/MemoryWriter.h"
#include "Runtime/Engine/Public/StaticMeshResources.h"


// Sets default values
ALoadLandscapeStaticMesh::ALoadLandscapeStaticMesh()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LandscapeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LandscapeMesh"));
	RootComponent = LandscapeMesh;
}

// Called when the game starts or when spawned
void ALoadLandscapeStaticMesh::BeginPlay()
{
	Super::BeginPlay();
	
	// 通过设置“LevelNum”，确定要进行加载的层级
	int LevelNum = 15;
	//LoadLandscapeMesh(LevelNum); 
	GetTileShouldBeLoadedEachLevel(LevelNum);
}

// Called every frame
void ALoadLandscapeStaticMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	int LoadFrameSpace = 1;
	int TileNum = Frame_id / LoadFrameSpace;
	if (Frame_id % LoadFrameSpace == 0 && TileNum < pTileShouldBeLoaded.Num())
	{
		FVector Tile = pTileShouldBeLoaded[TileNum];
		LoadLandscapeMesh_OneTile(Tile.X, Tile.Y, Tile.Z);
		FString Message = "Frame: " + FString::FromInt(Frame_id);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, Message);		
	}


	//int LoadNumEachFrame = 4;
	//int TileNum = Frame_id * LoadNumEachFrame;
	//if (TileNum < pTileShouldBeLoaded.Num())
	//{
	//	for (int i = 0; i< LoadNumEachFrame; i++)
	//	{
	//		FVector Tile = pTileShouldBeLoaded[TileNum + i]; 
	//		LoadLandscapeMesh_OneTile(Tile.X, Tile.Y, Tile.Z);
	//		FString Message = "Frame: " + FString::FromInt(Frame_id);
	//		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, Message);
	//	}		
	//}

	Frame_id += 1;
}


void ALoadLandscapeStaticMesh::LoadLandscapeMesh(int LevelNum)
{
	//各层级瓦片范围
	int XMin = 0;
	int XMax = 0;
	int YMin = 0;
	int YMax = 0;
	if (LevelNum >= 0 && LevelNum <= 5)
	{
		XMin = 0;
		XMax = pow(2, LevelNum) - 1;
		YMin = 0;
		YMax = pow(2, LevelNum + 1) - 1;
	}
	else if (LevelNum == 15)
	{
		XMin = 21469;
		XMax = 21488;
		YMin = 48582;
		YMax = 48603;
	}

	for (int32 YNum = YMin; YNum <= YMax; YNum++)
	{
		for (int32 XNum = XMin; XNum <= XMax; XNum++)
		{
			//获取TileMesh
			FString TileMeshFolder = "/Game/Cache/Grid/" + FString::FromInt(LevelNum) + "/" + FString::FromInt(YNum) + "/" + FString::FromInt(XNum) + "/";
			FString TileMeshName = "Grid_" + FString::FromInt(LevelNum) + "-" + FString::FromInt(YNum) + "-" + FString::FromInt(XNum);
			FString TileMeshUassectName = "StaticMesh'" + TileMeshFolder + TileMeshName + "." + TileMeshName + "'";
			UStaticMesh* TileMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *(TileMeshUassectName)));

			if (!TileMesh)
			{
				FString Message = "No Data: " + TileMeshName;
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, Message);
				continue;
			}

			//创建TileMeshComponent
			UStaticMeshComponent* TileMeshComponent = NewObject<UStaticMeshComponent>(this);
			this->AddOwnedComponent(TileMeshComponent);
			TileMeshComponent->AttachTo(RootComponent);

			//设置TileMeshComponent的几何为TileMesh
			TileMeshComponent->SetStaticMesh(TileMesh);

			TileMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			TileMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			TileMeshComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Overlap);
			//TileMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

			TileMeshComponent->RegisterComponent();
			TileMeshComponent->MarkRenderStateDirty();

			FString Message = "Loadinig Tile: " + TileMeshName;
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, Message);
		}
	}
}

void ALoadLandscapeStaticMesh::GetTileShouldBeLoadedEachLevel(int LevelNum)
{
	//各层级瓦片范围
	int XMin = 0;
	int XMax = 0;
	int YMin = 0;
	int YMax = 0;
	if (LevelNum >= 0 && LevelNum <= 5)
	{
		XMin = 0;
		XMax = pow(2, LevelNum) - 1;
		YMin = 0;
		YMax = pow(2, LevelNum + 1) - 1;
	}
	else if (LevelNum == 15)
	{
		XMin = 21469;
		XMax = 21488;
		YMin = 48582;
		YMax = 48603;
	}

	for (int32 YNum = YMin; YNum <= YMax; YNum++)
	{
		for (int32 XNum = XMin; XNum <= XMax; XNum++)
		{
			pTileShouldBeLoaded.Add(FVector(LevelNum, YNum, XNum));
		}
	}
}

bool ALoadLandscapeStaticMesh::LoadLandscapeMesh_OneTile(int LevelNum, int YNum, int XNum)
{
	//获取TileMesh
	FString TileMeshFolder = "/Game/Cache/Grid/" + FString::FromInt(LevelNum) + "/" + FString::FromInt(YNum) + "/" + FString::FromInt(XNum) + "/";
	FString TileMeshName = "Grid_" + FString::FromInt(LevelNum) + "-" + FString::FromInt(YNum) + "-" + FString::FromInt(XNum);
	FString TileMeshUassectName = "StaticMesh'" + TileMeshFolder + TileMeshName + "." + TileMeshName + "'";
	UStaticMesh* TileMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, *(TileMeshUassectName)));

	if (!TileMesh)
	{
		FString Message = "No Data: " + TileMeshName;
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, Message);
		return false;
	}

	//创建TileMeshComponent
	UStaticMeshComponent* TileMeshComponent = NewObject<UStaticMeshComponent>(this);
	this->AddOwnedComponent(TileMeshComponent);
	TileMeshComponent->AttachTo(RootComponent);

	TileMesh->BodySetup->CollisionTraceFlag=ECollisionTraceFlag::CTF_UseComplexAsSimple;
	//设置TileMeshComponent的几何为TileMesh
	TileMeshComponent->SetStaticMesh(TileMesh);

	TileMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//TileMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//TileMeshComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Overlap);
	//TileMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	TileMeshComponent->RegisterComponent();
	TileMeshComponent->MarkRenderStateDirty();

	FString Message = "Loadinig Tile: " + TileMeshName;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, Message);

	return true;
}
