// Fill out your copyright notice in the Description page of Project Settings.


#include "EarthActor.h"

// Sets default values
AEarthActor::AEarthActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEarthActor::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AEarthActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
