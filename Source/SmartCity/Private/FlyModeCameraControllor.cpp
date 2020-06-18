// Fill out your copyright notice in the Description page of Project Settings.
#include "FlyModeCameraControllor.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerInput.h"
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
        CameraComponent->SetRelativeLocation(EarthActor->GetActorLocation() + FVector(1.5, 0, 0) * EarthRadius);
        ArmLength = (CameraComponent->GetComponentLocation() - EarthActor->GetActorLocation()).Size();
    }
    CameraComponent->SetRelativeRotation(FVector(-1.5, 0, 0).Rotation());
    UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = true;

    //初始化中国深圳视角向量
    currentLocation = CameraComponent->GetRelativeLocation() - EarthActor->GetActorLocation();
    ShenZhenLocation = FVector(-44090.35f, -122952.766f, 75192.59f) - EarthActor->GetActorLocation();
    OnSpaceBarDown();
    //deltaLocation=(positionLocation-currentLocation)/frameSize;
}

// Called every frame
void AFlyModeCameraControllor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    //相机滚轮操作
    if (isZooming)
    {
        Zoom(currentZoomVal / zoomFrameSize);
        currentFrame++;
        if (currentFrame == zoomFrameSize)
        {
            isZooming = false;
            currentFrame = 0;
        }
        return;
    }
    //相机移动到中国视角
    if (isRotating)
    {
        RotateByFVector(currentLocation, currentLocation + deltaLocation);
        currentLocation += deltaLocation;
        currentFrame++;

        if (currentFrame == rotateFrameSize)
        {
            isRotating = false;
            currentFrame = 0;
        }
        return;
    }
    //相机回到正北正南
    if (isReseting)
    {
        currentResetRotation += deltaResetRotation;
        CameraComponent->SetRelativeRotation(currentResetRotation);
        currentFrame++;
        if (currentFrame == resetFrameSize)
        {
            isReseting = false;
            currentFrame = 0;
        }
        return;
    }
    if (isLeftButtonHold)
    {
        LeftButtonHold();
    }
    if (isMidButtonHold)
    {
        MidButtonHold();
    }
    if (isRightButtonHold)
    {
        RightButtonHold();
    }
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
    //PlayerInputComponent->BindAxis("MouseLeft", this, &AFlyModeCameraControllor::LeftButtonHold);

    //滚轮操作	
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("ScrollWheelUp", EKeys::MouseWheelAxis, 1.0f));
    PlayerInputComponent->BindAxis("ScrollWheelUp", this, &AFlyModeCameraControllor::OnScrollWheelUpPress);

    //鼠标中键
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("MouseMid", EKeys::MiddleMouseButton));
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MouseMid", EKeys::MiddleMouseButton, 1.0f));
    PlayerInputComponent->BindAction("MouseMid", IE_Pressed, this, &AFlyModeCameraControllor::MidButtonDown);
    PlayerInputComponent->BindAction("MouseMid", IE_Released, this, &AFlyModeCameraControllor::MidButtonUp);
    //PlayerInputComponent->BindAxis("MouseMid", this, &AFlyModeCameraControllor::MidButtonHold);

    //鼠标右键
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("MouseRight", EKeys::RightMouseButton));
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MouseRight", EKeys::RightMouseButton, 1.0f));
    PlayerInputComponent->BindAction("MouseRight", IE_Pressed, this, &AFlyModeCameraControllor::RightButtonDown);
    PlayerInputComponent->BindAction("MouseRight", IE_Released, this, &AFlyModeCameraControllor::RightButtonUp);
    //空格
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("SpaceBar", EKeys::SpaceBar));
    PlayerInputComponent->BindAction("SpaceBar", IE_Pressed, this, &AFlyModeCameraControllor::OnSpaceBarDown);
    //回车
    UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Enter", EKeys::Enter));
    PlayerInputComponent->BindAction("Enter", IE_Pressed, this, &AFlyModeCameraControllor::OnEnterDown);
}

void AFlyModeCameraControllor::LeftButtonDown()
{
    //深度缓存
    // FVector2D currentPt;
    // GetWorld()->GetFirstPlayerController()->GetMousePosition(currentPt.X, currentPt.Y);
    // float depth = GetDepth(currentPt);
    // GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Green, FString::SanitizeFloat(depth));
    GetWorld()->GetFirstPlayerController()->GetMousePosition(oldCursorPt.X, oldCursorPt.Y);
    isLeftButtonHold = true;
}

void AFlyModeCameraControllor::LeftButtonUp()
{
    isLeftButtonHold = false;
}

void AFlyModeCameraControllor::LeftButtonHold()
{
    GetWorld()->GetFirstPlayerController()->GetMousePosition(currentCursorPt.X, currentCursorPt.Y);
    CalcDragRotation(oldCursorPt, currentCursorPt);

    FVector inter;
    CursorPointOnEarth(currentCursorPt, inter);

    oldCursorPt = currentCursorPt;
}

