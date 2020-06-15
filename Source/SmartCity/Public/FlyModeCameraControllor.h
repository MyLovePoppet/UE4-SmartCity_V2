// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraControllorPawn.h"

#include "FlyModeCameraControllor.generated.h"
class AFlyModeCameraControllor;

class CameraManipulator
{
public:
    static bool LineSphereFirstIntersect(const FVector lineOri, const FVector lineDir, const FVector sphereOri,
                                         const float radius, FVector& intersectPt);
    
    void SetHitQueryDistance(const float inHitDist);

    FVector TrackballCenter;
    float TrackballRadius;
    AFlyModeCameraControllor* cameraOwner;
    float hitQueryDistance;
};

/**
 * 
 */
UCLASS()
class SMARTCITY_API AFlyModeCameraControllor : public ACameraControllorPawn
{
    GENERATED_BODY()
    AFlyModeCameraControllor();
//protected:
    //UPROPERTY(EditAnywhere,BlueprintReadWrite)
    //UStaticMeshComponent* StaticMesh;

private:
    uint8 bLeftHold;
    uint8 bMidHold;
    uint8 bRightHold;
    FVector2D oldCursorPt;
    FVector2D currentCursorPt;
    //中间旋转所绕轴
    FVector MidHoldAxis;

    float ArmLength;
    FVector OldLocationOnEarth;
    
    //当前的球体放大倍数
    float NowScale;
public:
    virtual void OnMouseUpAction(FKey Key) override;
    virtual void OnMouseDownAction(FKey Key) override;
    virtual void OnMouseXMove(float Axis) override;
    virtual void OnMouseYMove(float Axis) override;
    virtual void OnScrollWheel(float Axis) override;

    virtual void BeginPlay() override;
protected:

    bool ScreenCursorInfoToWorld(const FVector2D screenCursorPt, FVector& WorldPt, FVector& WorldDir);
    bool CursorPointOnEarth(FVector2D CursorPoint, FVector& intersectPt);
    FVector GetPointToCenterVector(FVector TouchPoint);
    void RotateEarthByAxis(float AngleDeg);
    void CalcDragRotation(const FVector2D inCursorPt, const FVector2D inNextCursorPt);
    void Zoom(float val);
    FVector GetHorizontalVector();
    void ScaleBall(float Sign);
};
