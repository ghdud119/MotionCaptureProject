// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "Networking/Public/Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "JsonUtilities/Public/JsonObjectConverter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);
    LandmarkVectors.Init(FVector(0, 0, 0), 33);
}

bool AMyCharacter::Connect(FString Host, int32 Port)
{
    // 소켓 생성
    //Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);
    if (Socket == nullptr)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Failed to create socket")));
        return false;
    }

    // 주소 설정
    FIPv4Address IPv4Address;
    FIPv4Address::Parse(Host, IPv4Address);

    TSharedRef<FInternetAddr> Address = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    Address->SetIp(IPv4Address.Value);
    Address->SetPort(Port);

    // 연결 시도
    bool bConnected = Socket->Connect(*Address);
    if (bConnected)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Connected to %s:%d"), *Host, Port));
        return true;
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Failed to connect to %s:%d"), *Host, Port));
        Disconnect();
        return false;
    }

    return false;
}


void AMyCharacter::Disconnect()
{
    if (Socket)
    {
        Socket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
        Socket = nullptr;
    }
}

bool AMyCharacter::ReceiveData(TArray<float>& OutData)
{
    if (Socket)
    {
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Success to recive socket")));
        uint32 PendingDataSize = 0;
        if (Socket->HasPendingData(PendingDataSize))
        {
            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Success to recive data")));
            TArray<uint8> ReceivedData;
            ReceivedData.SetNumUninitialized(PendingDataSize);
            int32 BytesRead = 0;
            Socket->Recv(ReceivedData.GetData(), ReceivedData.Num(), BytesRead);

            FString DataAsString;
            FFileHelper::BufferToString(DataAsString, ReceivedData.GetData(), BytesRead);
            TArray<TSharedPtr<FJsonValue>> JsonArray;
            TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(DataAsString);

            //UE_LOG(LogTemp, Log, TEXT("Received JSON: %s"), *DataAsString);

            if (FJsonSerializer::Deserialize(Reader, JsonArray))
            {
                // JSON 배열의 각 요소에 대해 반복합니다.
                for (const TSharedPtr<FJsonValue>& JsonValue : JsonArray)
                {
                    TSharedPtr<FJsonObject> LandmarkObject = JsonValue->AsObject();

                    int32 Num = LandmarkObject->GetIntegerField("Num");
                    float X = LandmarkObject->GetNumberField("z") *-10;
                    float Y = LandmarkObject->GetNumberField("x") *-120 + 60;
                    float Z = LandmarkObject->GetNumberField("y") * -120 + 240;
                    float Visibility = LandmarkObject->GetNumberField("visibility");

                    // FVector 배열에 저장합니다.
                    LandmarkVectors[Num] = FVector(X, Y, Z); 
                    //정규화 필요, 언리얼 내부 캐릭터 뼈대 비교, 특정 시간대 이벤트 (타임함수)
                    // 로그에 벡터를 출력합니다.
                    UE_LOG(LogTemp, Log, TEXT("Landmark %d: %s"), Num, *LandmarkVectors[Num].ToString());
                }
            }
        }
        else
        {
            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Failed to recive socket")));
            return false;
        }
    }
    else
    {
        return false;
    }
    return true;
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();

}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMyCharacter::PlayAIMontage(UAnimMontage* AnyMtg)
{
    PlayAnimMontage(AnyMtg);
}

double AMyCharacter::TestFunc(FVector AnyVA, FVector AnyVB)
{    double RstA = AnyVA.Size();
    double RstB = AnyVB.Size();

    double Result;
    return Result = RstA>RstB ? RstB-RstA : RstA-RstB;
    
}
