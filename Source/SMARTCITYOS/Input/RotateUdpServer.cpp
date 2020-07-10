// Fill out your copyright notice in the Description page of Project Settings.


#include "RotateUdpServer.h"

#include "InputPawn.h"

void ARotateUdpServer::BeginPlay()
{
    Super::BeginPlay();
    bool isSuccess;
    StartUDPReceiver("DragUDPServer", "192.168.1.1", EOperationPort::FLY_MODE_ROTATE, isSuccess);
    if (!isSuccess)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "Init falied!");
    }
}

void ARotateUdpServer::Handle(const TSharedPtr<FJsonObject>& JsonObject)
{
    FString Type;
    if (JsonObject->TryGetStringField("type", Type))
    {
        switch (ToEnumType(Type))
        {
        case EOperationType::FLY_MODE_START:
            {
                float X = JsonObject->GetNumberField("coordinateX");
                float Y = JsonObject->GetNumberField("coordinateY");
                FVector2D PCLocation = ToPCLocation(FVector2D(X, Y));
                //按下三个鼠标按键
                for (auto& IIputBase : AInputPawn::inputListeners)
                {
                    IIputBase->OnMouseLButtonDown(PCLocation);
                    IIputBase->OnMouseMidButtonDown(PCLocation);
                    IIputBase->OnMouseRButtonDown(PCLocation);
                }
                LastRotatePosition = PCLocation;
                break;
            }

        case EOperationType::FLY_MODE_ROTATE:
            {
                float RotateAngle = JsonObject->GetNumberField("degree");
                for (auto& IInputBase : AInputPawn::inputListeners)
                {
                    //转化为移动的数据
                    RotateAngle=FMath::Clamp(RotateAngle,-5.f,5.f);
                    float XOffset=RotateAngle/180.0f*PCScreenSize.X;
                    FVector2D CurrentLocation=LastRotatePosition+FVector2D(XOffset,0.f);
                    IInputBase->OnMouseMMove(CurrentLocation,1.0f);
                    LastRotatePosition=CurrentLocation;
                }
                break;
            }
            
        default:
            break;
        }
    }
}
