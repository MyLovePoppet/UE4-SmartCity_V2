// Fill out your copyright notice in the Description page of Project Settings.
#include "FlyModeCameraControllor.h"
#include "Kismet/GameplayStatics.h"
#include "SMARTCITYOS/Core/UESceneTools.h"

// Sets default values
FlyModeCameraControllor::FlyModeCameraControllor(float _EarthRadius) : CameraControllor()
{
    OriginRadius = _EarthRadius;
    VirtualRadius = OriginRadius;
    //FlyModeCameraControllor::Restart();
}

void FlyModeCameraControllor::Restart()
{
    CameraControllor::Restart();
	FVector ShenZhenLocation = FVector(-44090.35f, -122952.766f, 75192.59f);
	//FVector ShenZhenLocation = FVector(1, 0, 0);
    CameraState->SetLocation(EarthLocation + 1.5f * ShenZhenLocation.GetSafeNormal() * VirtualRadius);
    CameraState->SetRotation(GetPointToCenterVector(CameraState->GetLocation()).Rotation());

    ArmLength = (CameraState->GetLocation() - EarthLocation).Size();

    UGameplayStatics::GetPlayerController(GWorld, 0)->bShowMouseCursor = true;
    //GEngine->Exec(GWorld, TEXT("r.SetNearClipPlane 100.0"), *GLog);
    CameraState->SetNearClipPlane(100.0);
}

void FlyModeCameraControllor::Stop()
{
    CameraControllor::Stop();
}

//计算射线与球的第一个交点，通过intersectPt返回，函数返回值确定是否有交点
bool FlyModeCameraControllor::LineSphereFirstIntersect(const FVector LineOri, const FVector LineDir,
                                                       const FVector SphereOri, FVector& out_IntersectPt)
{
    //计算球心到直线距离，若距离大于球心，不相交
    double distToLine = FMath::PointDistToLine(SphereOri, LineDir, LineOri);
    if (distToLine > VirtualRadius)
        return false;

    //计算射线起点到球心距离，若小于半径，代表在球内部，返回false
    double distToSphereOri = FVector::Dist(LineOri, SphereOri);
    if (distToSphereOri <= VirtualRadius)
        return false;

    //联立球体方程、射线方程，求解交点
    FVector unitLineDir = LineDir.GetSafeNormal();
    FVector sphereOriToLineOriV = LineOri - SphereOri;
    double a = FVector::DotProduct(unitLineDir, unitLineDir);
    double b = 2 * FVector::DotProduct(unitLineDir, sphereOriToLineOriV);
    double c = FVector::DotProduct(sphereOriToLineOriV, sphereOriToLineOriV) - VirtualRadius * VirtualRadius;
    double b2_4ac = b * b - 4 * a * c;
    //GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Calculate"));

    if (FMath::Abs(b2_4ac) < KINDA_SMALL_NUMBER)
    {
        double t = (b * -1.f) / (2 * a);
        out_IntersectPt = LineOri + LineDir * t;
        return true;
    }
    else if (b2_4ac > 0)
    {
        double t1 = (b * -1.f + FMath::Sqrt(b2_4ac)) / (2 * a);
        double t2 = (b * -1.f - FMath::Sqrt(b2_4ac)) / (2 * a);

        FVector t1_pt = LineOri + LineDir * t1;
        FVector t2_pt = LineOri + LineDir * t2;

        double t1_dist = FVector::Dist(LineOri, t1_pt);
        double t2_dist = FVector::Dist(LineOri, t2_pt);

        out_IntersectPt = (t1_dist > t2_dist) ? t2_pt : t1_pt;
        return true;
    }
    return false;
}


bool FlyModeCameraControllor::ViewPortToWorldSafety(FVector2D ScreenPos, FVector& out_WorldPos)
{
    const bool bSuccess = AUESceneTools::ViewPortToWorld(ScreenPos, out_WorldPos);
    const bool bAboveSurface = (out_WorldPos - EarthLocation).SizeSquared() - 2 * OriginRadius * OriginRadius < 0;
    return bSuccess && bAboveSurface;
}

