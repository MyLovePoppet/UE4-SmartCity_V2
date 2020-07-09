// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera.h"
#include "..\Input\InputBase.h"

class SMARTCITYOS_API CameraControllor : public InputBase
{
public:   
	// Sets default values for this pawn's properties
	CameraControllor();
    static Camera* CameraState;
	static FVector EarthLocation;

public:
	virtual void Restart(){this->RegisterListener();};
	virtual void Stop(){this->UnRegisterListener();};
	//原始缩放为1时，地球的大小
	//float OriginRadius = 6378137.f;
};
