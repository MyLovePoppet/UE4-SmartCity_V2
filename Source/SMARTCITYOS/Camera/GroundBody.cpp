// Fill out your copyright notice in the Description page of Project Settings.


#include "GroundBody.h"
#include "Kismet/GameplayStatics.h"
GroundBody::GroundBody()
{
}

GroundBody::~GroundBody()
{
}


FVector GroundBody::GetActorLocation() const
{
    return ActorLocation;
}

void GroundBody::SetActorLocation(const FVector& _ActorLocation)
{
    this->ActorLocation = _ActorLocation;
}

FVector GroundBody::GetUpDirection() const
{
    return ActorUpDirection;
}

void GroundBody::SetUpDirection(const FVector& _UpDirection)
{
    this->ActorUpDirection = _UpDirection;
}


TPair<FVector, float> GroundBody::GetCurrentForward() const
{
    return CurrentForward;
}

void GroundBody::SetCurrentForward(const TPair<FVector, float>& _CurrentForward)
{
    this->CurrentForward = _CurrentForward;
}

TPair<FVector, float> GroundBody::GetCurrentRight() const
{
    return CurrentRight;
}

void GroundBody::SetCurrentRight(const TPair<FVector, float>& _CurrentRight)
{
    this->CurrentRight = _CurrentRight;
}