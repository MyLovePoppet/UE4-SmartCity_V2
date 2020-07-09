// Fill out your copyright notice in the Description page of Project Settings.


#include "SmartCityOSPortal.h"




extern SMARTCITYOS_API class Synchronize GSynchronize;

// Sets default values
ASmartCityOSPortal::ASmartCityOSPortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;	
}

// Called when the game starts or when spawned
void ASmartCityOSPortal::BeginPlay()
{
	Super::BeginPlay();
	  
	
	//初始化Camera
	GenerateCamera();	


	//初始化CameraState,用于判断视点是否发生变化
	CameraState = SCOSCameraControllor->CameraState;
}

//void ASmartCityOSPortal::PostInitializeComponents() {
//	Super::PostInitializeComponents();
//	//初始化飞行模式
//}

// Called every frame
void ASmartCityOSPortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// if(EarthActor)
	// {
	// 	EarthLocation=EarthActor->GetActorLocation();
	// 	CameraControllor::EarthLocation = EarthLocation;
	// 	CameraState->SetEarthLocation(EarthLocation);
	//
	// }
	if(SCOSCameraControllor == FlyModeCache)
	{
		Synchronize::SynchronizeCameraToUE();
		Synchronize::SynchronizeCameraToSCOS();
	}
	else if (SCOSCameraControllor == GroundModeCache) {
		Synchronize::SynchronizeCameraToUE();
		Synchronize::SynchronizeCameraToSCOS(); 
		Synchronize::GroundModeUpdate();
	}
	//在Tick中判断CameraState是否发生改变，若改变则执行
	Camera NewCameraState = *CameraState;

	//控制大气层是否显示
	ChangeControllorMod();
}

FMatrix CreatMatrixFromString(FString InMatrixStr)
{
	//将字符串按逗号拆分为数组
	TArray<FString> MxStrArr;
	InMatrixStr.ParseIntoArray(MxStrArr,TEXT(","),true);

	//将字符串数组转化为FMatrix
	FMatrix Mx;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			Mx.M[i][j] = FCString::Atof(*MxStrArr[i * 4 + j]);
		}
	}
	return Mx;
}



void ASmartCityOSPortal::GenerateCamera()
{
	//初始化CameraState
	CameraState = new Camera();
	CameraState->SetEarthLocation(EarthLocation);
	GroundBody* BodyState = new GroundBody();
	GSynchronize.CameraState = CameraState;
	GSynchronize.BodyState = BodyState;
	LastCameraState = *CameraState;

	//初始化InputPawn的位置和旋转
// 	const FVector pawnLocation = FVector::ZeroVector;
// 	const FRotator pawnRotation = FRotator::ZeroRotator;
// 	FTransform Transform;
// 	Transform.SetLocation(pawnLocation);
// 	Transform.SetRotation(FQuat(pawnRotation));

	//初始化inputpawn
	FActorSpawnParameters parameters;
	parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	UserInput = GetWorld()->SpawnActor<AInputPawn>(FVector::ZeroVector, FRotator::ZeroRotator,parameters);
	Cast<UCustomGravityPawnMovement>(UserInput->GetMovementComponent())->SetEarthLocation(EarthLocation);
	//UserInput->AddToRoot();
	UECamera = UserInput->GetUECamera();
	GSynchronize.UserInput = UserInput;
	GSynchronize.UECamera = UECamera;
	Synchronize::SynchronizeCameraToSCOS();

	//初始化SCOSCameraControllor
	CameraControllor::CameraState = CameraState;
	CameraControllor::EarthLocation = EarthLocation;
	FlyModeCache = new FlyModeCameraControllor(6378137.f);
	GroundModeCache = new GroundModeCameraControllor(BodyState);
	GroundModeCache->Stop();
	SCOSCameraControllor = FlyModeCache;
	SCOSCameraControllor->Restart();
	 
	Synchronize::SynchronizeCameraToUE();
}

bool ASmartCityOSPortal::IsCameraStateChanged(const Camera & InLastCameraState, const Camera & NewCameraState)
{
	//相机状态改变的判定条件，可根据实际情况增删判断条件
	if (InLastCameraState.GetAspectRatio() != NewCameraState.GetAspectRatio())
		return true;
	if (InLastCameraState.GetFOV() != NewCameraState.GetFOV())
		return true;
	if (!InLastCameraState.GetScreenResolution().Equals(NewCameraState.GetScreenResolution()))
		return true;
	if (!InLastCameraState.GetLocation().Equals(NewCameraState.GetLocation()))
		return true;
	if (!InLastCameraState.GetRotation().Equals(NewCameraState.GetRotation()))
		return true;

	return false;
}

void ASmartCityOSPortal::ChangeControllorMod() 
{
	if (SCOSCameraControllor == FlyModeCache) 
	{
		if (CameraState->GetCameraHeight() < 1000) 
		{
			//顺序不可变，否则多线程下会有同步错误的问题。
			SCOSCameraControllor->Stop();
			GroundModeCache->Restart();
			Synchronize::UpdateActorLocation();
			Synchronize::SynchronizeCameraToUE();
			SCOSCameraControllor = GroundModeCache;

		}
	}
	else if (SCOSCameraControllor == GroundModeCache) 
	{
	}
}