//根据屏幕坐标获得对应的地球上3D坐标
bool FlyModeCameraControllor::CursorPointOnEarth(FVector2D CursorPoint, FVector& intersectPt)
{
    FVector worldPt;
    FVector worldDirection;
    ScreenCursorInfoToWorld(CursorPoint, worldPt, worldDirection);

    return LineSphereFirstIntersect(worldPt, worldDirection, EarthLocation,
                                    intersectPt);
}

//鼠标拖拽
void FlyModeCameraControllor::DragRotation(const FVector2D inCursorPt, const FVector2D inNextCursorPt)
{
    //获取相机近裁剪面点对应的ue坐标系点
    FVector intersectPt;
    FVector nextIntersectPt;

    if (CursorPointOnEarth(inCursorPt, intersectPt) &&
        CursorPointOnEarth(inNextCursorPt, nextIntersectPt))
    {
        FVector centerToIntersectVec = (intersectPt - EarthLocation).GetSafeNormal();
        FVector centerToNextIntersectVec = (nextIntersectPt - EarthLocation).GetSafeNormal();
        //求角度差
        FQuat DeltaQuat = FQuat::FindBetweenNormals(centerToIntersectVec, centerToNextIntersectVec);
        //旋转臂
        FVector TargetLocation = DeltaQuat.UnrotateVector(
            CameraState->GetLocation() - EarthLocation);
        CameraState->SetLocation(TargetLocation + EarthLocation);
        //旋转角度
        FQuat TargetQuat = DeltaQuat.Inverse() * CameraState->GetRotation().Quaternion();
        CameraState->SetRotation(TargetQuat);
    }
}

//屏幕坐标转世界坐标
bool FlyModeCameraControllor::ScreenCursorInfoToWorld(const FVector2D screenCursorPt, FVector& WorldPt,
                                                      FVector& WorldDir)
{
    return UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(GWorld, 0),
                                                    screenCursorPt,
                                                    WorldPt, WorldDir);
}

void FlyModeCameraControllor::OnMouseLButtonDown(FVector2D position)
{
    oldCursorPt = position;
    FVector WorldPos;
    if (ViewPortToWorldSafety(oldCursorPt, WorldPos))
    {
        VirtualRadius = (WorldPos - EarthLocation).Size();
        isLeftButtonHold = true;
    }
}

void FlyModeCameraControllor::OnMouseLButtonUp(FVector2D position)
{
    VirtualRadius = OriginRadius;
    isLeftButtonHold = false;
}

void FlyModeCameraControllor::OnMouseLMove(FVector2D position, float value)
{
    if (isLeftButtonHold)
    {
        currentCursorPt = position;
        DragRotation(oldCursorPt, currentCursorPt);
        oldCursorPt = currentCursorPt;
    }
}

void FlyModeCameraControllor::OnMouseMidButtonDown(FVector2D position)
{
    oldMidCursorPt = position;
    FVector WorldPos = FVector::ZeroVector;
    if (ViewPortToWorldSafety(oldMidCursorPt, WorldPos))
    {
        VirtualRadius = (WorldPos - EarthLocation).Size();
        //获取屏幕坐标对应的地球上3D坐标
        OldLocationOnEarth = WorldPos;
        MidHoldAxis = GetPointToCenterVector(WorldPos);
        isMidButtonHold = true;
    }
}

void FlyModeCameraControllor::OnMouseMidButtonUp(FVector2D position)
{
    VirtualRadius = OriginRadius;
    isMidButtonHold = false;
}

