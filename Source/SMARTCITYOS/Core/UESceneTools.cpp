// Fill out your copyright notice in the Description page of Project Settings.


#include "UESceneTools.h"

// Sets default values
AUESceneTools::AUESceneTools()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AUESceneTools::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUESceneTools::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AUESceneTools::ViewPortToWorld(FVector2D ScreenPos, FVector & outWorldPos)
{
	float ScreenSpaceDepth = GetDepthOnScreen(ScreenPos);
	APlayerController* Player = GWorld->GetFirstPlayerController();
	ULocalPlayer* const LP = Player ? Player->GetLocalPlayer() : nullptr;
	if (LP && LP->ViewportClient)
	{
		// get the projection data
		FSceneViewProjectionData ProjectionData;
		if (LP->GetProjectionData(LP->ViewportClient->Viewport, eSSP_FULL, /*out*/ ProjectionData))
		{
			FMatrix const InvViewProjMatrix = ProjectionData.ComputeViewProjectionMatrix().InverseFast();
			//FSceneView::DeprojectScreenToWorld(ScreenPosition, ProjectionData.GetConstrainedViewRect(), InvViewProjMatrix, /*out*/ WorldPosition, /*out*/ WorldDirection);

			FIntRect ViewRect = ProjectionData.GetConstrainedViewRect();
			float PixelX = FMath::TruncToFloat(ScreenPos.X);
			float PixelY = FMath::TruncToFloat(ScreenPos.Y);

			// Get the eye position and direction of the mouse cursor in two stages (inverse transform projection, then inverse transform view).
			// This avoids the numerical instability that occurs when a view matrix with large translation is composed with a projection matrix

			// Get the pixel coordinates into 0..1 normalized coordinates within the constrained view rectangle
			const float NormalizedX = (PixelX - ViewRect.Min.X) / ((float)ViewRect.Width());
			const float NormalizedY = (PixelY - ViewRect.Min.Y) / ((float)ViewRect.Height());

			// Get the pixel coordinates into -1..1 projection space
			const float ScreenSpaceX = (NormalizedX - 0.5f) * 2.0f;
			//从左上角为原点转换到左下角为原点
			const float ScreenSpaceY = ((1.0f - NormalizedY) - 0.5f) * 2.0f;

			// The start of the ray trace is defined to be at mousex,mousey,1 in projection space (z=1 is near, z=0 is far - this gives us better precision)
			// To get the direction of the ray trace we need to use any z between the near and the far plane, so let's use (mousex, mousey, 0.5)
			const FVector4 RayStartProjectionSpace = FVector4(ScreenSpaceX, ScreenSpaceY, ScreenSpaceDepth, 1.0f);

			// Projection (changing the W coordinate) is not handled by the FMatrix transforms that work with vectors, so multiplications
			// by the projection matrix should use homogeneous coordinates (i.e. FPlane).
			const FVector4 HGRayStartWorldSpace = InvViewProjMatrix.TransformFVector4(RayStartProjectionSpace);
			FVector TargetWorldSpace(HGRayStartWorldSpace.X, HGRayStartWorldSpace.Y, HGRayStartWorldSpace.Z);
			// divide vectors by W to undo any projection and get the 3-space coordinate
			if (HGRayStartWorldSpace.W != 0.0f)
			{
				TargetWorldSpace /= HGRayStartWorldSpace.W;
			}

			outWorldPos = TargetWorldSpace;
			return true;
		}
	}
	outWorldPos = FVector::ZeroVector;
	return false;
}

float AUESceneTools::GetDepthOnScreen(FVector2D TargetPoint)
{
	FVector2D renderTargetSize;
	TArray<DepthPixel> depthSurface = GetDepthSurface(renderTargetSize);
	// UE_LOG(LogTemp, Log, TEXT("Size: %s"), *renderTargetSize.ToString());
	// for (int i = 0; i < renderTargetSize.X; ++i)
	// {
	//     FString Line;
	//     for (int j = 0; j < renderTargetSize.Y; ++j)
	//     {
	//         Line = Line.Append(FString::SanitizeFloat(depthSurface[j * renderTargetSize.X + i].depth) + " ");
	//     }
	//     UE_LOG(LogTemp, Log, TEXT("%s"),*Line);
	// }
	//TArray64<int8> temp;
	//memcpy(temp.GetData(),depthSurface.GetData(),depthSurface.Num());
	//FFileHelper::SaveArrayToFile(temp,TEXT("Texture"),);
	// depthSurface.ToString();
	float DepthPixelValue = depthSurface[TargetPoint.Y * renderTargetSize.X + TargetPoint.X].depth;
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, FString::SanitizeFloat(DepthPixelValue * 1024));
	return DepthPixelValue;
}

