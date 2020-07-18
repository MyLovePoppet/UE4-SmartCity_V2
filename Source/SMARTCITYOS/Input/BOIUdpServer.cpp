// Fill out your copyright notice in the Description page of Project Settings.


#include "BOIUdpServer.h"

bool FBoiUdpReceiver::Init()
{
    if (UdpServerUtilities::OperationMap.Num() == 0)
    {
        UdpServerUtilities::InitOperationMap();
    }
    bool isSuccess;
    StartUDPReceiver("DragUDPServer", "192.168.1.1", EOperationPort::FLY_MODE_DRAG, isSuccess);
    if (!isSuccess)
    {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, "Init falied!");
    }
    return isSuccess;
}

uint32 FBoiUdpReceiver::Run()
{
    while (true)
    {
        FString JsonStr;
        bool isSuccess;
        DataRecv(JsonStr, isSuccess);
        if (isSuccess)
        {
            GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Green, JsonStr);
            TSharedPtr<FJsonObject> JsonObject;
            if (UdpServerUtilities::GetJsonObjectFromJsonFString(JsonStr, JsonObject))
            {
                Handle(JsonObject);
            }
        }
    }
}

void FBoiUdpReceiver::Stop()
{
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
}

void FBoiUdpReceiver::StartUDPReceiver(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort,
                                       bool& success)
{
    TSharedRef<FInternetAddr> targetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    FIPv4Address Addr;
    FIPv4Address::Parse(TheIP, Addr);
    //Create Socket
    FIPv4Endpoint Endpoint(FIPv4Address::Any, ThePort); //所有ip地址本地
    //FIPv4Endpoint Endpoint(Addr, ThePort);                 //指定ip地址
    Socket = FUdpSocketBuilder(*YourChosenSocketName)
             .AsBlocking()
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
}

void FBoiUdpReceiver::DataRecv(FString& str, bool& success)
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

void FBoiUdpReceiver::Handle(const TSharedPtr<FJsonObject>& JsonObject)
{
}

// Sets default values
ABOIUdpServer::ABOIUdpServer()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABOIUdpServer::BeginPlay()
{
    Super::BeginPlay();
    DragUdpRunnable = MakeShareable(new FBoiUdpReceiver());
    DragUdpThread = MakeShareable(FRunnableThread::Create(DragUdpRunnable.Get(),TEXT("DragUdpThread")));
}

void ABOIUdpServer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    DragUdpThread->Kill(false);
}

// Called every frame
void ABOIUdpServer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
