// Fill out your copyright notice in the Description page of Project Settings.


#include "DragUdpServer.h"

#include "InputPawn.h"

void ADragUdpServer::BeginPlay()
{
    Super::BeginPlay();
    bool isSuccess;
    StartUDPReceiver("DragUDPServer", "192.168.1.1", EOperationPort::FLY_MODE_DRAG, isSuccess);
    if (!isSuccess)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "Init falied!");
    }
}

void ADragUdpServer::Handle(const TSharedPtr<FJsonObject>& JsonObject)
{
    //Super::Handle(JsonObject);
    FString Type;
    if (JsonObject->TryGetStringField("type", Type))
    {
        switch (UdpServerUtilities::ToEnumType(Type))
        {
        case EOperationType::FLY_MODE_DRAG:
            {
                float X = JsonObject->GetNumberField("coordinateX");
                float Y = JsonObject->GetNumberField("coordinateY");
                //FVector2D PCLocation = CurrentLocation + FVector2D(X, Y);
                //CurrentLocation = PCLocation;
                FVector2D PCLocation = UdpServerUtilities::ToPCLocation(FVector2D(X, Y));
                for (auto& IInputBase : AInputPawn::inputListeners)
                {
                    IInputBase->OnMouseLMove(PCLocation, 1.0f);
                }
                break;
            }
        case EOperationType::FLY_MODE_START:
            {
                float X = JsonObject->GetNumberField("coordinateX");
                float Y = JsonObject->GetNumberField("coordinateY");
                FVector2D PCLocation = UdpServerUtilities::ToPCLocation(FVector2D(X, Y));
                //按下三个鼠标按键
                for (auto& IIputBase : AInputPawn::inputListeners)
                {
                    IIputBase->OnMouseLButtonDown(PCLocation);
                    IIputBase->OnMouseMidButtonDown(PCLocation);
                    IIputBase->OnMouseRButtonDown(PCLocation);
                }
                //LastRotatePosition = PCLocation;
                break;
            }
        default:
            break;
        }
    }
}
