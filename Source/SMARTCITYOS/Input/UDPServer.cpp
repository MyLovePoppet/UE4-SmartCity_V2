// Fill out your copyright notice in the Description page of Project Settings.


#include "UDPServer.h"

#include "InputPawn.h"

FVector2D AUDPServer::PhoneScreenSize = FVector2D(2244.0f, 1080.f);
FVector2D AUDPServer::PCScreenSize = FVector2D(1920.0f, 980.f);
TMap<FString,EOperationType> AUDPServer::OperationMap=TMap<FString,EOperationType>();
// Sets default values
AUDPServer::AUDPServer()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    if (OperationMap.Num() == 0)
    {
        InitOperationMap();
    }
}

// Called when the game starts or when spawned
void AUDPServer::BeginPlay()
{
    Super::BeginPlay();
    bool isSuccess;
    StartUDPReceiver("TestUDP", "192.168.1.1", 9099, isSuccess);
    if (!isSuccess)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "Init falied!");
    }
}

void AUDPServer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    //~~~~~~~~~~~~~~~~

    if (UdpSocketReceiver)
        delete UdpSocketReceiver;
    UdpSocketReceiver = nullptr;

    //Clear all sockets!
    //      makes sure repeat plays in Editor dont hold on to old sockets!
    if (Socket)
    {
        Socket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
    }
    //释放三个鼠标按键
    for (auto& IIputBase : AInputPawn::inputListeners)
    {
        IIputBase->OnMouseLButtonUp(FVector2D::ZeroVector);
        IIputBase->OnMouseMidButtonUp(FVector2D::ZeroVector);
        IIputBase->OnMouseRButtonUp(FVector2D::ZeroVector);
    }
}

// Called every frame
void AUDPServer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    FString JsonStr;
    bool isSuccess;
    DataRecv(JsonStr, isSuccess);
    if (isSuccess)
    {
        

        GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Green, JsonStr);  
        TSharedPtr<FJsonObject> JsonObject;
        if (GetJsonObjectFromJsonFString(JsonStr, JsonObject))
        {
            Handle(JsonObject);
        }
    }
}

void AUDPServer::StartUDPReceiver(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort,
                                  bool& success)
{
    TSharedRef<FInternetAddr> targetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    FIPv4Address Addr;
    FIPv4Address::Parse(TheIP, Addr);
    //Create Socket
    FIPv4Endpoint Endpoint(FIPv4Address::Any, ThePort); //所有ip地址本地
    //FIPv4Endpoint Endpoint(Addr, ThePort);                 //指定ip地址
    Socket = FUdpSocketBuilder(*YourChosenSocketName)
             .AsNonBlocking()
             .AsReusable()    
             .BoundToEndpoint(Endpoint)
             .WithReceiveBufferSize(2 * 1024 * 1024);
    //BUFFER SIZE
    int32 BufferSize = 1024;
    Socket->SetSendBufferSize(BufferSize, BufferSize);
    Socket->SetReceiveBufferSize(BufferSize, BufferSize);

    if (!Socket)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red,TEXT("No socket"));
        success = false;
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red,TEXT("The receiver is initialized"));
        success = true;
    }

    //return true;
}

void AUDPServer::DataRecv(FString& str, bool& success)
{
    if (!Socket)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red,TEXT("No socket"));
        success = false;
        //return success;
    }
    TSharedRef<FInternetAddr> targetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    TArray<uint8> ReceivedData; //定义一个接收器
    uint32 Size;
    if (Socket->HasPendingData(Size))
    {
        success = true;
        str = "";
        uint8* Recv = new uint8[Size];
        int32 BytesRead = 0;

        ReceivedData.SetNumUninitialized(FMath::Min(Size, 65507u));
        Socket->RecvFrom(ReceivedData.GetData(), ReceivedData.Num(), BytesRead, *targetAddr); //创建远程接收地址
        char ansiiData[1024];
        FMemory::Memcpy(ansiiData, ReceivedData.GetData(), BytesRead); //拷贝数据到接收器
        ansiiData[BytesRead] = 0; //判断数据结束
        FString debugData = ANSI_TO_TCHAR(ansiiData); //字符串转换
        str = debugData;
        // memset(ansiiData,0,1024);//清空 
    }
    else
    {
        success = false;
    }
    //return success;
}

