// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraControllorPawn.h"

#include "GameFramework/PlayerInput.h"

// Sets default values
ACameraControllorPawn::ACameraControllorPawn()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    //SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("MyCamera"));

    AutoPossessPlayer = EAutoReceiveInput::Player0;

    CameraState = MakeShareable<Camera>(new Camera());
}

// Called when the game starts or when spawned
void ACameraControllorPawn::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void ACameraControllorPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);


    //更新Camera的State
    UpdateCameraState();
}

// Called to bind functionality to input
void ACameraControllorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    //鼠标左键
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("MouseLeft", EKeys::LeftMouseButton));
    PlayerInputComponent->BindAction("MouseLeft", IE_Pressed, this, &ACameraControllorPawn::OnMouseDown);
    PlayerInputComponent->BindAction("MouseLeft", IE_Released, this, &ACameraControllorPawn::OnMouseUp);

    //鼠标中键
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("MouseMid", EKeys::MiddleMouseButton));
    PlayerInputComponent->BindAction("MouseMid", IE_Pressed, this, &ACameraControllorPawn::OnMouseDown);
    PlayerInputComponent->BindAction("MouseMid", IE_Released, this, &ACameraControllorPawn::OnMouseUp);

    //鼠标右键
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("MouseRight", EKeys::RightMouseButton));
    PlayerInputComponent->BindAction("MouseRight", IE_Pressed, this, &ACameraControllorPawn::OnMouseDown);
    PlayerInputComponent->BindAction("MouseRight", IE_Released, this, &ACameraControllorPawn::OnMouseUp);


    //鼠标移动
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MouseXMove", EKeys::MouseX, 1.0f));
    PlayerInputComponent->BindAxis("MouseXMove", this, &ACameraControllorPawn::OnMouseXMove);
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MouseYMove", EKeys::MouseY, 1.0f));
    PlayerInputComponent->BindAxis("MouseYMove", this, &ACameraControllorPawn::OnMouseYMove);

    //滚轮操作	
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("ScrollWheelUp", EKeys::MouseWheelAxis, 1.0f));
    PlayerInputComponent->BindAxis("ScrollWheelUp", this, &ACameraControllorPawn::OnScrollWheel);

    //键盘轴事件 
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("KeyForwardMove", EKeys::W, 1.0f));
    PlayerInputComponent->BindAxis("KeyForwardMove", this, &ACameraControllorPawn::OnKeyMove);
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("KeyForwardMove", EKeys::S, -1.0f));
    PlayerInputComponent->BindAxis("KeyForwardMove", this, &ACameraControllorPawn::OnKeyMove);
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("KeyRightMove", EKeys::A, -1.0f));
    PlayerInputComponent->BindAxis("KeyRightMove", this, &ACameraControllorPawn::OnKeyMove);
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("KeyRightMove", EKeys::D, 1.0f));
    PlayerInputComponent->BindAxis("KeyRightMove", this, &ACameraControllorPawn::OnKeyMove);

    //键盘事件 
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("KeyDown", EKeys::LeftShift));
    PlayerInputComponent->BindAction("KeyDown", IE_Pressed, this, &ACameraControllorPawn::OnKeyDown);
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("KeyDown", EKeys::SpaceBar));
    PlayerInputComponent->BindAction("KeyDown", IE_Pressed, this, &ACameraControllorPawn::OnKeyDown);

    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("KeyUp", EKeys::LeftShift));
    PlayerInputComponent->BindAction("KeyUp", IE_Released, this, &ACameraControllorPawn::OnKeyDown);
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("KeyUp", EKeys::SpaceBar));
    PlayerInputComponent->BindAction("KeyUp", IE_Released, this, &ACameraControllorPawn::OnKeyDown);
}

void ACameraControllorPawn::OnMouseUp()
{
    if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustReleased(EKeys::LeftMouseButton))
        OnMouseUpAction(EKeys::LeftMouseButton);
    if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustReleased(EKeys::MiddleMouseButton))
        OnMouseUpAction(EKeys::MiddleMouseButton);
    if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustReleased(EKeys::RightMouseButton))
        OnMouseUpAction(EKeys::RightMouseButton);
}

void ACameraControllorPawn::OnMouseDown()
{
    if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::LeftMouseButton))
        OnMouseDownAction(EKeys::LeftMouseButton);
    if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::MiddleMouseButton))
        OnMouseDownAction(EKeys::MiddleMouseButton);
    if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::RightMouseButton))
        OnMouseDownAction(EKeys::RightMouseButton);
}

void ACameraControllorPawn::OnMouseDownAction(FKey Key)
{
}

void ACameraControllorPawn::OnMouseUpAction(FKey Key)
{
}

void ACameraControllorPawn::OnMouseXMove(float Axis)
{
}

void ACameraControllorPawn::OnMouseYMove(float Axis)
{
}

void ACameraControllorPawn::OnScrollWheel(float Axis)
{
}

void ACameraControllorPawn::OnKeyDown()
{
    if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::SpaceBar))
        OnKeyDownAction(EKeys::SpaceBar);
    if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::LeftShift))
        OnKeyDownAction(EKeys::LeftShift);
}

void ACameraControllorPawn::OnKeyUp()
{
    if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustReleased(EKeys::SpaceBar))
        OnKeyUpAction(EKeys::SpaceBar);
    if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustReleased(EKeys::LeftShift))
        OnKeyUpAction(EKeys::LeftShift);
}

void ACameraControllorPawn::OnKeyMove(float Axis)
{
    if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::W))
    {
        OnKeyAxisMove(EKeys::W, Axis);
    }

    if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::S))
    {
        OnKeyAxisMove(EKeys::S, Axis);
    }
    if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::A))
    {
        OnKeyAxisMove(EKeys::A, Axis);
    }
    if (GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::D))
    {
        OnKeyAxisMove(EKeys::D, Axis);
    }
}

void ACameraControllorPawn::OnKeyAxisMove(FKey Key, float Axis)
{
}

void ACameraControllorPawn::OnKeyUpAction(FKey Key)
{
}

void ACameraControllorPawn::OnKeyDownAction(FKey Key)
{
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

/*USpringArmComponent* ACameraControllorPawn::GetSpringArm()
{
    return SpringArm;
}*/

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

/*void ACameraControllorPawn::SetSpringArm(USpringArmComponent* _SpringArm)
{
    this->SpringArm = _SpringArm;
}*/

void ACameraControllorPawn::SetCameraComponent(UCameraComponent* _CameraComponent)
{
    this->CameraComponent = _CameraComponent;
}
