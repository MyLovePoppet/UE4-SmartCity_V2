// Fill out your copyright notice in the Description page of Project Settings.


#include "GroundCameraControllor.h"

#include "Kismet/GameplayStatics.h"
// Sets default values
AGroundCameraControllor::AGroundCameraControllor(): Super()
{
    CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("MyCollisionCapsule"));
    if (CapsuleComponent)
    {
        CapsuleComponent->InitCapsuleSize(42.0f, 96.0f);
        CapsuleComponent->SetCollisionProfileName(TEXT("Pawn"));
        CapsuleComponent->SetSimulatePhysics(true);
        CapsuleComponent->SetEnableGravity(false);
        RootComponent = CapsuleComponent;
        CapsuleComponent->GetBodyInstance()->COMNudge = FVector(0.0f, 0.0f, -96.0f);
        CapsuleComponent->SetLinearDamping(0.15f);
        CapsuleComponent->SetAngularDamping(100.0f);
        CapsuleComponent->SetNotifyRigidBodyCollision(true);
    }

    /*if (SpringArm)
    {
        SpringArm->AddRelativeLocation(FVector(0.f, 0.f, 64.f));
        SpringArm->TargetArmLength = 10.0f;
        SpringArm->bDoCollisionTest = true;
        SpringArm->ProbeSize = 15.0f;
        SpringArm->ProbeChannel = ECollisionChannel::ECC_Camera;
        SpringArm->bUsePawnControlRotation = false;
        SpringArm->bInheritPitch = true;
        SpringArm->bInheritYaw = true;
        SpringArm->bInheritRoll = true;
        SpringArm->SetupAttachment(CapsuleComponent);
    }*/

    if (CameraComponent)
    {
        CameraComponent->FieldOfView = 90.0f;
        //CameraComponent->SetupAttachment(SpringArm, SpringArm->SocketName);
        CameraComponent->SetupAttachment(CapsuleComponent);
    }


    MovementComponent = CreateDefaultSubobject<UCustomGravityPawnMovement>(TEXT("MovementComponent0"));
    if (MovementComponent)
    {
        MovementComponent->UpdatedComponent = CapsuleComponent;
        MovementComponent->SetUpdatedComponent(CapsuleComponent);
        MovementComponent->SetComponentOwner(this);
        MovementComponent->setPlanetActor(this->EarthActor);
    }


    // Initialization

    CameraPitchMin = -85.0f;
    CameraPitchMax = 85.0f;

    RotationInterpSpeed = 5.f;
    MinVelocityToRotateMesh = 2.f;
}


void AGroundCameraControllor::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    CurrentForwardDirection = GetActorForwardVector();
    CurrentRightDirection = GetActorRightVector();
}

// Called every frame
void AGroundCameraControllor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UpdateMeshRotation(DeltaTime);

    //    RootComponent->SetWorldRotation(FRotationMatrix::MakeFromXZ(CurrentForwardDirection, GetActorUpVector()).Rotator());
}

