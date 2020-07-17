// Fill out your copyright notice in the Description page of Project Settings.


#include "GroundMoveUdpServer.h"

#include "InputPawn.h"

void AGroundMoveUdpServer::BeginPlay()
{
    Super::BeginPlay();
    bool isSuccess;
    StartUDPReceiver("GroundMoveServer", "192.168.1.1", EOperationPort::GROUND_MODE_MOVE, isSuccess);
    if (!isSuccess)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "Ground Move Server Init falied!");
    }
}

void AGroundMoveUdpServer::Handle(const TSharedPtr<FJsonObject>& JsonObject)
{
    FString Type;
    if (JsonObject->TryGetStringField("type", Type))
    {
        switch (UdpServerUtilities::ToEnumType(Type))
        {
        case EOperationType::GROUND_MODE_MOVE:
            {
                float ForwardAxisValue = JsonObject->GetNumberField("VeticalAxis");
                //ForwardAxisValue = FMath::Clamp(ForwardAxisValue, -1.f, 1.f) / 2.f;
                //ForwardAxisValue = FMath::Sign(ForwardAxisValue);
                float RightAxisValue = JsonObject->GetNumberField("HorizontalAxis");
                //RightAxisValue = FMath::Clamp(RightAxisValue, -1.f, 1.f) / 2.f;
                //RightAxisValue = FMath::Sign(RightAxisValue);
                // if (FMath::Abs(ForwardAxisValue) >= 2.f || FMath::Abs(RightAxisValue) >= 2.f)
                // {
                //     for (auto& IInputBase : AInputPawn::inputListeners)
                //     {
                //         //IInputBase->OnKeyDown(EKeys::LeftShift);
                //     }
                // }
                // else
                // {
                //     for (auto& IInputBase : AInputPawn::inputListeners)
                //     {
                //         //IInputBase->OnKeyUp(EKeys::LeftShift);
                //     }
                // }
                for (auto& IInputBase : AInputPawn::inputListeners)
                {
                    if (ForwardAxisValue != 0.f)
                        IInputBase->WAxis(ForwardAxisValue);
                    if (RightAxisValue != 0.f)
                        IInputBase->DAxis(RightAxisValue);
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


        default:
            break;
        }
    }
}
