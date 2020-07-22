// Fill out your copyright notice in the Description page of Project Settings.


// Fill out your copyright notice in the Description page of Project Settings.


#include "UdpServerBase.h"

#include "InputPawn.h"


// Sets default values
AUdpServerBase::AUdpServerBase()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    if (UdpServerUtilities::OperationMap.Num() == 0)
    {
        UdpServerUtilities::InitOperationMap();
    }
}

// Called when the game starts or when spawned
void AUdpServerBase::BeginPlay()
{
    Super::BeginPlay();
}

void AUdpServerBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    //~~~~~~~~~~~~~~~~

    if (UdpSocketReceiver)
    {
        UdpSocketReceiver->Stop();
        delete UdpSocketReceiver;
        UdpSocketReceiver = nullptr;
    }


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
void AUdpServerBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    //uint8 CurrentFrame = 0;
    while (!OperationMessageQueue.IsEmpty() /*&& CurrentFrame < MAXFRAME*/)
    {
        TSharedPtr<FJsonObject> JsonObject;
        OperationMessageQueue.Dequeue(JsonObject);
        Handle(JsonObject);
        //CurrentFrame++;
    }
}

void AUdpServerBase::StartUDPReceiver(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort,
                                      bool& success)
{
    //TSharedRef<FInternetAddr> targetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    FIPv4Address Addr;
    FIPv4Address::Parse(TheIP, Addr);
    //Create Socket
    FIPv4Endpoint Endpoint(FIPv4Address::Any, ThePort); //所有ip地址本地
    //FIPv4Endpoint Endpoint(Addr, ThePort); //指定ip地址

    Socket = FUdpSocketBuilder(*YourChosenSocketName)
             .AsNonBlocking()
             .AsReusable()
             .BoundToEndpoint(Endpoint)
             .WithReceiveBufferSize(/*2 * 1024 * */1024);
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


    FString name = "SMARTCITYOS UDP Receiver Thread";
    UdpSocketReceiver = new FUdpSocketReceiver(Socket, FTimespan::FromMilliseconds(100), *name);
    UdpSocketReceiver->OnDataReceived().BindLambda(
        [this](const FArrayReaderPtr& arrayReader, const FIPv4Endpoint& endPoint)
        {
            char ansiiData[1024];
            FMemory::Memcpy(ansiiData, arrayReader->GetData(), arrayReader->TotalSize()); //拷贝数据到接收器
            ansiiData[arrayReader->TotalSize()] = 0; //判断数据结束
            FString JsonStr = ANSI_TO_TCHAR(ansiiData); //字符串转换
            GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Green, JsonStr);
            TSharedPtr<FJsonObject> JsonObject;
            if (UdpServerUtilities::GetJsonObjectFromJsonFString(JsonStr, JsonObject))
            {
                this->OperationMessageQueue.Enqueue(JsonObject);
            }
        }
    );

    UdpSocketReceiver->Start();
    //return true;
}

/*
void AUdpServerBase::DataRecv(FString& str, bool& success)
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
*/
void AUdpServerBase::Handle(const TSharedPtr<FJsonObject>& JsonObject)
{
}
