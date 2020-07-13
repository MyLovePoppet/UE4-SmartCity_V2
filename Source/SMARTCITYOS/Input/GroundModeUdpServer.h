// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UdpServerBase.h"
#include "GroundModeUdpServer.generated.h"

/**
 * 
 */
UCLASS()
class SMARTCITYOS_API AGroundModeUdpServer : public AUdpServerBase
{
	GENERATED_BODY()
	protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

	virtual void Handle(const TSharedPtr<FJsonObject>& JsonObject) override;
};
