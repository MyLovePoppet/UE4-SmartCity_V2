// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "UESceneTools.h"
#include "SmartCityOS/Camera/Camera.h"
#include "SmartCityOS/Camera/GroundBody.h"
#include "SmartCityOS/Input/InputPawn.h"


/**
 * 
 */
class SMARTCITYOS_API Synchronize
{
public:
	Synchronize();
	~Synchronize();

public:
 	//定义UEMap对象
	static AActor* MapActor;
 	//定义Camera对象(OS内)
	static Camera* CameraState;
	//Ground模式的身体姿态信息
	static GroundBody* BodyState;
 	//定义UE相机
 	static UCameraComponent* UECamera;

	static AInputPawn* UserInput;

 public:
 	//将camera同步到UE
 	static void SynchronizeCameraToUE();
 	//将UEcamera同步到SCOS
 	static void SynchronizeCameraToSCOS();

	static void SynchronizeGroundToUE();

	static void SynchronizeGroundToSCOS();
	//强制更新actor的位置，一般只在切换CameraMode时调用
	static void UpdateActorLocation();

	static void GroundModeUpdate();
	
};
