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
    //����UEMap����
    static AActor* MapActor;
    //����Camera����(OS��)
    static Camera* CameraState;
    //Groundģʽ��������̬��Ϣ
    static GroundBody* BodyState;
    //����UE���
    static UCameraComponent* UECamera;

    static AInputPawn* UserInput;

public:
    //��cameraͬ����UE
    static void SynchronizeCameraToUE();
    //��UEcameraͬ����SCOS
    static void SynchronizeCameraToSCOS();

    static void SynchronizeGroundToUE();

    static void SynchronizeGroundToSCOS();
    //ǿ�Ƹ���actor��λ�ã�һ��ֻ���л�CameraModeʱ����
    static void UpdateActorLocation();

    static void GroundModeUpdate();
};
