// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Networking/Public/Networking.h"
#include "UdpServerInfo.h"

/**
 * 
 */
class SMARTCITYOS_API UdpServerUtilities
{
public:
    static TMap<FString, EOperationType> OperationMap;
    static void InitOperationMap();
    static bool GetJsonObjectFromJsonFString(const FString& _jsonFString, TSharedPtr<FJsonObject>& _jsonObject);
    static EOperationType ToEnumType(const FString& Str);
    static FVector2D ToPCLocation(FVector2D PhoneLocation);
    static bool SendDataWithUdp(const FString& Message, const FString& TheIP = TEXT("192.168.50.52"),
                                const uint32 ThePort = EOperationPort::SEND_BACK_PORT,
                                const FString& YourChosenSocketName = TEXT("UdpSender"));
    static bool GetFStringInJsonFormat(const TSharedPtr<FJsonObject>& _ptrJsonObj, FString& _strGet);
};
