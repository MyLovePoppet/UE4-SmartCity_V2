// Fill out your copyright notice in the Description page of Project Settings.


#include "InputBase.h"
#include "InputPawn.h"


InputBase::InputBase()
{
    RegisterListener();
}
InputBase::~InputBase()
{
    UnRegisterListener();
}

void InputBase::RegisterListener()
{
    AInputPawn::RegisterListener(this);
}

void InputBase::UnRegisterListener()
{
    AInputPawn::UnRegisterListener(this);
}