void AGroundCameraControllor::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
                                        bool bSelfMoved,
                                        FVector HitLocation, FVector HitNormal, FVector NormalImpulse,
                                        const FHitResult& Hit)
{
    Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

    if (MovementComponent == NULL)
    {
        return;
    }

    MovementComponent->CapsuleHited(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
}

// Called when the game starts or when spawned
void AGroundCameraControllor::BeginPlay()
{
    Super::BeginPlay();
    UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = false;
}

void AGroundCameraControllor::UpdateMeshRotation(float DeltaTime)
{
    /*/
    const float Speed = MovementComponent != NULL ? MovementComponent->Velocity.Size() : 0.0f;

    if (Speed < MinVelocityToRotateMesh)//&&bRotateMeshOnlyWhenMoving
    {
        return;
    }

    FRotator MeshRotation = GetMesh()->GetRelativeRotation();
    const FVector ProjectedVelocity = FVector::VectorPlaneProject(GetMovementComponent()->Velocity, GetActorUpVector());
    const FRotator Rot = FRotationMatrix::MakeFromXZ(GetTransform().InverseTransformVector(ProjectedVelocity), GetActorUpVector()).Rotator();

    MeshRotation.Yaw = 
        MeshStartRotation.Yaw + Rot.Yaw;

    GetMesh()->SetRelativeRotation( MeshRotation);// , MeshRotation, DeltaTime, RotationInterpSpeed);
    */
}


void AGroundCameraControllor::Jump()
{
    if (MovementComponent)
        MovementComponent->DoJump();
}

void AGroundCameraControllor::Sprint()
{
    if (MovementComponent)
    {
        MovementComponent->DoSprint();
    }
}

void AGroundCameraControllor::StopSprint()
{
    if (MovementComponent)
    {
        MovementComponent->DoStopSprint();
    }
}

void AGroundCameraControllor::setPlanetActor(AActor* Planet)
{
    if (MovementComponent)
    {
        MovementComponent->setPlanetActor(Planet);
    }
}

void AGroundCameraControllor::AddForwardMovementInput(float ScaleValue)
{
    if (MovementComponent == NULL) { return; }

    const FVector UpDirection = GetActorUpVector();
    const FVector CameraForward = CameraComponent->GetForwardVector();
    const float Dot = FVector::DotProduct(UpDirection, CameraForward);

    if (FMath::Abs(Dot) < 1 - SMALL_NUMBER)
    {
        CurrentForwardDirection = FVector::VectorPlaneProject(CameraForward, GetActorUpVector());
    }

    const float ControlValue = ScaleValue;
    // MovementComponent->IsMovingOnGround() ? ScaleValue : ScaleValue * MovementComponent->AirControlRatio;

    AddMovementInput(CurrentForwardDirection.GetSafeNormal(), ControlValue, false);
}

void AGroundCameraControllor::AddRightMovementInput(float ScaleValue)
{
    if (MovementComponent == NULL) { return; }

    const FVector UpDirection = GetActorUpVector();
    const FVector CameraRight = CameraComponent->GetRightVector();
    const float Dot = FVector::DotProduct(UpDirection, CameraRight);

    if (FMath::Abs(Dot) < 1 - SMALL_NUMBER)
    {
        CurrentRightDirection = FVector::VectorPlaneProject(CameraRight, UpDirection);
    }

    //	const float ControlValue = MovementComponent->IsMovingOnGround() ? ScaleValue : ScaleValue * MovementComponent->AirControlRatio;
    const float ControlValue = ScaleValue;
    AddMovementInput(CurrentRightDirection.GetSafeNormal(), ControlValue, false);
}

void AGroundCameraControllor::AddCameraPitchInput(float ScaleValue)
{
    /*if (SpringArm != nullptr)
    {
        FRotator CameraRelativeRot = SpringArm->GetRelativeRotation();
        const float CameraNewPitch = FMath::ClampAngle(CameraRelativeRot.Pitch + ScaleValue, CameraPitchMin,
                                                       CameraPitchMax);
        CameraRelativeRot.Pitch = CameraNewPitch;
        SpringArm->SetRelativeRotation(CameraRelativeRot);
    }*/
    FRotator CameraRelativeRot = CameraComponent->GetRelativeRotation();
    const float CameraNewPitch = FMath::ClampAngle(CameraRelativeRot.Pitch + ScaleValue, CameraPitchMin,
                                                   CameraPitchMax);
    CameraRelativeRot.Pitch = CameraNewPitch;
    CameraComponent->SetRelativeRotation(CameraRelativeRot);
}

void AGroundCameraControllor::AddCameraYawInput(float ScaleValue)
{
    /*if (SpringArm != nullptr)
    {
        SpringArm->AddRelativeRotation(FRotator(0.0f, ScaleValue, 0.0f));
        //GetMesh()->SetRelativeRotation(MeshRotation);
    }*/
    CameraComponent->AddRelativeRotation(FRotator(0.0f, ScaleValue, 0.0f));
}

void AGroundCameraControllor::EnableDebugging()
{
    if (CapsuleComponent != NULL) { CapsuleComponent->SetHiddenInGame(false); }
    //	if (MovementComponent != NULL) { MovementComponent->EnableDebuging(); }
}

/*void AGroundCameraControllor::SetupPlayerInputComponent(class UInputComponent* InputComponent2)
{
    Super::SetupPlayerInputComponent(InputComponent2);
    // 在按下或松开“Grow”键时做出响应。
    InputComponent2->BindAction("Sprint", IE_Pressed, this, &AGroundCameraControllor::Sprint);
    InputComponent2->BindAction("Sprint", IE_Released, this, &AGroundCameraControllor::StopSprint);
    InputComponent2->BindAction("Jump", IE_Pressed, this, &AGroundCameraControllor::Jump);

    // 对两个移动轴“MoveX”和“MoveY”的值逐帧反应。
    InputComponent2->BindAxis("MoveForward", this, &AGroundCameraControllor::AddForwardMovementInput);
    InputComponent2->BindAxis("MoveRight", this, &AGroundCameraControllor::AddRightMovementInput);

    InputComponent2->BindAxis("LookUp", this, &AGroundCameraControllor::AddCameraPitchInput);
    InputComponent2->BindAxis("Turn", this, &AGroundCameraControllor::AddCameraYawInput);
}*/

void AGroundCameraControllor::DisableDebugging()
{
    if (CapsuleComponent != NULL) { CapsuleComponent->SetHiddenInGame(true); }
    //	if (MovementComponent != NULL) { MovementComponent->DisableDebuging(); }
}

FVector AGroundCameraControllor::GetCurrentForwardDirection() const
{
    return !CurrentForwardDirection.IsZero() ? CurrentForwardDirection : GetActorForwardVector();
}

FVector AGroundCameraControllor::GetCurrentRightDirection() const
{
    return !CurrentRightDirection.IsZero() ? CurrentRightDirection : GetActorRightVector();
}

UPawnMovementComponent* AGroundCameraControllor::GetMovementComponent() const
{
    return MovementComponent;
}

void AGroundCameraControllor::OnKeyUpAction(FKey Key)
{
    if (Key == EKeys::SpaceBar)
    {
    }
    else if (Key == EKeys::LeftShift)
    {
        this->StopSprint();
    }
}

void AGroundCameraControllor::OnKeyDownAction(FKey Key)
{
    if (Key == EKeys::SpaceBar)
    {
        this->Jump();
    }
    else if (Key == EKeys::LeftShift)
    {
        this->Sprint();
    }
}

void AGroundCameraControllor::OnKeyAxisMove(FKey Key, float Axis)
{
    if (Key == EKeys::W || Key == EKeys::S)
    {
        this->AddForwardMovementInput(Axis);
    }
    if (Key == EKeys::A || Key == EKeys::D)
    {
        this->AddRightMovementInput(Axis);
    }
}

void AGroundCameraControllor::OnMouseXMove(float Axis)
{
    this->AddCameraYawInput(Axis);
}

void AGroundCameraControllor::OnMouseYMove(float Axis)
{
    this->AddCameraPitchInput(Axis);
}
