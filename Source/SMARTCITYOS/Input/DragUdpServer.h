// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UdpServerBase.h"
#include "DragUdpServer.generated.h"

/**
 * 
 */
UCLASS()
class SMARTCITYOS_API ADragUdpServer : public AUdpServerBase
{
    GENERATED_BODY()

protected:
    //FVector2D CurrentLocation;
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    virtual void Handle(const TSharedPtr<FJsonObject>& JsonObject) override;
};
