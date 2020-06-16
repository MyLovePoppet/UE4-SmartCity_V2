// Fill out your copyright notice in the Description page of Project Settings.
#include "FlyModeCameraControllor.h"

#include <stack>

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerInput.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
//计算射线与球的第一个交点，通过intersectPt返回，函数返回值确定是否有交点
bool AFlyModeCameraControllor::LineSphereFirstIntersect(const FVector lineOri, const FVector lineDir,
                                                        const FVector sphereOri,
                                                        const float radius, FVector& intersectPt)
{
    //计算球心到直线距离，若距离大于球心，不相交
    double distToLine = FMath::PointDistToLine(sphereOri, lineDir, lineOri);
    if (distToLine > radius)
        return false;

    //计算射线起点到球心距离，若小于半径，代表在球内部，返回false
    double distToSphereOri = FVector::Dist(lineOri, sphereOri);
    if (distToSphereOri <= radius)
        return false;

    //联立球体方程、射线方程，求解交点
    FVector unitLineDir = lineDir.GetSafeNormal();
    FVector sphereOriToLineOriV = lineOri - sphereOri;
    double a = FVector::DotProduct(unitLineDir, unitLineDir);
    double b = 2 * FVector::DotProduct(unitLineDir, sphereOriToLineOriV);
    double c = FVector::DotProduct(sphereOriToLineOriV, sphereOriToLineOriV) - radius * radius;
    double b2_4ac = b * b - 4 * a * c;
    //GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Calculate"));

    if (FMath::Abs(b2_4ac) < KINDA_SMALL_NUMBER)
    {
        double t = (b * -1.f) / (2 * a);
        intersectPt = lineOri + lineDir * t;
        return true;
    }
    else if (b2_4ac > 0)
    {
        double t1 = (b * -1.f + FMath::Sqrt(b2_4ac)) / (2 * a);
        double t2 = (b * -1.f - FMath::Sqrt(b2_4ac)) / (2 * a);

        FVector t1_pt = lineOri + lineDir * t1;
        FVector t2_pt = lineOri + lineDir * t2;

        double t1_dist = FVector::Dist(lineOri, t1_pt);
        double t2_dist = FVector::Dist(lineOri, t2_pt);

        intersectPt = (t1_dist > t2_dist) ? t2_pt : t1_pt;
        return true;
    }
    return false;
    /* 得到玩家的camera方向和位置 *

    const FVector TraceStart = lineOri;
    const FVector Direction = lineDir;
    float MaxUseDistance = (sphereOri - lineOri).Size();
    //FMath::Sqrt((sphereOri - lineOri).SizeSquared() - radius * radius);
    const FVector TraceEnd = TraceStart + (Direction * MaxUseDistance); //计算射线的终点  

    //设置碰撞参数，Tag为一个字符串用于以后识别，true是TraceParams.bTraceComplex，this是InIgnoreActor  
    FCollisionQueryParams TraceParams(FName(TEXT("TraceUsableActor")), true, this);
    TraceParams.bReturnPhysicalMaterial = false;
    //使用复杂Collision判定，逐面判定，更加准确  
    TraceParams.bTraceComplex = true;

    /* FHitResults负责接受结果 *
    FHitResult Hit(ForceInit);
    bool isHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, TraceParams);
    intersectPt = Hit.ImpactPoint;
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, isHit ? TEXT("TrueHit") : TEXT("FalseHit"));
    return isHit;
    */
}

// Sets default values
AFlyModeCameraControllor::AFlyModeCameraControllor()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    RootComponent = CameraComponent;
    CameraComponent->bUsePawnControlRotation = false;
    CameraComponent->FieldOfView = 120.f;
    CameraComponent->SetActive(true);

    AutoPossessPlayer = EAutoReceiveInput::Player0;

    EarthActor = nullptr;
}

