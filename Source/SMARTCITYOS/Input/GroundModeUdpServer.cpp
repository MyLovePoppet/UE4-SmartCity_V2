// Fill out your copyright notice in the Description page of Project Settings.


#include "GroundModeUdpServer.h"

#include "InputPawn.h"

void AGroundModeUdpServer::BeginPlay()
{
    Super::BeginPlay();
    bool isSuccess;
    StartUDPReceiver("GroundMoveServer", "192.168.1.1", EOperationPort::GROUND_MODE, isSuccess);
    if (!isSuccess)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "GroundMoveServer Init falied!");
    }
}

void AGroundModeUdpServer::Handle(const TSharedPtr<FJsonObject>& JsonObject)
{
    FString Type;
    if (JsonObject->TryGetStringField("type", Type))
    {
        switch (UdpServerUtilities::ToEnumType(Type))
        {
        case EOperationType::GROUND_MODE_MOVE:
            {
                float ForwardAxisValue = JsonObject->GetNumberField("VeticalAxis");
                ForwardAxisValue = FMath::Clamp(ForwardAxisValue, -1.f, 1.f);
                //ForwardAxisValue = FMath::Sign(ForwardAxisValue);
                float RightAxisValue = JsonObject->GetNumberField("HorizontalAxis");
                RightAxisValue = FMath::Clamp(RightAxisValue, -1.f, 1.f);
                //RightAxisValue = FMath::Sign(RightAxisValue);
                for (auto& IInputBase : AInputPawn::inputListeners)
                {
                    IInputBase->WAxis(ForwardAxisValue);
                    IInputBase->DAxis(RightAxisValue);
                }
                break;
            }


        case EOperationType::GROUND_MODE_JUMP:
            {
                for (auto& IInputBase : AInputPawn::inputListeners)
                {
                    IInputBase->OnKeyDown(EKeys::SpaceBar);
                }
                break;
            }

        case EOperationType::GROUND_MODE_START_SPRINT:
            {
                for (auto& IInputBase : AInputPawn::inputListeners)
                {
                    IInputBase->OnKeyDown(EKeys::LeftShift);
                }
                break;
            }

        case EOperationType::GROUND_MODE_STOP_SPRINT:
            {
                for (auto& IInputBase : AInputPawn::inputListeners)
                {
                    IInputBase->OnKeyUp(EKeys::LeftShift);
                }
                break;
            }

        case EOperationType::GROUND_MODE_CAMERA_MOVE:
            {
                float YawValue = JsonObject->GetNumberField("distanceX");
                YawValue /= PhonePCScreenSize::PhoneScreenSize.X;
                YawValue = FMath::Clamp(YawValue, -1.f, 1.f) * 180.f;

                float PitchValue = JsonObject->GetNumberField("distanceY");
                PitchValue /= PhonePCScreenSize::PhoneScreenSize.Y;
                PitchValue = FMath::Clamp(PitchValue, -1.f, 1.f) * 180.f;
                for (auto& IInputBase : AInputPawn::inputListeners)
                {
                    IInputBase->MouseX(YawValue);
                    IInputBase->MouseY(PitchValue);
                }
                break;
            }

        default:
            break;
        }
    }
}
