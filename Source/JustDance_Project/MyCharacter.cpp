// MyCharacter.cpp

#include "MyCharacter.h"
#include "Networking/Public/Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "JsonUtilities/Public/JsonObjectConverter.h"
#include "BoneTree.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AMyCharacter::AMyCharacter()
{
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
	if (Socket == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Failed to create socket")));
		return false;
	}

	FIPv4Address IPv4Address;
	FIPv4Address::Parse(Host, IPv4Address);

	TSharedRef<FInternetAddr> Address = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	Address->SetIp(IPv4Address.Value);
	Address->SetPort(Port);

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
bool AMyCharacter::ReceiveData(TArray<float>& OutData)
{
	if (Socket)
	{
		uint32 PendingDataSize = 0;
		if (Socket->HasPendingData(PendingDataSize))
		{
			TArray<uint8> ReceivedData;
			ReceivedData.SetNumUninitialized(PendingDataSize);
			int32 BytesRead = 0;
			Socket->Recv(ReceivedData.GetData(), ReceivedData.Num(), BytesRead);

			FString DataAsString;
			FFileHelper::BufferToString(DataAsString, ReceivedData.GetData(), BytesRead);
			TArray<TSharedPtr<FJsonValue>> JsonArray;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(DataAsString);

			if (FJsonSerializer::Deserialize(Reader, JsonArray))
			{
				for (const TSharedPtr<FJsonValue>& JsonValue : JsonArray)
				{
					TSharedPtr<FJsonObject> LandmarkObject = JsonValue->AsObject();

					int32 Num = LandmarkObject->GetIntegerField("Num");
					float X = LandmarkObject->GetNumberField("z") * -0.05;
					float Y = LandmarkObject->GetNumberField("x");
					float Z = (LandmarkObject->GetNumberField("y") * -1) + 2.0f;
					float Visibility = LandmarkObject->GetNumberField("visibility");

					LandmarkVectors[Num] = FVector(X, Y, Z);

					UpdateBoneTree();

					UE_LOG(LogTemp, Log, TEXT("Landmark %d: %s"), Num, *LandmarkVectors[Num].ToString());
				}
			}
		}
		else
		{
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

	for (int i = 0; i < (LandmarkVectors.Num() + 2); i++)
	{



		CurrentBone = BoneMap.FindRef(i);

		if (CurrentBone)
		{
			if (i == 33)
			{
				CurrentBone->SetLandmarkVector(EstimatedPelvis);

				FString string = EstimatedPelvis.ToString();
				GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green, FString::Printf(TEXT("Bone33 ")));
				
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



void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	BoneRoot = UBoneTree::BuildBoneTree(BoneMap);
}

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

FRotator AMyCharacter::GetRotatorfromVector(FVector StartVector, FVector JointVector, FVector EndVector)
{
	FVector StarttoJoint = JointVector - StartVector;
	FVector JointtoEnd = EndVector - JointVector;

	StarttoJoint.Normalize();
	JointtoEnd.Normalize();


	FRotator Rotator = UKismetMathLibrary::FindLookAtRotation(StarttoJoint, JointtoEnd);




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