//根据屏幕坐标获得对应的地球上3D坐标
bool AFlyModeCameraControllor::CursorPointOnEarth(FVector2D CursorPoint, FVector& intersectPt)
{
    FVector worldPt;
    FVector worldDirection;
    ScreenCursorInfoToWorld(CursorPoint, worldPt, worldDirection);

    return LineSphereFirstIntersect(worldPt, worldDirection, EarthActor->GetActorLocation(),
                                    EarthRadius,
                                    intersectPt);
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
        FVector centerToIntersectVec = (intersectPt - EarthLocation).GetSafeNormal();
        FVector centerToNextIntersectVec = (nextIntersectPt - EarthLocation).GetSafeNormal();
        //求角度差
        FQuat DeltaQuat = FQuat::FindBetweenNormals(centerToIntersectVec, centerToNextIntersectVec);
        //旋转臂
        FVector TargetLocation = DeltaQuat.UnrotateVector(
            CameraComponent->GetComponentLocation() - EarthLocation);
        CameraComponent->SetRelativeLocation(TargetLocation + EarthLocation);
        //旋转角度
        FQuat TargetQuat = DeltaQuat.Inverse() * CameraComponent->GetRelativeRotation().Quaternion();
        CameraComponent->SetRelativeRotation(TargetQuat);
    }
}

bool AFlyModeCameraControllor::ScreenCursorInfoToWorld(const FVector2D screenCursorPt, FVector& WorldPt,
                                                       FVector& WorldDir)
{
    return UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), screenCursorPt,
                                                    WorldPt, WorldDir);
}

// Called when the game starts or when spawned
void AFlyModeCameraControllor::BeginPlay()
{
    Super::BeginPlay();
    EarthRadius = InitScale * OriginRadius;
    FVector InitScale3D = FVector(1, -1, 1) * InitScale;
    if (EarthActor)
    {
        EarthActor->GetRootComponent()->SetRelativeScale3D(InitScale3D);
        CameraComponent->SetRelativeLocation(EarthActor->GetActorLocation() + FVector(2, 0, 0) * EarthRadius);
        ArmLength = (CameraComponent->GetComponentLocation() - EarthActor->GetActorLocation()).Size();
    }
    CameraComponent->SetRelativeRotation(FVector(-1.5, 0, 0).Rotation());
    UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = true;
}

// Called every frame
void AFlyModeCameraControllor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AFlyModeCameraControllor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    //鼠标右键	
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("MouseLeft", EKeys::LeftMouseButton));
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MouseLeft", EKeys::LeftMouseButton, 1.0f));
    PlayerInputComponent->BindAction("MouseLeft", IE_Pressed, this, &AFlyModeCameraControllor::LeftButtonDown);
    PlayerInputComponent->BindAction("MouseLeft", IE_Released, this, &AFlyModeCameraControllor::LeftButtonUp);
    PlayerInputComponent->BindAxis("MouseLeft", this, &AFlyModeCameraControllor::LeftButtonHold);

    //滚轮操作	
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("ScrollWheelUp", EKeys::MouseWheelAxis, 1.0f));
    PlayerInputComponent->BindAxis("ScrollWheelUp", this, &AFlyModeCameraControllor::OnScrollWheelUpPress);

    //鼠标中键
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("MouseMid", EKeys::MiddleMouseButton));
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MouseMid", EKeys::MiddleMouseButton, 1.0f));
    PlayerInputComponent->BindAction("MouseMid", IE_Pressed, this, &AFlyModeCameraControllor::MidButtonDown);
    PlayerInputComponent->BindAction("MouseMid", IE_Released, this, &AFlyModeCameraControllor::MidButtonUp);
    PlayerInputComponent->BindAxis("MouseMid", this, &AFlyModeCameraControllor::MidButtonHold);

    //鼠标右键
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("MouseRight", EKeys::RightMouseButton));
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MouseRight", EKeys::RightMouseButton, 1.0f));
    PlayerInputComponent->BindAction("MouseRight", IE_Pressed, this, &AFlyModeCameraControllor::RightButtonDown);
    //PlayerInputComponent->BindAction("MouseRight", IE_Released, this, &AFlyModeCameraControllor::RightButtonUp);
    PlayerInputComponent->BindAxis("MouseRight", this, &AFlyModeCameraControllor::RightButtonHold);

    //空格
    //UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Space", EKeys::SpaceBar));
    //PlayerInputComponent->BindAction("Space", IE_Pressed, this, &AFlyModeCameraControllor::SpaceScaleBall);
}

void AFlyModeCameraControllor::LeftButtonDown()
{
    GetWorld()->GetFirstPlayerController()->GetMousePosition(oldCursorPt.X, oldCursorPt.Y);
}

void AFlyModeCameraControllor::LeftButtonUp()
{
}

void AFlyModeCameraControllor::LeftButtonHold(float val)
{
    if (FMath::Abs(val) > KINDA_SMALL_NUMBER)
    {
        GetWorld()->GetFirstPlayerController()->GetMousePosition(currentCursorPt.X, currentCursorPt.Y);
        CalcDragRotation(oldCursorPt, currentCursorPt);

        FVector inter;
        CursorPointOnEarth(currentCursorPt, inter);

        oldCursorPt = currentCursorPt;
    }
}


