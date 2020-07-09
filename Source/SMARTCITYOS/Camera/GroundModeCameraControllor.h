#pragma once

#include "CoreMinimal.h"
#include "GroundBody.h"
#include "CameraControllor.h"

/**
* 
*/
class SMARTCITYOS_API GroundModeCameraControllor : public CameraControllor
{
public:
    GroundBody* BodyState;
    //注销后重新注册响应事件，将MovementComponent::isWork设为true
    virtual void Restart() override;
    //该函数将注销输入响应事件，将MovementComponent::isWork设为false
    virtual void Stop() override;

    //InputBase Override Begin
    virtual void OnKeyDown(FKey key) override;
    virtual void OnKeyUp(FKey key) override;
    virtual void MouseX(float value) override;
    virtual void MouseY(float value) override;
    virtual void WAxis(float value) override;
    virtual void DAxis(float value) override;
    //InputBase Override End
public:
    GroundModeCameraControllor(GroundBody* _BodyState);
    ~GroundModeCameraControllor();
    void AddForwardMovementInput(float ScaleValue) const;
    void AddRightMovementInput(float ScaleValue) const;
    void AddCameraPitchInput(float ScaleValue) const;
    void AddCameraYawInput(float ScaleValue) const;
    void DoJump() const;
    void DoSprint() const;
    void StopSprint() const;
};
