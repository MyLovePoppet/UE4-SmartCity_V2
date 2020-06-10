// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyModeCameraControllor.h"
#include "Kismet/GameplayStatics.h"
//计算射线与球的第一个交点，通过intersectPt返回，函数返回值确定是否有交点
bool CameraManipulator::LineSphereFirstIntersect(const FVector lineOri, const FVector lineDir, const FVector sphereOri,
                                                 const float radius, FVector& intersectPt)
{
    //计算球心到直线距离，若距离大于球心，不相交
    float distToLine = FMath::PointDistToLine(sphereOri, lineDir, lineOri);
    if (distToLine > radius)
        return false;

    //计算射线起点到球心距离，若小于半径，代表在球内部，返回false
    float distToSphereOri = FVector::Dist(lineOri, sphereOri);
    if (distToSphereOri <= radius)
        return false;

    //联立球体方程、射线方程，求解交点
    FVector unitLineDir = lineDir.GetSafeNormal();
    FVector sphereOriToLineOriV = lineOri - sphereOri;
    float a = FVector::DotProduct(unitLineDir, unitLineDir);
    float b = 2 * FVector::DotProduct(unitLineDir, sphereOriToLineOriV);
    float c = FVector::DotProduct(sphereOriToLineOriV, sphereOriToLineOriV) - radius * radius;
    float b2_4ac = b * b - 4 * a * c;

    if (b2_4ac == 0)
    {
        float t = (b * -1.f) / (2 * a);
        intersectPt = lineOri + lineDir * t;
        return true;
    }
    else if (b2_4ac > 0)
    {
        float t1 = (b * -1.f + FMath::Sqrt(b2_4ac)) / (2 * a);
        float t2 = (b * -1.f - FMath::Sqrt(b2_4ac)) / (2 * a);

        FVector t1_pt = lineOri + lineDir * t1;
        FVector t2_pt = lineOri + lineDir * t2;

        float t1_dist = FVector::Dist(lineOri, t1_pt);
        float t2_dist = FVector::Dist(lineOri, t2_pt);

        intersectPt = (t1_dist > t2_dist) ? t2_pt : t1_pt;
        return true;
    }
    return false;
}

void CameraManipulator::SetHitQueryDistance(const float inHitDist)
{
    this->hitQueryDistance = inHitDist;
}

AFlyModeCameraControllor::AFlyModeCameraControllor(): Super()
{
    // RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    /*SpringArm->SetupAttachment(RootComponent);
    SpringArm->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
    SpringArm->TargetArmLength = 8000.f;
    SpringArm->bEnableCameraLag = true;
    SpringArm->CameraLagSpeed = 6.f;
    SpringArm->bDoCollisionTest = false;*/

    //CameraComponent->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    RootComponent = CameraComponent;
    CameraComponent->bUsePawnControlRotation = false;
    CameraComponent->FieldOfView = 120.f;
    CameraComponent->SetActive(true);

    /*
    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh0"));
    if (StaticMesh)
    {
        //StaticMesh->bCastDynamicShadow = true;
        StaticMesh->bAffectDynamicIndirectLighting = true;
        //StaticMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;
        StaticMesh->SetCollisionProfileName(TEXT("CharacterMesh"));
        StaticMesh->SetGenerateOverlapEvents(false);
        //StaticMesh->SetNotifyRigidBodyCollision(false);
        StaticMesh->SetupAttachment(RootComponent);
    }*/

    bLeftHold = false;
    bMidHold = false;
    bRightHold = false;
}

void AFlyModeCameraControllor::BeginPlay()
{
    Super::BeginPlay();
    ArmLength = EarthRadius * 1.5f;
    CameraComponent->SetRelativeLocation(EarthActor->GetActorLocation() + FVector(1.5, 0, 0) * ArmLength);
    ArmLength = (CameraComponent->GetComponentLocation() - EarthActor->GetActorLocation()).Size();
    CameraComponent->SetRelativeRotation(FVector(-1.5, 0, 0).Rotation());
    UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = true;
}


//根据屏幕坐标获取世界坐标以及方向
bool AFlyModeCameraControllor::ScreenCursorInfoToWorld(const FVector2D screenCursorPt, FVector& WorldPt,
                                                       FVector& WorldDir)
{
    return UGameplayStatics::DeprojectScreenToWorld(
        UGameplayStatics::GetPlayerController(CameraComponent, 0)
        , screenCursorPt, WorldPt, WorldDir);
}

//获取屏幕上点击在地球表面的3D坐标
bool AFlyModeCameraControllor::CursorPointOnEarth(FVector2D CursorPoint, FVector& intersectPt)
{
    FVector worldPt;
    FVector worldDirection;
    ScreenCursorInfoToWorld(CursorPoint, worldPt, worldDirection);

    return CameraManipulator::LineSphereFirstIntersect(worldPt, worldDirection, EarthActor->GetActorLocation(),
                                                       EarthRadius,
                                                       intersectPt);
}

