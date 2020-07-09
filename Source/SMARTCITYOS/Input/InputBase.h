// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class SMARTCITYOS_API InputBase
{
public:
    static bool bOnLeftControl;
    static bool bOnLeftShift;
    static bool bOnLeftAlt;
protected:
    void RegisterListener();
    void UnRegisterListener();
public:
    InputBase();
    virtual ~InputBase();

    virtual void OnMouseLButtonDown(FVector2D position)
    {
    };

    virtual void OnMouseLButtonUp(FVector2D position)
    {
    };

    virtual void OnMouseRButtonDown(FVector2D position)
    {
    };

    virtual void OnMouseRButtonUp(FVector2D position)
    {
    };

    virtual void OnMouseMidButtonDown(FVector2D position)
    {
    };

    virtual void OnMouseMidButtonUp(FVector2D position)
    {
    };

    virtual void OnMouseWheel(FVector2D position, float value)
    {
    };

    virtual void OnMouseLMove(FVector2D position, float value)
    {
    };

    virtual void OnMouseMMove(FVector2D position, float value)
    {
    };

    virtual void OnMouseRMove(FVector2D position, float value)
    {
    };

    //按下W时value=1.f；S是value=-1.f；
    virtual void WAxis(float value)
    {
    };
    //按下D时value=1.f；S是value=-1.f；
    virtual void DAxis(float value)
    {
    };

    virtual void MouseX(float value)
    {
    };

    virtual void MouseY(float value)
    {
    };

    virtual void OnKeyUp(FKey key)
    {
    };

    virtual void OnKeyDown(FKey key)
    {
    };
};
