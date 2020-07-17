// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UdpServerBase.h"
#include "GroundCameraUdpServer.generated.h"

/**
 * 
 */
UCLASS()
class SMARTCITYOS_API AGroundCameraUdpServer : public AUdpServerBase
{
    GENERATED_BODY()
    virtual void BeginPlay() override;

    virtual void Handle(const TSharedPtr<FJsonObject>& JsonObject) override;
};