//点到地心方向
FVector AFlyModeCameraControllor::GetPointToCenterVector(FVector TouchPoint)
{
    if (!EarthActor)
    {
        return FVector::ZeroVector;
    }
    FVector Axis = (EarthActor->GetActorLocation() - TouchPoint).GetSafeNormal();
    return Axis;
}

//绕MidHoldAxis轴自旋AngleDeg角度
void AFlyModeCameraControllor::RotateEarthByAxis(float AngleDeg)
{
    if (!EarthActor)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Error!!! EarthActor is NULL！"));
        return;
    }
    if (FMath::Abs(AngleDeg) < KINDA_SMALL_NUMBER)
        return;
    FVector EarthLocation = EarthActor->GetActorLocation();
    //得到绕MidHoldAixs轴旋转AngleDeg角度的四元数。
    FQuat DeltaQuat = FQuat(MidHoldAxis, AngleDeg);
    //必须左乘，不可使用AddRotation函数（内部是右乘）
    FVector TargetLocation = DeltaQuat.UnrotateVector(CameraComponent->GetComponentLocation() - EarthLocation);
    TargetLocation += EarthLocation;
    CameraComponent->SetRelativeLocation(TargetLocation);
    FQuat Target = DeltaQuat.Inverse() * CameraComponent->GetRelativeRotation().Quaternion();
    CameraComponent->SetRelativeRotation(Target);
}

void AFlyModeCameraControllor::CalcDragRotation(const FVector2D inCursorPt, const FVector2D inNextCursorPt)
{
    //获取相机近裁剪面点对应的ue坐标系点
    FVector intersectPt;
    FVector nextIntersectPt;

    const FVector EarthLocation = EarthActor->GetActorLocation();

    if (CursorPointOnEarth(inCursorPt, intersectPt) &&
        CursorPointOnEarth(inNextCursorPt, nextIntersectPt))
    {
        FVector centerToIntersectVec = intersectPt - EarthLocation;
        FVector centerToNextIntersectVec = nextIntersectPt - EarthLocation;
        //求角度差
        FQuat DeltaQuat = FQuat::FindBetween(centerToIntersectVec, centerToNextIntersectVec);
        //旋转臂
        FVector TargetVector = DeltaQuat.UnrotateVector(
            CameraComponent->GetComponentLocation() - EarthLocation);
        CameraComponent->SetRelativeLocation(TargetVector + EarthLocation);
        //旋转角度
        FQuat TargetQuat = DeltaQuat.Inverse() * CameraComponent->GetRelativeRotation().Quaternion();
        CameraComponent->SetRelativeRotation(TargetQuat);
    }
}

void AFlyModeCameraControllor::Zoom(float Speed)
{
    const FVector EarthLocation = EarthActor->GetActorLocation();
    FVector EarthToCamera = CameraComponent->GetComponentLocation() - EarthLocation;

    if (ArmLength <= EarthRadius + 50.0f && Speed > 0)
        return;
    if (ArmLength >= EarthRadius * 1.80f && Speed < 0)
        return;
    //预先计算出放大/缩小后，摄像机将到达的位置，屏幕射线平行（worldDirection不变）
    float DeltaLength = (Speed * (ArmLength - EarthRadius) * 0.1f);

    FVector WorldPt;
    FVector WorldDirection;
    ScreenCursorInfoToWorld(oldCursorPt, WorldPt, WorldDirection);
    FVector IntersectPt;

    //放大/缩小后，屏幕射线与地球交点
    if (!CameraManipulator::LineSphereFirstIntersect(
        WorldPt + DeltaLength * GetPointToCenterVector(CameraComponent->GetComponentLocation()), //当前位置+前进量
        WorldDirection, EarthActor->GetActorLocation(), EarthRadius,
        IntersectPt))
        return;
    const FVector Next = -GetPointToCenterVector(IntersectPt);

    //当前位置屏幕射线与地球交点
    if (!CameraManipulator::LineSphereFirstIntersect(
        WorldPt,
        WorldDirection, EarthActor->GetActorLocation(), EarthRadius,
        IntersectPt))
        return;
    const FVector Now = -GetPointToCenterVector(IntersectPt);

    //根据两交点与地心的向量，求出旋转角。
    FQuat DeltaAngle = FQuat::FindBetweenNormals(Now, Next);
    FVector TargetLocation = DeltaAngle.UnrotateVector(
        CameraComponent->GetComponentLocation() - EarthLocation);

    //调整距离并旋转臂
    ArmLength -= DeltaLength;
    TargetLocation = TargetLocation.GetSafeNormal() * ArmLength + EarthLocation;
    CameraComponent->SetRelativeLocation(TargetLocation);
    //调整角度
    FQuat TargetRotation = DeltaAngle.Inverse() * CameraComponent->GetRelativeRotation().Quaternion();
    CameraComponent->SetRelativeRotation(TargetRotation);
}