void AFlyModeCameraControllor::MidButtonDown()
{
    GetWorld()->GetFirstPlayerController()->GetMousePosition(oldCursorPt.X, oldCursorPt.Y);
    FVector intersectPt = FVector::ZeroVector;

    //获取屏幕坐标对应的地球上3D坐标
    if (CursorPointOnEarth(oldCursorPt, intersectPt))
    {
        OldLocationOnEarth = intersectPt;
        MidHoldAxis = GetPointToCenterVector(intersectPt);
        isMidButtonHold = true;
    }
}

void AFlyModeCameraControllor::MidButtonUp()
{
    isMidButtonHold = false;
}

void AFlyModeCameraControllor::MidButtonHold()
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

void AFlyModeCameraControllor::RightButtonDown()
{
    GetWorld()->GetFirstPlayerController()->GetMousePosition(oldCursorPt.X, oldCursorPt.Y);
    isRightButtonHold = true;
}

void AFlyModeCameraControllor::RightButtonUp()
{
    isRightButtonHold = false;
}

void AFlyModeCameraControllor::RightButtonHold()
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
    float SpeedY = (currentCursorPt.Y - oldCursorPt.Y) / SizeX;
    //GEngine->AddOnScreenDebugMessage(-1,1.0f,FColor::White,FString::SanitizeFloat(SpeedY));
    SpeedY = FMath::Clamp(SpeedY, -0.1f, 0.1f);
    Zoom(SpeedY);
    UpdateCameraState();
}
//空格键按下，回到中国视角，放入到Tick内进行平滑处理
void AFlyModeCameraControllor::OnSpaceBarDown()
{
    currentLocation = CameraComponent->GetRelativeLocation() - EarthActor->GetActorLocation();
    currentFrame = 0;
    positionLocation = ShenZhenLocation;
    currentLocation = currentLocation.GetSafeNormal();
    positionLocation = positionLocation.GetSafeNormal();
    deltaLocation = (positionLocation - currentLocation) / rotateFrameSize;
    isRotating = true;
}
//回车键按下，回到正北正南，放入到Tick内进行平滑处理
void AFlyModeCameraControllor::OnEnterDown()
{
    currentResetRotation = CameraComponent->GetRelativeRotation();
    FVector CameraToEarth = GetPointToCenterVector(CameraComponent->GetRelativeLocation());
    FRotator rotator = CameraToEarth.Rotation();
    positionResetRotation = rotator;
    deltaResetRotation = (positionResetRotation - currentResetRotation) * (1.0f / resetFrameSize);
    isReseting = true;
    UpdateCameraState();
}

void AFlyModeCameraControllor::OnScrollWheelUpPress(float val)
{
    if (FMath::Abs(val) > KINDA_SMALL_NUMBER)
    {
        //放入到Tick内进行平滑处理
        if (!isZooming)
        {
            GetWorld()->GetFirstPlayerController()->GetMousePosition(oldCursorPt.X, oldCursorPt.Y);
            //GEngine->AddOnScreenDebugMessage(-1,1.0f,FColor::White,FString::SanitizeFloat(val));
            currentZoomVal = FMath::Clamp(val,-1.f,1.0f);
            isZooming = true;
            currentFrame = 0;
        }
    }
    //GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green,FString::SanitizeFloat(val));
}

//放大/缩小,速率由Speed控制。
void AFlyModeCameraControllor::Zoom(const float Speed)
{
    GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Red, FString::SanitizeFloat(Speed));
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

void AFlyModeCameraControllor::ScaleBall(float Rate)
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

void AFlyModeCameraControllor::RotateByFVector(FVector centerToIntersectVec, FVector centerToNextIntersectVec)
{
    centerToIntersectVec = centerToIntersectVec.GetSafeNormal();
    centerToNextIntersectVec = centerToNextIntersectVec.GetSafeNormal();
    FVector EarthLocation = EarthActor->GetActorLocation();
    //求角度差
    FQuat DeltaQuat = FQuat::FindBetweenNormals(centerToIntersectVec, centerToNextIntersectVec);
    //
    //旋转臂
    FVector TargetLocation = DeltaQuat.RotateVector(
        CameraComponent->GetComponentLocation() - EarthLocation);
    CameraComponent->SetRelativeLocation(TargetLocation + EarthLocation);
    //旋转角度
    FQuat TargetQuat = DeltaQuat * CameraComponent->GetRelativeRotation().Quaternion();
    FRotator rotator = TargetQuat.Rotator();
    rotator.Roll = 0.0f;
    CameraComponent->SetRelativeRotation(rotator);
}
