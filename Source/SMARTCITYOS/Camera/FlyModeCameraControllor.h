// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CameraControllor.h"
#include "CoreMinimal.h"

/**
 * 
 */
class SMARTCITYOS_API FlyModeCameraControllor : public CameraControllor
{
public:
    FlyModeCameraControllor(float _EarthRadius);
    virtual void Restart() override;
    virtual void Stop() override;

protected:
    float ArmLength;
    float OriginRadius = 6378137.f;
    //需要满足鼠标点击不同海拔高度的拖拽同步
    //引入虚拟半径的概念，虚拟半径为”鼠标点击点“到”地心“的距离
    //应用与 bool LineSphereFirstIntersect
    //默认等于地球半径
    float VirtualRadius;

    bool LineSphereFirstIntersect(const FVector LineOri, const FVector LineDir, const FVector SphereOri,
                                  FVector& out_IntersectPt);
    bool ScreenCursorInfoToWorld(const FVector2D screenCursorPt, FVector& WorldPt, FVector& WorldDir);

    void DragRotation(const FVector2D inCursorPt, const FVector2D inNextCursorPt);

    bool CursorPointOnEarth(FVector2D CursorPoint, FVector& intersectPt);

    FVector GetPointToCenterVector(FVector TouchPoint);

    void RotateEarthByAxis(float AngleDeg);

    //void ScaleBall(float Sign = 1.f);

    FVector GetHorizontalVector();

    void Zoom(float Speed);

    void RotateByFVector(FVector centerToIntersectVec, FVector centerToNextIntersectVec);

public:
    //virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void OnMouseLButtonDown(FVector2D position) override;
    virtual void OnMouseLButtonUp(FVector2D position) override;
    void OnMouseLMove(FVector2D position, float value) override;

    virtual void OnMouseRButtonDown(FVector2D position) override;
    virtual void OnMouseRButtonUp(FVector2D position) override;
    void OnMouseRMove(FVector2D position, float value) override;

    virtual void OnMouseMidButtonDown(FVector2D position) override;
    virtual void OnMouseMidButtonUp(FVector2D position) override;
    void OnMouseMMove(FVector2D position, float value) override;

    virtual void OnMouseWheel(FVector2D position, float value) override;

    bool ViewPortToWorldSafety(FVector2D ScreenPos, FVector& out_WorldPos);
    //void MidButtonDown();
    //void MidButtonUp();

    //void RightButtonDown();
    //void RightButtonUp();
    //void RightButtonHold();
    //void OnSpaceBarDown();
    //void OnEnterDown();

    //void OnScrollWheelUpPress(float val);
private:
    FVector2D oldCursorPt;
    FVector2D currentCursorPt;

    //中间旋转所绕轴
    FVector MidHoldAxis;
    FVector OldLocationOnEarth;

    //放入到MouseMove内执行
    bool isLeftButtonHold = false;
    bool isMidButtonHold = false;
    bool isRightButtonHold = false;

    ////缩放插值所需要的变量
    //const uint32 zoomFrameSize = 20;
    //uint32 currentFrame = 0;
    //bool isZooming = false;
    //float currentZoomVal;

    ////旋转插值所需要的变量
    //const uint32 rotateFrameSize = 120;
    //bool isRotating = false;
    //FVector currentLocation;
    //FVector positionLocation;
    //FVector deltaLocation;
    //FVector ShenZhenLocation = FVector(-44090.35f, -122952.766f, 75192.59f);

    ////回到正北正南插值所需要的变量
    //const uint32 resetFrameSize = 20;
    //bool isReseting = false;
    //FRotator currentResetRotation;
    //FRotator deltaResetRotation;
    //FRotator positionResetRotation;
};