void AFlyModeCameraControllor::OnMouseDownAction(FKey Key)
{
    if (Key == EKeys::LeftMouseButton)
    {
        GetWorld()->GetFirstPlayerController()->GetMousePosition(oldCursorPt.X, oldCursorPt.Y);
        bLeftHold = true;
    }
    else if (Key == EKeys::MiddleMouseButton)
    {
        GetWorld()->GetFirstPlayerController()->GetMousePosition(oldCursorPt.X, oldCursorPt.Y);
        FVector intersectPt = FVector::ZeroVector;

        //获取屏幕坐标对应的地球上3D坐标
        CursorPointOnEarth(oldCursorPt, intersectPt);
        OldLocationOnEarth = intersectPt;
        MidHoldAxis = GetPointToCenterVector(intersectPt);

        bMidHold = true;
    }
    else if (Key == EKeys::RightMouseButton)
    {
        GetWorld()->GetFirstPlayerController()->GetMousePosition(oldCursorPt.X, oldCursorPt.Y);
        bRightHold = true;
    }
}

void AFlyModeCameraControllor::OnMouseUpAction(FKey Key)
{
    if (Key == EKeys::LeftMouseButton)
    {
        bLeftHold = false;
    }
    else if (Key == EKeys::MiddleMouseButton)
    {
        bMidHold = false;
    }
    else if (Key == EKeys::RightMouseButton)
    {
        bRightHold = false;
    }
}

void AFlyModeCameraControllor::OnMouseXMove(float Axis)
{
    if (bLeftHold)
    {
        if (FMath::Abs(Axis) > KINDA_SMALL_NUMBER)
        {
            //GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Hold"));

            GetWorld()->GetFirstPlayerController()->GetMousePosition(currentCursorPt.X, currentCursorPt.Y);
            //GEngine->AddOnScreenDebugMessage(0, 3.f, FColor::Green, currentCursorPt.ToString());
            CalcDragRotation(oldCursorPt, currentCursorPt);
            oldCursorPt = currentCursorPt;
        }
    }
    else if (bMidHold)
    {
        if (FMath::Abs(Axis) > KINDA_SMALL_NUMBER)
        {
            GetWorld()->GetFirstPlayerController()->GetMousePosition(currentCursorPt.X, currentCursorPt.Y);
            //鼠标的X坐标变化率转为度数。
            uint32 SizeX = GEngine->GameViewport->Viewport->GetSizeXY().X;
            float AngleDegX = (currentCursorPt.X - oldCursorPt.X) * 5.f / SizeX;
            RotateEarthByAxis(AngleDegX);

            //camer绕鼠标所指向地球上一点作垂直旋转。
            uint32 SizeY = GEngine->GameViewport->Viewport->GetSizeXY().Y;
            float AngleDegY = (currentCursorPt.Y - oldCursorPt.Y) * 10.f / SizeY;
            FVector HorizontalVector = GetHorizontalVector();
            FQuat DeltaQuat = FQuat(HorizontalVector, -AngleDegY);
            //臂旋转
            FVector TargetLocation = DeltaQuat.UnrotateVector(
                CameraComponent->GetComponentLocation() - OldLocationOnEarth);
            TargetLocation += OldLocationOnEarth;
            CameraComponent->SetRelativeLocation(TargetLocation);
            //方向旋转
            FQuat TargetQuat = DeltaQuat.Inverse() * CameraComponent->GetRelativeRotation().Quaternion();
            CameraComponent->SetRelativeRotation(TargetQuat);

            oldCursorPt = currentCursorPt;
        }
    }
    else if (bRightHold)
    {
        const uint32 SizeX = GEngine->GameViewport->Viewport->GetSizeXY().X;
        const uint32 SizeY = GEngine->GameViewport->Viewport->GetSizeXY().Y;
        GetWorld()->GetFirstPlayerController()->GetMousePosition(currentCursorPt.X, currentCursorPt.Y);
        //x偏移
        FVector intersectPt = FVector::ZeroVector;
        //获取屏幕坐标对应的地球上3D坐标
        CursorPointOnEarth(oldCursorPt, intersectPt);
        MidHoldAxis = GetPointToCenterVector(intersectPt);


        const float SpeedX = (currentCursorPt.X - oldCursorPt.X) / SizeY / 20.f;
        RotateEarthByAxis(SpeedX);
        //根据右键y偏移量得速率。
        const float SpeedY = (currentCursorPt.Y - oldCursorPt.Y) / SizeX;
        Zoom(SpeedY);
    }
}

void AFlyModeCameraControllor::OnMouseYMove(float Axis)
{
    OnMouseXMove(Axis);
}

void AFlyModeCameraControllor::OnScrollWheel(float Axis)
{
    if (FMath::Abs(Axis) > KINDA_SMALL_NUMBER)
    {
        GetWorld()->GetFirstPlayerController()->GetMousePosition(oldCursorPt.X, oldCursorPt.Y);
        Zoom(Axis);
    }
}

FVector AFlyModeCameraControllor::GetHorizontalVector()
{
    //屏幕坐标
    FVector2D Left(0, 0), Right(10, 0);
    FVector Left3D, Right3D;
    FVector Useless;
    ScreenCursorInfoToWorld(Left, Left3D, Useless);
    ScreenCursorInfoToWorld(Right, Right3D, Useless);
    return (Left3D - Right3D).GetSafeNormal();
}
