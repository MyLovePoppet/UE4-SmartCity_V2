// Fill out your copyright notice in the Description page of Project Settings.


#include "Synchronize.h"
#include "UESceneTools.h"
#include "Kismet/GameplayStatics.h"


extern SMARTCITYOS_API class AUESceneTools GUESceneTools;


Synchronize::Synchronize()
{

}

Synchronize::~Synchronize()
{
}

AActor* Synchronize::MapActor;
//定义Camera对象(OS内)
Camera* Synchronize::CameraState;
//定义UE相机

UCameraComponent* Synchronize::UECamera;

GroundBody* Synchronize::BodyState;

AInputPawn* Synchronize::UserInput;





 void Synchronize::SynchronizeCameraToUE()
 {
    //将本地相机的位置赋给UE相机
    UECamera->SetWorldLocation(CameraState->GetLocation());
    UECamera->SetWorldRotation(CameraState->GetRotation());
	GNearClippingPlane = CameraState->GetNearClipPlane();
	UECamera->UpdateComponentToWorld();
 }


 //如果aspectRatio 与 inviewRect不一致，可能存在隐患
 FMatrix CalculateProjectMatrix(float inFOV,const FVector2D inViewRect,float inNearClippingPlane)
 {
	 // Avoid divide by zero in the projection matrix calculation by clamping FOV
	 float MatrixFOV = FMath::Max(0.001f, inFOV) * (float)PI / 360.0f;
	 float XAxisMultiplier;
	 float YAxisMultiplier;
	 
	 const int32 SizeX = inViewRect.X;
	 const int32 SizeY = inViewRect.Y;

	 // if x is bigger, and we're respecting x or major axis, AND mobile isn't forcing us to be Y axis aligned
	 if (SizeX > SizeY)
	 {
		 //if the viewport is wider than it is tall
		 XAxisMultiplier = 1.0f;
		 YAxisMultiplier = SizeX / (float)SizeY;
	 }
	 else
	 {
		 //if the viewport is taller than it is wide
		 XAxisMultiplier = SizeY / (float)SizeX;
		 YAxisMultiplier = 1.0f;
	 }
	 return FReversedZPerspectiveMatrix(
		 MatrixFOV,
		 MatrixFOV,
		 XAxisMultiplier,
		 YAxisMultiplier,
		 inNearClippingPlane,
		 inNearClippingPlane
	 );
	 
 }

 void Synchronize::SynchronizeCameraToSCOS()
 {
	 CameraState->SetCameraRight(UECamera->GetRightVector());
	 //将UE相机的位置传回本地相机
	 CameraState->SetLocation(UECamera->GetComponentLocation());
	 CameraState->SetRotation(UECamera->GetComponentRotation());
	 CameraState->SetAspectRatio(UECamera->AspectRatio);
	 CameraState->SetScreenResolution(GEngine->GameViewport->Viewport->GetSizeXY());
	 CameraState->SetFOV(UECamera->FieldOfView);
	 CameraState->SetIsPerspectView(UECamera->ProjectionMode == ECameraProjectionMode::Perspective);
	 CameraState->SetUpDirection(UECamera->GetUpVector());

	 CameraState->SetNearClipPlane(GNearClippingPlane);

	 FMatrix TranslateMatrix = FTranslationMatrix(-CameraState->GetLocation());

	 FMatrix  RotationMatrix = FInverseRotationMatrix(CameraState->GetRotation()) * FMatrix(
		 FPlane(0, 0, 1, 0),
		 FPlane(1, 0, 0, 0),
		 FPlane(0, 1, 0, 0),
		 FPlane(0, 0, 0, 1));	 

	 FMatrix ProjectMatrix = CalculateProjectMatrix(CameraState->GetFOV(), CameraState->GetScreenResolution(), CameraState->GetNearClipPlane());

	 FMatrix ViewProjectMatrix = TranslateMatrix * RotationMatrix*ProjectMatrix;

	 FMatrix ViewProjectMatrix1 = GUESceneTools.GetViewProjectionMatrix();

	 if (!ViewProjectMatrix.Equals(ViewProjectMatrix1))
	 {
		 int i = 0;
	 }

	 CameraState->SetViewProjectionMatrix(ViewProjectMatrix);
 }


 void Synchronize::SynchronizeGroundToUE()
 {
	 //Ground修改的信息传到InputPawn
	 Cast<UCustomGravityPawnMovement>(UserInput->GetMovementComponent())->bIsWork = BodyState->bMovementWork;
	 if (BodyState->bDoJump)
	 {
		 UserInput->Jump();
		 BodyState->bDoJump = false;
	 }
	 if (BodyState->bDoSprint)
	 {
		 UserInput->Sprint();
		 BodyState->bDoSprint = false;
	 }
	 if (BodyState->bStopSprint)
	 {
		 UserInput->StopSprint();
		 BodyState->bStopSprint = false;
	 }

	 auto CurrentForward = BodyState->GetCurrentForward();
	 if (FMath::Abs(CurrentForward.Value) > KINDA_SMALL_NUMBER)
	 {
		 UserInput->AddMovementInput(CurrentForward.Key.GetSafeNormal(), CurrentForward.Value, false);
		 CurrentForward.Value = 0.f;
		 BodyState->SetCurrentForward(CurrentForward);
	 }

	 auto CurrentRight = BodyState->GetCurrentRight();
	 if (FMath::Abs(CurrentRight.Value) > KINDA_SMALL_NUMBER)
	 {
		 UserInput->AddMovementInput(CurrentRight.Key.GetSafeNormal(), CurrentRight.Value, false);
		 CurrentRight.Value = 0.f;
		 BodyState->SetCurrentRight(CurrentRight);
	 }
 }

 void Synchronize::SynchronizeGroundToSCOS()
 {
	 //InputPawn的信息同步到BodyState
	 BodyState->SetActorLocation(UserInput->GetActorLocation());
	 BodyState->SetUpDirection(UserInput->GetActorUpVector());
 }

 void Synchronize::UpdateActorLocation() {
	 UserInput->SetActorLocation(BodyState->GetActorLocation());
	 UECamera->SetRelativeLocation(FVector::ZeroVector);
	 UECamera->SetRelativeRotation(FRotator::ZeroRotator);
	 CameraState->SetRelativeRotation(UECamera->GetRelativeRotation());
	 GNearClippingPlane = CameraState->GetNearClipPlane();
 }

 void Synchronize::GroundModeUpdate() {
	 Synchronize::SynchronizeGroundToUE();
	 Synchronize::SynchronizeGroundToSCOS();
	 UECamera->SetRelativeRotation(CameraState->GetRelativeRotation());
	 UECamera->UpdateComponentToWorld();
	 Synchronize::SynchronizeCameraToSCOS();
 }