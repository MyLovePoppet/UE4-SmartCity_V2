// Fill out your copyright notice in the Description page of Project Settings.


#include "GroundCameraUdpServer.h"

#include "InputPawn.h"

void AGroundCameraUdpServer::BeginPlay()
{
    Super::BeginPlay();
    bool isSuccess;
    StartUDPReceiver("GroundMoveServer", "127.0.0.1", EOperationPort::GROUND_MODE_CAMERA, isSuccess);
    if (!isSuccess)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "Ground Camera Move Server Init falied!");
    }
}

void AGroundCameraUdpServer::Handle(const TSharedPtr<FJsonObject>& JsonObject)
{
    FString Type;
    if (JsonObject->TryGetStringField("type", Type))
    {
        switch (UdpServerUtilities::ToEnumType(Type))
        {
        case EOperationType::GROUND_MODE_CAMERA_MOVE:
            {
                float YawValue = JsonObject->GetNumberField("distanceX");
                YawValue = FMath::Clamp(YawValue, -1.f, 1.f);

                float PitchValue = JsonObject->GetNumberField("distanceY");
                PitchValue = FMath::Clamp(PitchValue, -1.f, 1.f);
                for (auto& IInputBase : AInputPawn::inputListeners)
                {
                    IInputBase->MouseX(YawValue);
                    IInputBase->MouseY(PitchValue);
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
        default:
            break;
        }
    }
}