void FlyModeCameraControllor::OnMouseMMove(FVector2D position, float value)
{
    if (isMidButtonHold)
    {
        currentMidCursorPt = position;
        //X轴:鼠标的X坐标变化率转为度数。
        uint32 SizeX = GEngine->GameViewport->Viewport->GetSizeXY().X;
        float AngleDegX = (currentMidCursorPt.X - oldMidCursorPt.X) * 5.f / SizeX;
        RotateEarthByAxis(AngleDegX);

        //Y轴:camer绕鼠标所指向地球上一点作垂面旋转。
        // uint32 SizeY = GEngine->GameViewport->Viewport->GetSizeXY().Y;
        // float AngleDegY = (currentCursorPt.Y - oldCursorPt.Y) * 3.f / SizeY;
        // FVector HorizontalVector = GetHorizontalVector();
        // FQuat DeltaQuat = FQuat(HorizontalVector, -AngleDegY);
        // //臂旋转
        // FVector TargetLocation = DeltaQuat.UnrotateVector(CameraState->GetLocation() - OldLocationOnEarth);
        // TargetLocation += OldLocationOnEarth;
        // ArmLength = (TargetLocation - EarthLocation).Size();
        // if (ArmLength > OriginRadius + 50)
        // {
        //     CameraState->SetLocation(TargetLocation);
        //     //方向旋转
        //     FQuat TargetQuat = DeltaQuat.Inverse() * CameraState->GetRotation().Quaternion();
        //     CameraState->SetRotation(TargetQuat);
        // }
        oldMidCursorPt = currentMidCursorPt;
    }
}

void FlyModeCameraControllor::OnMouseRButtonDown(FVector2D position)
{
    oldCursorPt = position;
    FVector WorldPos = FVector::ZeroVector;

    if (ViewPortToWorldSafety(oldCursorPt, WorldPos))
    {
        VirtualRadius = (WorldPos - EarthLocation).Size();
        //GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple,TEXT("isRightButtonHold"));
        //DrawDebugSphere(GetWorld(), WorldPos, 20, 50, FColor::Red, false, 50);
        MidHoldAxis = GetPointToCenterVector(WorldPos);
        isRightButtonHold = true;
    }
}

void FlyModeCameraControllor::OnMouseRButtonUp(FVector2D position)
{
    VirtualRadius = OriginRadius;
    isRightButtonHold = false;
}

void FlyModeCameraControllor::OnMouseRMove(FVector2D position, float value)
{
    if (isRightButtonHold)
    {
        currentCursorPt = position;
        const uint32 SizeX = GEngine->GameViewport->Viewport->GetSizeXY().X;
        const uint32 SizeY = GEngine->GameViewport->Viewport->GetSizeXY().Y;
        //x偏移
        const float SpeedX = (currentCursorPt.X - oldCursorPt.X) / SizeX / 20.f;
        RotateEarthByAxis(SpeedX);
        //根据右键y偏移量得速率。
        float SpeedY = (currentCursorPt.Y - oldCursorPt.Y) / SizeY;
        //SpeedY = FMath::Clamp(SpeedY, -0.1f, 0.1f);
        Zoom(SpeedY);
    }
}

void FlyModeCameraControllor::OnMouseWheel(FVector2D position, float value)
{
    FVector WorldPos;
    if (FMath::Abs(value) > 1e-6)
    {
        oldCursorPt = position;
        if (ViewPortToWorldSafety(oldCursorPt, WorldPos))
        {
            VirtualRadius = (WorldPos - EarthLocation).Size();
            Zoom(value);
        }
    }
}

