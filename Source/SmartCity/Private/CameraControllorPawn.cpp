// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraControllorPawn.h"

#include "GameFramework/PlayerInput.h"

// Sets default values
ACameraControllorPawn::ACameraControllorPawn()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;


    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("MyCamera"));

    AutoPossessPlayer = EAutoReceiveInput::Player0;
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
}

// Called to bind functionality to input
void ACameraControllorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    //鼠标左键
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("MouseLeft", EKeys::LeftMouseButton));
    PlayerInputComponent->BindAction("MouseLeft", IE_Pressed, this, &ACameraControllorPawn::OnLeftMouseDown);
    PlayerInputComponent->BindAction("MouseLeft", IE_Released, this, &ACameraControllorPawn::OnLeftMouseUp);

    //滚轮操作	
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("ScrollWheelUp", EKeys::MouseWheelAxis, 1.0f));
    PlayerInputComponent->BindAxis("ScrollWheelUp", this, &ACameraControllorPawn::OnScrollWheel);

    //鼠标中键
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("MouseMid", EKeys::MiddleMouseButton));
    PlayerInputComponent->BindAction("MouseMid", IE_Pressed, this, &ACameraControllorPawn::OnMidMouseDown);
    PlayerInputComponent->BindAction("MouseMid", IE_Released, this, &ACameraControllorPawn::OnMidMouseUp);

    //鼠标右键
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("MouseRight", EKeys::RightMouseButton));
    PlayerInputComponent->BindAction("MouseRight", IE_Pressed, this, &ACameraControllorPawn::OnRightMouseDown);
    PlayerInputComponent->BindAction("MouseRight", IE_Released, this, &ACameraControllorPawn::OnRightMouseUp);
    //鼠标移动
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MouseMove", EKeys::MouseX, 1.0f));
    PlayerInputComponent->BindAxis("MouseMove", this, &ACameraControllorPawn::OnMouseMove);
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MouseMove", EKeys::MouseY, 1.0f));
    PlayerInputComponent->BindAxis("MouseMove", this, &ACameraControllorPawn::OnMouseMove);
}

void ACameraControllorPawn::OnLeftMouseUp()
{
}

void ACameraControllorPawn::OnLeftMouseDown()
{
}

void ACameraControllorPawn::OnRightMouseUp()
{
}

void ACameraControllorPawn::OnRightMouseDown()
{
}

void ACameraControllorPawn::OnMidMouseUp()
{
}

void ACameraControllorPawn::OnMidMouseDown()
{
}

void ACameraControllorPawn::OnMouseMove(float Axis)
{
}

void ACameraControllorPawn::OnScrollWheel(float Axis)
{
}
