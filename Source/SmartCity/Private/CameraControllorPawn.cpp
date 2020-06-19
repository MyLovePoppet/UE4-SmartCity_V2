// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraControllorPawn.h"

#include "GameFramework/PlayerInput.h"
#include "Renderer/Private/PostProcess/SceneRenderTargets.h"
#include "RHI.h"
// Sets default values
ACameraControllorPawn::ACameraControllorPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("MyCamera"));

    AutoPossessPlayer = EAutoReceiveInput::Player0;

    CameraState = MakeShareable<Camera>(new Camera());
}

void ACameraControllorPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
void ACameraControllorPawn::Init(AActor* _EarthActor)
{
    this->SetEarthActor(_EarthActor);
}
/**
 *还有一个ViewProJectionMatrix没实现,此函数放在tick内进行更新
 */
void ACameraControllorPawn::UpdateCameraState()
{
    this->CameraState->SetLocation(CameraComponent->GetComponentLocation());
    this->CameraState->SetRotation(CameraComponent->GetComponentRotation());
    this->CameraState->SetAspectRatio(CameraComponent->AspectRatio);
    this->CameraState->SetScreenResolution(GEngine->GameViewport->Viewport->GetSizeXY());
    this->CameraState->SetFOV(CameraComponent->FieldOfView);
    this->CameraState->SetIsPerspectView(CameraComponent->ProjectionMode == ECameraProjectionMode::Perspective);
    this->CameraState->SetUpDirection(CameraComponent->GetUpVector());
    //this->CameraState->SetViewProjectionMatrix()
}
/**
 *
 */
bool ACameraControllorPawn::ViewPortToWorld(FVector2D ScreenPos, FVector& outWorldPos)
{
    float ScreenSpaceDepth = DepthOnScreen(ScreenPos);
    APlayerController* Player = GetWorld()->GetFirstPlayerController();
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

float ACameraControllorPawn::DepthOnScreen(FVector2D TargetPoint)
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
    GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Blue, FString::SanitizeFloat(DepthPixelValue * 1024));
    return DepthPixelValue;
}

TArray<DepthPixel> ACameraControllorPawn::GetDepthSurface(FVector2D& OutRenderTargetSize)
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

Camera* ACameraControllorPawn::GetCameraState()
{
    return CameraState.Get();
}

AActor* ACameraControllorPawn::GetEarthActor()
{
    return EarthActor;
}

float ACameraControllorPawn::GetEarthRadius()
{
    return EarthRadius;
}


UCameraComponent* ACameraControllorPawn::GetCameraComponent()
{
    return CameraComponent;
}

void ACameraControllorPawn::SetCameraState(Camera* _Camera)
{
    this->CameraState = MakeShareable(_Camera);
}

void ACameraControllorPawn::SetEarthActor(AActor* _EarthActor)
{
    this->EarthActor = _EarthActor;
}

void ACameraControllorPawn::SetEarthRadius(float _EarthRadius)
{
    this->EarthRadius = _EarthRadius;
}

void ACameraControllorPawn::SetCameraComponent(UCameraComponent* _CameraComponent)
{
    this->CameraComponent = _CameraComponent;
}