//放大/缩小,速率由Speed控制。
void FlyModeCameraControllor::Zoom(const float Speed)
{
    //GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString::SanitizeFloat(Speed));

    if (ArmLength <= VirtualRadius + 100.0f && Speed > 0)
        return;
    if (ArmLength >= OriginRadius * 2.0f && Speed < 0)
        return;
    //预先计算出放大/缩小后，摄像机将到达的位置，屏幕射线平行（worldDirection不变）
    float DeltaLength = (Speed * (ArmLength - VirtualRadius) * 0.1f);

    FVector WorldPt;
    FVector WorldDirection;

    ScreenCursorInfoToWorld(oldCursorPt, WorldPt, WorldDirection);
    FVector IntersectPt;

    //放大/缩小后，屏幕射线与地球交点
    if (!LineSphereFirstIntersect(
        WorldPt + DeltaLength * GetPointToCenterVector(CameraState->GetLocation()), //当前位置+前进量
        WorldDirection, EarthLocation, IntersectPt))
        return;
    const FVector Next = -GetPointToCenterVector(IntersectPt);

    //当前位置屏幕射线与地球交点
    if (!LineSphereFirstIntersect(
        WorldPt,
        WorldDirection, EarthLocation, IntersectPt))
        return;
    const FVector Now = -GetPointToCenterVector(IntersectPt);

    //根据两交点与地心的向量，求出旋转角。
    FQuat DeltaAngle = FQuat::FindBetweenNormals(Now, Next);
    FVector TargetLocation = DeltaAngle.UnrotateVector(
        CameraState->GetLocation() - EarthLocation);

    //调整距离并旋转臂
    ArmLength -= DeltaLength;
    TargetLocation = TargetLocation.GetSafeNormal() * ArmLength + EarthLocation;
    CameraState->SetLocation(TargetLocation);
    //调整角度
    FQuat TargetRotation = DeltaAngle.Inverse() * CameraState->GetRotation().Quaternion();
    CameraState->SetRotation(TargetRotation);
    //ScaleBall(Speed);
    //
}

FVector FlyModeCameraControllor::GetHorizontalVector()
{
    //屏幕坐标
    FVector2D Left(0, 0), Right(10, 0);
    FVector Left3D, Right3D;
    FVector Useless;
    ScreenCursorInfoToWorld(Left, Left3D, Useless);
    ScreenCursorInfoToWorld(Right, Right3D, Useless);
    return (Left3D - Right3D).GetSafeNormal();
}

//点到地心方向
FVector FlyModeCameraControllor::GetPointToCenterVector(FVector TouchPoint)
{
    FVector Axis = (EarthLocation - TouchPoint).GetSafeNormal();
    return Axis;
}

//让地球绕轴旋转。
void FlyModeCameraControllor::RotateEarthByAxis(float AngleDeg)
{
    if (FMath::Abs(AngleDeg) < KINDA_SMALL_NUMBER)
        return;
    //得到绕MidHoldAixs轴旋转AngleDeg角度的四元数。
    FQuat DeltaQuat = FQuat(MidHoldAxis, AngleDeg);
    //必须左乘，不可使用AddRotation函数（内部是右乘）
    FVector TargetLocation = DeltaQuat.UnrotateVector(CameraState->GetLocation() - EarthLocation);
    TargetLocation += EarthLocation;
    CameraState->SetLocation(TargetLocation);
    FQuat Target = DeltaQuat.Inverse() * CameraState->GetRotation().Quaternion();
    CameraState->SetRotation(Target);
}

void FlyModeCameraControllor::RotateByFVector(FVector centerToIntersectVec, FVector centerToNextIntersectVec)
{
    centerToIntersectVec = centerToIntersectVec.GetSafeNormal();
    centerToNextIntersectVec = centerToNextIntersectVec.GetSafeNormal();
    //求角度差
    FQuat DeltaQuat = FQuat::FindBetweenNormals(centerToIntersectVec, centerToNextIntersectVec);
    //
    //旋转臂
    FVector TargetLocation = DeltaQuat.RotateVector(
        CameraState->GetLocation() - EarthLocation);
    CameraState->SetLocation(TargetLocation + EarthLocation);
    //旋转角度
    FQuat TargetQuat = DeltaQuat * CameraState->GetRotation().Quaternion();
    FRotator rotator = TargetQuat.Rotator();
    rotator.Roll = 0.0f;
    CameraState->SetRotation(rotator);
}