TArray<DepthPixel> AUESceneTools::GetDepthSurface(FVector2D & OutRenderTargetSize)
{
	//    float depth;
	float* cpuDataPtr; // Texture深度值数组首地址
	TArray<DepthPixel> mydata; //最终获取色深度值数据
	FIntPoint buffsize; //深度长宽大小X和Y

	ENQUEUE_RENDER_COMMAND(ReadSurfaceFloatCommand)( // 将读取深度数据的命令推给渲染线程进行执行
		[&cpuDataPtr, &mydata, &buffsize, &OutRenderTargetSize](FRHICommandListImmediate& RHICmdList)
		//&cpuDataPtr, &mydata, &buffsize为传入的外部参数
	{
		FSceneRenderTargets::Get(RHICmdList).AdjustGBufferRefCount(RHICmdList, 1);
		FTexture2DRHIRef uTex2DRes = FSceneRenderTargets::Get(RHICmdList).GetSceneDepthSurface();
		buffsize = uTex2DRes->GetSizeXY();
		uint32 sx = buffsize.X;
		uint32 sy = buffsize.Y;
		OutRenderTargetSize.X = buffsize.X;
		OutRenderTargetSize.Y = buffsize.Y;

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, buffsize.ToString());

		mydata.AddUninitialized(sx * sy);
		uint32 Lolstrid = 0;
		cpuDataPtr = static_cast<float*>(RHILockTexture2D(uTex2DRes, 0, RLM_ReadOnly, Lolstrid, true));
		// 加锁获取可读depth Texture深度值数组首地址
		memcpy(mydata.GetData(), cpuDataPtr, sx * sy * sizeof(DepthPixel)); //复制深度数据
		RHIUnlockTexture2D(uTex2DRes, 0, true); //解锁
		FSceneRenderTargets::Get(RHICmdList).AdjustGBufferRefCount(RHICmdList, -1);
	});
	FlushRenderingCommands(); //等待渲染线程执行

	return mydata;
}


bool AUESceneTools::ProjectWorldToScreen(const FVector& WorldPosition, const FIntRect& ViewRect, const FMatrix& ViewProjectionMatrix, FVector2D& out_ScreenPos)
{
	FPlane Result = ViewProjectionMatrix.TransformFVector4(FVector4(WorldPosition, 1.f));
	if (Result.W > 0.0f)
	{
		// the result of this will be x and y coords in -1..1 projection space
		const float RHW = 1.0f / Result.W;
		FPlane PosInScreenSpace = FPlane(Result.X * RHW, Result.Y * RHW, Result.Z * RHW, Result.W);

		// Move from projection space to normalized 0..1 UI space
		const float NormalizedX = (PosInScreenSpace.X / 2.f) + 0.5f;
		const float NormalizedY = 1.f - (PosInScreenSpace.Y / 2.f) - 0.5f;

		FVector2D RayStartViewRectSpace(
			(NormalizedX * (float)ViewRect.Width()),
			(NormalizedY * (float)ViewRect.Height())
		);

		out_ScreenPos = RayStartViewRectSpace + FVector2D(static_cast<float>(ViewRect.Min.X), static_cast<float>(ViewRect.Min.Y));

		return true;
	}
	return false;
}

FMatrix AUESceneTools::GetViewProjectionMatrix()
{
	FMatrix ViewProjMatrix;
	APlayerController* Player = GWorld->GetFirstPlayerController();
	ULocalPlayer* const LP = Player ? Player->GetLocalPlayer() : nullptr;
	if (LP && LP->ViewportClient)
	{
		// get the projection data
		FSceneViewProjectionData ProjectionData;
		if (LP->GetProjectionData(LP->ViewportClient->Viewport, eSSP_FULL, /*out*/ ProjectionData))
		{
			ViewProjMatrix = ProjectionData.ComputeViewProjectionMatrix();
		}	    
	}

	return ViewProjMatrix;
}

// TArray<HitProxy*> AUE_SceneTools::GetHitProxySurface(FVector2D & OutRenderTargetSize)
// {
// 	return TArray<HitProxy*>();
// }

