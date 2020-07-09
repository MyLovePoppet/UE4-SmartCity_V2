// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Renderer/Private/PostProcess/SceneRenderTargets.h"
#include "RHI.h"
#include "UESceneTools.generated.h"



struct DepthPixel //定义深度像素结构体
{
	float depth;
	char stencil;
	char unused1;
	char unused2;
	char unused3;
};

UCLASS()
class SMARTCITYOS_API AUESceneTools : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUESceneTools();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	static bool ViewPortToWorld(FVector2D ScreenPos, FVector& outWorldPos);

	static float GetDepthOnScreen(FVector2D TargetPoint);

	static TArray<DepthPixel> GetDepthSurface(FVector2D& OutRenderTargetSize);

	static FMatrix GetViewProjectionMatrix();

	static bool ProjectWorldToScreen(const FVector& WorldPosition, const FIntRect& ViewRect, const FMatrix& ViewProjectionMatrix, FVector2D& out_ScreenPos);

	//static TArray<HitProxy*> GetHitProxySurface(FVector2D& OutRenderTargetSize);

};
