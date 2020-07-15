// Fill out your copyright notice in the Description page of Project Settings.


#include "InputPawn.h"

#include "Kismet/GameplayStatics.h"


TArray<InputBase*> AInputPawn::inputListeners;
bool InputBase::bOnLeftControl;
bool InputBase::bOnLeftShift;
bool InputBase::bOnLeftAlt;

void AInputPawn::RegisterListener(InputBase* lister)
{
    if (lister && !inputListeners.Contains(lister))
        inputListeners.Add(lister);
}

void AInputPawn::UnRegisterListener(InputBase* lister)
{
    inputListeners.Remove(lister);
}

// Sets default values
AInputPawn::AInputPawn()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    //初始化UECamera
    //RootComponent = UECamera;
    InputBase::bOnLeftControl = false;
    InputBase::bOnLeftShift = false;
    InputBase::bOnLeftAlt = false;
    AutoPossessPlayer = EAutoReceiveInput::Player0;

    CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("MyCollisionCapsule"));
    if (CapsuleComponent)
    {
        CapsuleComponent->SetWorldLocation(FVector::ZeroVector);
        CapsuleComponent->InitCapsuleSize(42.0f, 96.0f);
        CapsuleComponent->SetCollisionProfileName(TEXT("Pawn"));
        CapsuleComponent->SetSimulatePhysics(false);
        CapsuleComponent->SetEnableGravity(false);
        RootComponent = CapsuleComponent;
        CapsuleComponent->GetBodyInstance()->COMNudge = FVector(0.0f, 0.0f, -96.0f);
        CapsuleComponent->SetLinearDamping(0.15f);
        CapsuleComponent->SetAngularDamping(100.0f);
        CapsuleComponent->SetNotifyRigidBodyCollision(true);
    }
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SCOSCamera"));
    if (CameraComponent)
    {
        CameraComponent->FieldOfView = 90.0f;
        CameraComponent->SetupAttachment(CapsuleComponent);
        UECamera = CameraComponent;
    }
    MovementComponent = CreateDefaultSubobject<UCustomGravityPawnMovement>(TEXT("MovementComponent0"));

    // Initialization
    //CameraPitchMin = -85.0f;
    //CameraPitchMax = 85.0f;
}

// Called when the game starts or when spawned
void AInputPawn::BeginPlay()
{
    Super::BeginPlay();
    AInputPawn::inputListeners.Empty();
}

// Called every frame
void AInputPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    for (auto& listener : inputListeners)
    {
        listener->OnTick(DeltaTime);
    }
}

UCameraComponent* AInputPawn::GetUECamera()
{
    return UECamera;
}

void AInputPawn::MouseLMove(float value)
{
    FVector2D currentCursorPt;
    GetWorld()->GetFirstPlayerController()->GetMousePosition(currentCursorPt.X, currentCursorPt.Y);
    for (InputBase* il : inputListeners)
        il->OnMouseLMove(currentCursorPt, value);
}

void AInputPawn::ScrollWheel(float value)
{
    FVector2D currentCursorPt;
    GetWorld()->GetFirstPlayerController()->GetMousePosition(currentCursorPt.X, currentCursorPt.Y);
    for (InputBase* il : inputListeners)
        il->OnMouseWheel(currentCursorPt, value);
}

void AInputPawn::MouseMMove(float value)
{
    FVector2D currentCursorPt;
    GetWorld()->GetFirstPlayerController()->GetMousePosition(currentCursorPt.X, currentCursorPt.Y);
    for (InputBase* il : inputListeners)
        il->OnMouseMMove(currentCursorPt, value);
}

void AInputPawn::MouseRMove(float value)
{
    FVector2D currentCursorPt;
    GetWorld()->GetFirstPlayerController()->GetMousePosition(currentCursorPt.X, currentCursorPt.Y);
    for (InputBase* il : inputListeners)
        il->OnMouseRMove(currentCursorPt, value);
}

void AInputPawn::PressedAction(FKey key)
{
    //GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, key.ToString());
    FVector2D currentCursorPt;
    GetWorld()->GetFirstPlayerController()->GetMousePosition(currentCursorPt.X, currentCursorPt.Y);
    if (key == EKeys::LeftMouseButton)
    {
        for (InputBase* il : inputListeners)
            il->OnMouseLButtonDown(currentCursorPt);
    }
    else if (key == EKeys::MiddleMouseButton)
    {
        for (InputBase* il : inputListeners)
            il->OnMouseMidButtonDown(currentCursorPt);
    }
    else if (key == EKeys::RightMouseButton)
    {
        for (InputBase* il : inputListeners)
            il->OnMouseRButtonDown(currentCursorPt);
    }
    else
    {
        if (key == EKeys::LeftAlt)
        {
            InputBase::bOnLeftAlt = true;
        }
        else if (key == EKeys::LeftControl)
        {
            InputBase::bOnLeftControl = true;
        }
        else if (key == EKeys::LeftShift)
        {
            InputBase::bOnLeftShift = true;
        }

        for (InputBase* il : inputListeners)
            il->OnKeyDown(key);
    }
}

