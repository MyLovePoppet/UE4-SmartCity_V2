// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


/**
 * 
 */
class SMARTCITYOS_API GroundBody
{
public:
    GroundBody();
    ~GroundBody();

private:
    FVector ActorUpDirection;
    FVector ActorLocation;

    TPair<FVector, float> CurrentForward;
    TPair<FVector, float> CurrentRight;

public:
    bool bMovementWork = false;
    bool bDoJump = false;
    bool bDoSprint = false;
    bool bStopSprint = false;
	bool bUpdateActorLocation = false;
public:
    FVector GetUpDirection() const;
    FVector GetActorLocation() const;
    TPair<FVector, float> GetCurrentForward() const;
    TPair<FVector, float> GetCurrentRight() const;

public:
    void SetUpDirection(const FVector& _UpDirection);
    void SetCurrentForward(const TPair<FVector, float>& _CurrentForward);
    void SetCurrentRight(const TPair<FVector, float>& _CurrentRight);
    void SetActorLocation(const FVector& _ActorLocation);
};