//处理接收到的字符串
void AUDPServer::Handle(const TSharedPtr<FJsonObject>& JsonObject)
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
                LastRotatePosition=PCLocation;
                break;
            }
        case EOperationType::FLY_MODE_DRAG:
            {
                float X = JsonObject->GetNumberField("coordinateX");
                float Y = JsonObject->GetNumberField("coordinateY");
                for (auto& IInputBase : AInputPawn::inputListeners)
                {
                    IInputBase->OnMouseLMove(ToPCLocation(FVector2D(X, Y)), 1.0f);
                }
                break;
            }
        
        case EOperationType::FLY_MODE_SCALE:
            {
                float X = JsonObject->GetNumberField("coordinateX");
                float Y = JsonObject->GetNumberField("coordinateY");
                float ScaleRate = JsonObject->GetNumberField("degree");
                float Value = FMath::Sign(ScaleRate - 1.0f);
                for (auto& IInputBase : AInputPawn::inputListeners)
                {
                    IInputBase->OnMouseWheel(ToPCLocation(FVector2D(X, Y)), Value);
                }
                break;
            }
            
        case EOperationType::FLY_MODE_ROTATE:
            {
                // float X = JsonObject->GetNumberField("coordinateX");
                // float Y = JsonObject->GetNumberField("coordinateY");
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

        case EOperationType::GROUND_MODE_MOVE:
            {
                float ForwardAxisValue = JsonObject->GetNumberField("VeticalAxis");
                ForwardAxisValue = FMath::Clamp(ForwardAxisValue, -1.f, 1.f);
                float RightAxisValue = JsonObject->GetNumberField("HorizontalAxis");
                RightAxisValue = FMath::Clamp(RightAxisValue, -1.f, 1.f);
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
                YawValue/=PhoneScreenSize.X;
                YawValue = FMath::Clamp(YawValue, -1.f, 1.f);

                float PitchValue = JsonObject->GetNumberField("pitchValue");
                PitchValue/=PhoneScreenSize.Y;
                PitchValue = FMath::Clamp(PitchValue, -1.f, 1.f);
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

//读取Json字符串
bool AUDPServer::GetJsonObjectFromJsonFString(const FString& _jsonFString, TSharedPtr<FJsonObject>& _jsonObject)
{
    if (!_jsonFString.IsEmpty())
    {
        TSharedRef<TJsonReader<>> t_reader = TJsonReaderFactory<>::Create(_jsonFString);
        return FJsonSerializer::Deserialize(t_reader, _jsonObject);
    }
    return false;
}

FVector2D AUDPServer::ToPCLocation(FVector2D PhoneLocation)
{
    float X = FMath::Clamp(PhoneLocation.X / PhoneScreenSize.X * PCScreenSize.X, 0.f, PCScreenSize.X);
    float Y = FMath::Clamp(PhoneLocation.Y / PhoneScreenSize.Y * PCScreenSize.Y, 0.f, PCScreenSize.Y);
    return FVector2D(X, Y);
}

void AUDPServer::InitOperationMap()
{
    OperationMap.Add(TEXT("FLY_MODE_START"), EOperationType::FLY_MODE_START);
    OperationMap.Add(TEXT("FLY_MODE_DRAG"), EOperationType::FLY_MODE_DRAG);
    OperationMap.Add(TEXT("FLY_MODE_SCALE"), EOperationType::FLY_MODE_SCALE);
    OperationMap.Add(TEXT("FLY_MODE_ROTATE"), EOperationType::FLY_MODE_ROTATE);

    OperationMap.Add(TEXT("GROUND_MODE_MOVE"), EOperationType::GROUND_MODE_MOVE);

    OperationMap.Add(TEXT("GROUND_MODE_JUMP"), EOperationType::GROUND_MODE_JUMP);
    OperationMap.Add(TEXT("GROUND_MODE_START_SPRINT"), EOperationType::GROUND_MODE_START_SPRINT);
    OperationMap.Add(TEXT("GROUND_MODE_STOP_SPRINT"), EOperationType::GROUND_MODE_STOP_SPRINT);

    OperationMap.Add(TEXT("GROUND_MODE_CAMERA_MOVE"), EOperationType::GROUND_MODE_CAMERA_MOVE);
}
