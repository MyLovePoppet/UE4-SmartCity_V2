#include "GroundModeCameraControllor.h"
#include "Kismet/GameplayStatics.h"


GroundModeCameraControllor::GroundModeCameraControllor(GroundBody* _BodyState): CameraControllor()
{
    BodyState = _BodyState;
    //GroundModeCameraControllor::Restart();
}

GroundModeCameraControllor::~GroundModeCameraControllor()
{
    BodyState->bMovementWork = false;
    GroundModeCameraControllor::Stop();
}

void GroundModeCameraControllor::Restart()
{
    CameraControllor::Restart();
    BodyState->bMovementWork = true;
    BodyState->SetActorLocation(CameraState->GetLocation()); //移动胶囊体
    CameraState->SetNearClipPlane(10.f);
    //CameraState->SetLocation(BodyState->GetActorLocation());
    UGameplayStatics::GetPlayerController(GWorld, 0)->bShowMouseCursor = false;
    //该设置可能会使得鼠标控制失效，如需改变应修改回去。
    FInputModeGameOnly InputMode;
    UGameplayStatics::GetPlayerController(GWorld, 0)->SetInputMode(InputMode);
}

void GroundModeCameraControllor::Stop()
{
    CameraControllor::Stop();
    BodyState->bMovementWork = false;
}

void GroundModeCameraControllor::AddForwardMovementInput(float ScaleValue) const
{
    const FVector RightDirection = CameraState->GetCameraRight();
    const FVector UpDirection = BodyState->GetUpDirection();
    FQuat RightToForward = FQuat(UpDirection, -3.14159 / 2);
    const FVector CameraForward = RightToForward.RotateVector(RightDirection);
    //const float Dot = FVector::DotProduct(UpDirection, CameraForward);

    //if (FMath::Abs(Dot) < 1 - SMALL_NUMBER)
    {
        FVector CurrentForwardDirection = FVector::VectorPlaneProject(CameraForward, UpDirection);
        TPair<FVector, float> CurrentForward(CurrentForwardDirection, ScaleValue);
        BodyState->SetCurrentForward(CurrentForward);
    }
    //AddMovementInput(CurrentForwardDirection.GetSafeNormal(), ControlValue, false);
}

void GroundModeCameraControllor::AddRightMovementInput(float ScaleValue) const
{
    const FVector UpDirection = BodyState->GetUpDirection();
    const FVector CameraRight = CameraState->GetCameraRight();
    //const float Dot = FVector::DotProduct(UpDirection, CameraRight);
    //if (FMath::Abs(Dot) < 1 - SMALL_NUMBER)
    {
        FVector CurrentRightDirection = FVector::VectorPlaneProject(CameraRight, UpDirection);
        TPair<FVector, float> CurrentRight(CurrentRightDirection, ScaleValue);
        BodyState->SetCurrentRight(CurrentRight);
    }
    //AddMovementInput(CurrentRightDirection.GetSafeNormal(), ControlValue, false);
}

void GroundModeCameraControllor::AddCameraPitchInput(float ScaleValue) const
{
    FRotator CameraRelativeRot = CameraState->GetRelativeRotation();
    const float CameraNewPitch = FMath::ClampAngle(CameraRelativeRot.Pitch + ScaleValue, -89.f,
                                                   89.f);
    CameraRelativeRot.Pitch = CameraNewPitch;
    CameraState->SetRelativeRotation(CameraRelativeRot);
}

void GroundModeCameraControllor::AddCameraYawInput(float ScaleValue) const
{
    FRotator CameraRelativeRot = CameraState->GetRelativeRotation();
    CameraRelativeRot.Yaw += ScaleValue;
    CameraState->SetRelativeRotation(CameraRelativeRot);
}

void GroundModeCameraControllor::DoJump() const
{
    BodyState->bDoJump = true;
}

void GroundModeCameraControllor::DoSprint() const
{
    BodyState->bDoSprint = true;
}

void GroundModeCameraControllor::StopSprint() const
{
    BodyState->bStopSprint = true;
}


//////////////////////////
///InputBase Override
void GroundModeCameraControllor::OnKeyDown(FKey key)
{
    if (key == EKeys::SpaceBar)
    {
        BodyState->bDoJump = true;
    }
    else if (key == EKeys::LeftShift)
    {
        BodyState->bDoSprint = true;
    }
}

void GroundModeCameraControllor::OnKeyUp(FKey key)
{
    if (key == EKeys::LeftShift)
    {
        BodyState->bStopSprint = true;
    }
}

void GroundModeCameraControllor::MouseX(float value)
{
    AddCameraYawInput(value);
}

void GroundModeCameraControllor::MouseY(float value)
{
    AddCameraPitchInput(value);
}

void GroundModeCameraControllor::WAxis(float value)
{
    AddForwardMovementInput(value);
}

void GroundModeCameraControllor::DAxis(float value)
{
    AddRightMovementInput(value);
}