void AFlyModeCameraControllor::OnScrollWheelUpPress(float val)
{
    if (FMath::Abs(val) > KINDA_SMALL_NUMBER)
    {
        GetWorld()->GetFirstPlayerController()->GetMousePosition(oldCursorPt.X, oldCursorPt.Y);
        Zoom(val);
        UpdateCameraState();
    }
    //GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green,FString::SanitizeFloat(val));
}

//放大/缩小,速率由Speed控制。
void AFlyModeCameraControllor::Zoom(const float Speed)
{
    const FVector EarthLocation = EarthActor->GetActorLocation();

    if (ArmLength <= EarthRadius + 11.0f && Speed > 0)
        return;
    if (ArmLength >= EarthRadius * 1.70f && Speed < 0)
        return;
    //预先计算出放大/缩小后，摄像机将到达的位置，屏幕射线平行（worldDirection不变）
    float DeltaLength = (Speed * (ArmLength - EarthRadius) * 0.1f);

    FVector WorldPt;
    FVector WorldDirection;
    ScreenCursorInfoToWorld(oldCursorPt, WorldPt, WorldDirection);
    FVector IntersectPt;

    //放大/缩小后，屏幕射线与地球交点
    if (!LineSphereFirstIntersect(
        WorldPt + DeltaLength * GetPointToCenterVector(CameraComponent->GetComponentLocation()), //当前位置+前进量
        WorldDirection, EarthActor->GetActorLocation(), EarthRadius,
        IntersectPt))
        return;
    const FVector Next = -GetPointToCenterVector(IntersectPt);

    //当前位置屏幕射线与地球交点
    if (!LineSphereFirstIntersect(
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
    ScaleBall(Speed);
    UpdateCameraState();
}


void AFlyModeCameraControllor::MidButtonDown()
{
    GetWorld()->GetFirstPlayerController()->GetMousePosition(oldCursorPt.X, oldCursorPt.Y);
    FVector intersectPt = FVector::ZeroVector;

    //获取屏幕坐标对应的地球上3D坐标
    CursorPointOnEarth(oldCursorPt, intersectPt);
    OldLocationOnEarth = intersectPt;
    MidHoldAxis = GetPointToCenterVector(intersectPt);

    DrawDebugLine(GetWorld(), EarthActor->GetActorLocation(),
                  EarthActor->GetActorLocation() - MidHoldAxis * EarthRadius * 1.5,
                  FColor::Red, true, 20.f);

    UpdateCameraState();
}

void AFlyModeCameraControllor::MidButtonUp()
{
}

void AFlyModeCameraControllor::MidButtonHold(float val)
{
    if (FMath::Abs(val) > KINDA_SMALL_NUMBER)
    {
        GetWorld()->GetFirstPlayerController()->GetMousePosition(currentCursorPt.X, currentCursorPt.Y);
        //鼠标的X坐标变化率转为度数。
        uint32 SizeX = GEngine->GameViewport->Viewport->GetSizeXY().X;
        float AngleDegX = (currentCursorPt.X - oldCursorPt.X) * 5.f / SizeX;
        RotateEarthByAxis(AngleDegX);

        //camer绕鼠标所指向地球上一点作垂面旋转。
        uint32 SizeY = GEngine->GameViewport->Viewport->GetSizeXY().Y;
        float AngleDegY = (currentCursorPt.Y - oldCursorPt.Y) * 3.f / SizeY;
        FVector HorizontalVector = GetHorizontalVector();
        FQuat DeltaQuat = FQuat(HorizontalVector, -AngleDegY);

        //臂旋转
        FVector TargetLocation = DeltaQuat.UnrotateVector(CameraComponent->GetComponentLocation() - OldLocationOnEarth);
        TargetLocation += OldLocationOnEarth;
        ArmLength = (TargetLocation - EarthActor->GetActorLocation()).Size();
        if (ArmLength > EarthRadius + 50)
        {
            CameraComponent->SetRelativeLocation(TargetLocation);
            //方向旋转
            FQuat TargetQuat = DeltaQuat.Inverse() * CameraComponent->GetRelativeRotation().Quaternion();
            CameraComponent->SetRelativeRotation(TargetQuat);
        }
        oldCursorPt = currentCursorPt;
        UpdateCameraState();
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

void AFlyModeCameraControllor::RightButtonDown()
{
    GetWorld()->GetFirstPlayerController()->GetMousePosition(oldCursorPt.X, oldCursorPt.Y);
}


void AFlyModeCameraControllor::RightButtonHold(float val)
{
    if (FMath::Abs(val) > KINDA_SMALL_NUMBER)
    {
        const uint32 SizeX = GEngine->GameViewport->Viewport->GetSizeXY().X;
        const uint32 SizeY = GEngine->GameViewport->Viewport->GetSizeXY().Y;
        GetWorld()->GetFirstPlayerController()->GetMousePosition(currentCursorPt.X, currentCursorPt.Y);
        //x偏移
        FVector intersectPt = FVector::ZeroVector;
        //获取屏幕坐标对应的地球上3D坐标
        CursorPointOnEarth(oldCursorPt, intersectPt);
        MidHoldAxis = GetPointToCenterVector(intersectPt);
        //DrawDebugLine(GetWorld(), EarthActor->GetActorLocation(), EarthActor->GetActorLocation() - MidHoldAxis * 7000,
        //         FColor::Red, true, 20.f);

        const float SpeedX = (currentCursorPt.X - oldCursorPt.X) / SizeY / 20.f;
        RotateEarthByAxis(SpeedX);
        //根据右键y偏移量得速率。
        const float SpeedY = (currentCursorPt.Y - oldCursorPt.Y) / SizeX;
        Zoom(SpeedY);
        UpdateCameraState();
    }
}

//点到地心方向
FVector AFlyModeCameraControllor::GetPointToCenterVector(FVector TouchPoint)
{
    if (!EarthActor)
    {
        GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Red, TEXT("Error!!! EarthActor is NULL！"));
        return FVector::ZeroVector;
    }
    FVector Axis = (EarthActor->GetActorLocation() - TouchPoint).GetSafeNormal();
    return Axis;
}

//让地球绕轴旋转。
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
    //EarthActor->SetActorRotation(quat * EarthActor->GetActorRotation().Quaternion());
    FVector TargetLocation = DeltaQuat.UnrotateVector(CameraComponent->GetComponentLocation() - EarthLocation);
    TargetLocation += EarthLocation;
    CameraComponent->SetRelativeLocation(TargetLocation);
    FQuat Target = DeltaQuat.Inverse() * CameraComponent->GetRelativeRotation().Quaternion();
    CameraComponent->SetRelativeRotation(Target);
}

void AFlyModeCameraControllor::ScaleBall(float Sign)
{
    float DistanceRate = ArmLength / EarthRadius;
    //缩放倍率随相机地球距离比线性改变，
    // 距离比    -> 倍率
    //(1.8-1.1)->(1-1000)
    float k = (1.f - 1000.f) / (1.8f - 1.1f);
    float b = 1.f - 1.8f * k;
    NowScale = k * DistanceRate + b;
    //缩放倍率限制在1~1000
    if (NowScale < 1.f)
    {
        return;
    }
    if (NowScale > 1000.f)
    {
        //如果地球已经是1000.f就不再放大，否则固定到1000.f;因为缩放的步长不是固定值，不能精准到达1000.f。
        if (FMath::IsNearlyEqual(EarthActor->GetRootComponent()->GetRelativeScale3D().X, 1000.f * InitScale))
            return;
        NowScale = 1000.f;
    }
    if (EarthActor)
    {
        //50.f为默认球的默认大小。
        EarthRadius = InitScale * OriginRadius * NowScale;
        ArmLength = DistanceRate * EarthRadius;
        CameraComponent->SetRelativeLocation(
            (CameraComponent->GetComponentLocation() - EarthActor->GetActorLocation()).GetSafeNormal() * ArmLength +
            EarthActor->GetActorLocation());
        EarthActor->GetRootComponent()->SetRelativeScale3D(FVector(1.f, -1.f, 1.f) * NowScale * InitScale);
    }
    GEngine->AddOnScreenDebugMessage(0, 50.f, FColor::Blue,
                                     TEXT("NowScale:") + FString::SanitizeFloat(NowScale * InitScale));
    GEngine->AddOnScreenDebugMessage(1, 50.f, FColor::Red,TEXT("DistRate:") + FString::SanitizeFloat(DistanceRate));
    GEngine->AddOnScreenDebugMessage(2, 50.f, FColor::Green,TEXT("EarthRadius:") + FString::SanitizeFloat(EarthRadius));
}