void AInputPawn::ReleasedAction(FKey key)
{
    FVector2D currentCursorPt;
    GetWorld()->GetFirstPlayerController()->GetMousePosition(currentCursorPt.X, currentCursorPt.Y);
    if (key == EKeys::LeftMouseButton)
    {
        for (InputBase* il : inputListeners)
            il->OnMouseLButtonUp(currentCursorPt);
    }
    else if (key == EKeys::MiddleMouseButton)
    {
        for (InputBase* il : inputListeners)
            il->OnMouseMidButtonUp(currentCursorPt);
    }
    else if (key == EKeys::RightMouseButton)
    {
        for (InputBase* il : inputListeners)
            il->OnMouseRButtonUp(currentCursorPt);
    }
    else
    {
        if (key == EKeys::LeftAlt)
        {
            InputBase::bOnLeftAlt = false;
        }
        else if (key == EKeys::LeftControl)
        {
            InputBase::bOnLeftControl = false;
        }
        else if (key == EKeys::LeftShift)
        {
            InputBase::bOnLeftShift = false;
        }

        for (InputBase* il : inputListeners)
            il->OnKeyUp(key);
    }
}

// void AInputPawn::DoubleClick(FKey key)
// {
//     for (InputBase* il : inputListeners)
//         il->OnDoubleClick(key);
// }

//按下W时value=1.f；S是value=-1.f
void AInputPawn::WAxis(float value)
{
    for (InputBase* il : inputListeners)
        il->WAxis(value);
}

//按下D时value=1.f；S是value=-1.f
void AInputPawn::DAxis(float value)
{
    for (InputBase* il : inputListeners)
        il->DAxis(value);
}

//按下D时value=1.f；S是value=-1.f
void AInputPawn::MouseX(float value)
{
    for (InputBase* il : inputListeners)
        il->MouseX(value);
}

//按下D时value=1.f；S是value=-1.f
void AInputPawn::MouseY(float value)
{
    for (InputBase* il : inputListeners)
        il->MouseY(value);
}

// Called to bind functionality to input
void AInputPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // TArray<FKey> ActionKeys = {
    //     EKeys::One, EKeys::Two, EKeys::Three, EKeys::Four, EKeys::Five, EKeys::Six, EKeys::Seven,
    //     EKeys::SpaceBar,
    //     EKeys::LeftShift, EKeys::LeftControl, EKeys::LeftAlt,
    //     EKeys::LeftMouseButton, EKeys::MiddleMouseButton, EKeys::RightMouseButton
    // };
    // for (FKey key : ActionKeys)
    // {
    //     UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping(key.GetFName(), key));
    //     PlayerInputComponent->BindAction(key.GetFName(), IE_Released, this, &AInputPawn::ReleasedAction);
    //     PlayerInputComponent->BindAction(key.GetFName(), IE_Pressed, this, &AInputPawn::PressedAction);
    // }
    // // FKey key=EKeys::LeftMouseButton;
    // // UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping(key.GetFName(), key));
    // // PlayerInputComponent->BindAction(key.GetFName(), IE_DoubleClick, this, &AInputPawn::DoubleClick);
    // //
    // //鼠标左键	
    // UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MouseLeft", EKeys::LeftMouseButton, 1.0f));
    // PlayerInputComponent->BindAxis("MouseLeft", this, &AInputPawn::MouseLMove);
    //
    // //滚轮操作
    // UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("ScrollWheelUp", EKeys::MouseWheelAxis, 1.0f));
    // PlayerInputComponent->BindAxis("ScrollWheelUp", this, &AInputPawn::ScrollWheel);
    //
    // //鼠标中键轴
    // UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MouseMid", EKeys::MiddleMouseButton, 1.0f));
    // PlayerInputComponent->BindAxis("MouseMid", this, &AInputPawn::MouseMMove);
    //
    // //鼠标右键轴
    // UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MouseRight", EKeys::RightMouseButton, 1.0f));
    // PlayerInputComponent->BindAxis("MouseRight", this, &AInputPawn::MouseRMove);
    //
    //
    // ////////////////////////////////////////////////////////////////////////////////////////////////
    // ///以下为Ground模式的按键绑定
    // //鼠标移动
    // UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MouseX", EKeys::MouseX, 1.0f));
    // PlayerInputComponent->BindAxis("MouseX", this, &AInputPawn::MouseX); //AddCameraYawInput);
    // UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MouseY", EKeys::MouseY, 1.0f));
    // PlayerInputComponent->BindAxis("MouseY", this, &AInputPawn::MouseY); //AddCameraPitchInput);
    //
    // //键盘轴事件 
    // UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("KeyForwardMove", EKeys::W, 1.0f));
    // PlayerInputComponent->BindAxis("KeyForwardMove", this, &AInputPawn::WAxis);
    // UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("KeyForwardMove", EKeys::S, -1.0f));
    // PlayerInputComponent->BindAxis("KeyForwardMove", this, &AInputPawn::WAxis);
    // UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("KeyRightMove", EKeys::A, -1.0f));
    // PlayerInputComponent->BindAxis("KeyRightMove", this, &AInputPawn::DAxis);
    // UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("KeyRightMove", EKeys::D, 1.0f));
    // PlayerInputComponent->BindAxis("KeyRightMove", this, &AInputPawn::DAxis);
}

void AInputPawn::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
                           bool bSelfMoved,
                           FVector HitLocation, FVector HitNormal, FVector NormalImpulse,
                           const FHitResult& Hit)
{
    Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

    if (MovementComponent)
    {
        MovementComponent->CapsuleHited(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse,
                                        Hit);
    }
}

void AInputPawn::Jump()
{
    if (MovementComponent)
    {
        MovementComponent->DoJump();
    }
}

void AInputPawn::Sprint()
{
    if (MovementComponent)
    {
        MovementComponent->DoSprint();
    }
}

void AInputPawn::StopSprint()
{
    if (MovementComponent)
    {
        MovementComponent->DoStopSprint();
    }
}

UPawnMovementComponent* AInputPawn::GetMovementComponent() const
{
    return MovementComponent;
}
