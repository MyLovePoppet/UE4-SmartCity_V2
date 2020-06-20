// Fill out your copyright notice in the Description page of Project Settings.


#include "GridCollisionTestActor.h"
//#include "UObject/LinkerLoad.h"

// Sets default values
AGridCollisionTestActor::AGridCollisionTestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGridCollisionTestActor::BeginPlay()
{
	Super::BeginPlay();
	GridStaticMeshComponent->SetRelativeLocation(FVector(-296830.f,-5635620.f,-2975810.f));
	//设置为将复杂碰撞用于简单碰撞
	GridStaticMeshComponent->GetStaticMesh()->BodySetup->CollisionTraceFlag=ECollisionTraceFlag::CTF_UseComplexAsSimple;
}

// Called every frame
void AGridCollisionTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

