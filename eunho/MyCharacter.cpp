// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "Networking/Public/Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "JsonUtilities/Public/JsonObjectConverter.h"
#include "BoneTree.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);
    LandmarkVectors.Init(FVector(0, 0, 0), 33);
}

AMyCharacter::~AMyCharacter()
{
    Disconnect();
    //delete BoneRoot;
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

void AMyCharacter::GmGetAllSocketNames()
{
    TArray<FName> a = GetMesh()->GetAllSocketNames();
      
    GmAllSockets.Empty(0);
      
    for (auto Element : a)
    {
        GmAllSockets.AddUnique(Element);
    }

    CurAISkeletalMesh = Cast<AMyCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), RefAIClass))->GetMesh();

}

bool AMyCharacter::GmCompareValues(float ErrorT)
{
    bool bResult = true;
    
    if(!GmAllSockets.IsEmpty())
    {
        FTransform CurRootTransform = GetMesh()->GetSocketTransform(GmAllSockets[0], RTS_World);
        FTransform CurRootTransform2 = CurAISkeletalMesh->GetSocketTransform(GmAllSockets[0], RTS_World);
        
        for (int i = 1;  i < GmAllSockets.Num() - 1; i++)
        {
            FVector OutVec;
            FRotator OutRot;
            GetMesh()->TransformToBoneSpace(GmAllSockets[i], CurRootTransform.GetLocation(), CurRootTransform.Rotator(), OutVec, OutRot);

        
            FVector OutVec2;
            FRotator OutRot2;
            CurAISkeletalMesh->TransformToBoneSpace(GmAllSockets[i], CurRootTransform2.GetLocation(), CurRootTransform2.Rotator(), OutVec2, OutRot2);

            if (!FMath::IsNearlyEqual(OutVec.Size()+OutRot.Vector().Size(), OutVec2.Size()+OutRot2.Vector().Size(), ErrorT))
            {
                bResult = false;
            }
        }
    }

    return bResult;
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
void AMyCharacter::UpdateBoneTree()
{
    FVector ShoulderCenter = (LandmarkVectors[12] + LandmarkVectors[11]) / 2;
    FVector HipCenter = (LandmarkVectors[24] + LandmarkVectors[23]) / 2;
    EstimatedPelvis = (ShoulderCenter - HipCenter) + HipCenter * 0.1;
    EsitmatedSpine_03 = (ShoulderCenter - HipCenter) + HipCenter * 0.8;

    UBoneTree* CurrentBone = nullptr;

    for (int i = 0; i < LandmarkVectors.Num(); i++)
    {
        CurrentBone = BoneMap.FindRef(i);

        if (CurrentBone)
        {
            if (i == 33)
            {
                CurrentBone->SetLandmarkVector(EstimatedPelvis);
            }
            else if (i == 34)
            {
                CurrentBone->SetLandmarkVector(EsitmatedSpine_03);
            }
            else
            {
                CurrentBone->SetLandmarkVector(LandmarkVectors[i]);
            }

            CurrentBone->SetRelativeVector();

            UBoneTree* ParentBone = CurrentBone->GetParent();
            TArray<UBoneTree*> ChildBone = CurrentBone->GetChildren();

            if (ParentBone && 0 < ChildBone.Num())
            {
                FRotator AdjacentRotation = GetRotatorfromVector(
                    ParentBone->GetLandmarkVector(),
                    CurrentBone->GetLandmarkVector(),
                    ChildBone[0]->GetLandmarkVector()
                );

                if (i == 11)
                {
                    GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green, FString::Printf(TEXT("Bone11 : %s"), *AdjacentRotation.ToString()));
                }

                CurrentBone->SetAdjacentRotation(AdjacentRotation);
            }
        }
        else
        {
            //UE_LOG(LogTemp, Error, TEXT("Landmark %d is broken : %p "), i, CurrentBone);
        }

        LandmarksUpdated = true;
    }
}
// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();
    BoneRoot = UBoneTree::BuildBoneTree(BoneMap);
}



// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
    UBoneTree* Pelvis = BoneMap.FindRef(33);
    DrawDebugBones(BoneRoot, {0,0,0});

    if (LandmarksUpdated)
    {
        // Print the landmark vectors to file
        DebugPrintDataToFile();

        // Reset the flag
        LandmarksUpdated = false;
    }

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
FRotator AMyCharacter::GetRotatorfromVector(FVector StartVector, FVector JointVector, FVector EndVector)
{
    FVector tmpStarttoJoint = StartVector - JointVector;
    FVector tmpJointtoEnd = EndVector - JointVector;

    FVector StarttoJoint = ChangeCoordinate(tmpStarttoJoint);
    FVector JointtoEnd = ChangeCoordinate(tmpJointtoEnd);

    //below this is old thing.. 
    ////this is for  X - axis Rotator angle calculator
    //FVector tmp1 = StarttoJoint;
    //tmp1.X = 0.0f;
    //FVector tmp2 = JointtoEnd;
    //tmp2.X = 0.0f;

    //float cosineX = FVector::DotProduct(tmp1, tmp2) / tmp1.Size() * tmp2.Size();
    //float RotatorX = FMath::RadiansToDegrees(FMath::Acos(cosineX));

    ////this is for  Y - axis Rotator angle calculator
    //FVector tmp3 = StarttoJoint;
    //tmp3.Y = 0.0f;
    //FVector tmp4 = JointtoEnd;
    //tmp4.Y = 0.0f;

    //float cosineY = FVector::DotProduct(tmp3, tmp4) / tmp3.Size() * tmp4.Size();
    //float RotatorY = FMath::RadiansToDegrees(FMath::Acos(cosineY));

    ////this is for  Z - axis Rotator angle calculator
    //FVector tmp5 = StarttoJoint;
    //tmp5.Z = 0.0f;
    //FVector tmp6 = JointtoEnd;
    //tmp6.Z = 0.0f;

    //float cosineZ = FVector::DotProduct(tmp5, tmp6) / tmp5.Size() * tmp6.Size();
    //float RotatorZ = FMath::RadiansToDegrees(FMath::Acos(cosineZ));
	
    /*maybe this is right thing.. you have to change coordinate transformation*/
    FRotator Rotator = JointtoEnd.Rotation();




    return Rotator;
}
FVector AMyCharacter::ChangeCoordinate(FVector changeVector)
{
	float nX = changeVector.Y * -1;
	float nY = changeVector.X;
	float nZ = changeVector.Z ;

	
	return FVector(nX,nY,nZ);
}

void AMyCharacter::DrawDebugBones(UBoneTree* Bone, const FVector& ParentPosition)
{
	UWorld* World = GetWorld();
	if (World != nullptr)
	{
		const float Radius = 10.0f;
		const FColor Color = FColor::Red;
		const float LifeTime = 0.3f;

		FVector BonePosition = ParentPosition + Bone->GetRelativeVector() * 100;

		DrawDebugPoint(World, BonePosition, Radius, Color, false, LifeTime);

		// 로테이션 값을 방향 벡터로 변환
		FVector Direction = Bone->GetRelativeVector();

		// 디버그 라인 그리기
		FVector LineEnd = BonePosition + Direction * 100.0f; 
		DrawDebugLine(World, BonePosition, LineEnd, FColor::Blue, false, LifeTime);

		// 자식 본들에 대해 재귀적으로 호출
		for (UBoneTree* Child : Bone->GetChildren())
		{
			DrawDebugBones(Child, BonePosition);
		}

	}
}

void AMyCharacter::DebugPrintDataToFile()
{
	FString Content = "";

	// Get current time
	FDateTime Now = FDateTime::UtcNow();

	// Convert the current time to string
	FString Timestamp = Now.ToString();

	// 랜드마크 벡터를 저장하는 루프
	for (int i = 0; i < LandmarkVectors.Num(); i++)
	{
		FVector LandmarkVector = LandmarkVectors[i];

		Content += FString::Printf(TEXT("[%s] Landmark %d: X=%f, Y=%f, Z=%f\n"),
			*Timestamp,
			i,
			LandmarkVector.X,
			LandmarkVector.Y,
			LandmarkVector.Z);
	}

	Content += FString::Printf(TEXT("\nCharacter LandMark Done \n Bonetree Data Start\n\n"));

	UBoneTree* CurrentBoneTree = nullptr;
	for (int i = 0; i < 35; i++)
	{
		if ((9 < i && i < 17) || (22 < i && i < 29) || (32 < i && i < 35))
		{
			CurrentBoneTree = BoneMap.FindRef(i);

			Content += FString::Printf(TEXT("[%s] BoneMap Number: %d\n"), *Timestamp, i);
			Content += FString::Printf(TEXT("[%s] Landmark Vector: %s\n"), *Timestamp, *CurrentBoneTree->GetLandmarkVector().ToString());
			Content += FString::Printf(TEXT("[%s] Relative Vector: %s\n"), *Timestamp, *CurrentBoneTree->GetRelativeVector().ToString());
			Content += FString::Printf(TEXT("[%s] Rotation: %s\n"), *Timestamp, *CurrentBoneTree->GetAdjacentRotation().ToString());
			Content += FString::Printf(TEXT("\n"));
		}
	}

	// 텍스트를 파일로 저장
	FFileHelper::SaveStringToFile(Content, TEXT("D:\\Desktop\\LandmarkVectorsData.txt"));
}

double AMyCharacter::TestFunc(FVector AnyVA, FVector AnyVB)
{    double RstA = AnyVA.Size();
    double RstB = AnyVB.Size();

    double Result;
    return Result = RstA>RstB ? RstB-RstA : RstA-RstB;
    
}
