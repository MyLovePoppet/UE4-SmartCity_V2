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

float ACameraControllorPawn::GetDepth(FVector2D cursor)
{
    if(cursor.X<0||cursor.Y<0)
        return 0.f;
    struct DepthPixel //定义深度像素结构体
    {
        float depth;
        char stencil;
        char unused1;
        char unused2;
        char unused3;
    };
    float depth;
    //float* cpuDataPtr; // Texture深度值数组首地址
    //TArray<DepthPixel> mydata; //最终获取色深度值数据
    //FIntPoint buffsize; //深度长宽大小X和Y
    
    FVector2D screenSize=FVector2D(1920,1080);
    ENQUEUE_RENDER_COMMAND(ReadSurfaceFloatCommand)( // 将读取深度数据的命令推给渲染线程进行执行
        [&screenSize,&cursor, &depth](FRHICommandListImmediate& RHICmdList)
        //&cpuDataPtr, &mydata, &buffsize为传入的外部参数
        {
            FSceneRenderTargets::Get(RHICmdList).AdjustGBufferRefCount(RHICmdList, 1);
            FTexture2DRHIRef uTex2DRes = FSceneRenderTargets::Get(RHICmdList).GetSceneDepthSurface();
            FIntPoint buffsize = uTex2DRes->GetSizeXY();
            uint32 sx = buffsize.X;
            uint32 sy = buffsize.Y;
            
            cursor.X=cursor.X*sx/screenSize.X;
            cursor.Y=cursor.Y*sx/screenSize.Y;
            
            //mydata.AddUninitialized(sx * sy);
            uint32 Lolstrid = 0;
            void* cpuDataPtr = RHILockTexture2D(uTex2DRes, 0, RLM_ReadOnly, Lolstrid, true);
            DepthPixel *depthPixel=static_cast<DepthPixel*>(cpuDataPtr);
            // 加锁获取可读depth Texture深度值数组首地址
            //DepthPixel depthPixel;
            depthPixel += static_cast<size_t>(sx * cursor.Y + cursor.X);
            DepthPixel dep;
            memcpy(&dep,depthPixel,sizeof(DepthPixel));
            //memcpy(&depthPixel, cpuDataPtr, sizeof(DepthPixel)); //复制深度数据
            depth=dep.depth;
            RHIUnlockTexture2D(uTex2DRes, 0, true); //解锁
            FSceneRenderTargets::Get(RHICmdList).AdjustGBufferRefCount(RHICmdList, -1);
        });
    FlushRenderingCommands(); //等待渲染线程执行
    return depth;
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
