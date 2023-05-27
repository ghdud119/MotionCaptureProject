// MyCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

class UBoneTree;

UCLASS()
class JUSTDANCE_PROJECT_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMyCharacter();
	~AMyCharacter();

public:
	UFUNCTION(BlueprintCallable, Category = "Networking")
		bool Connect(FString Host, int32 Port);

	UFUNCTION(BlueprintCallable, Category = "Networking")
		void Disconnect();

	UFUNCTION(BlueprintCallable, Category = "Networking")
		bool ReceiveData(TArray<float>& OutData);

	UFUNCTION(BlueprintCallable, Category = "DataProcessing")
		void UpdateBoneTree();

	UFUNCTION(BlueprintCallable, Category = "DataProcessing")
	FRotator GetRotatorfromVector(FVector StartVector, FVector JointVector, FVector EndVector);

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintReadOnly)
		FString pakitChecker;

	UPROPERTY(BlueprintReadOnly)
		TArray<FVector> LandmarkVectors;

	UPROPERTY(BlueprintReadOnly)
		FVector EstimatedPelvis;

	UPROPERTY(BlueprintReadOnly)
		FVector EsitmatedSpine_03;

	UPROPERTY(BlueprintReadOnly)
		TMap<int32, UBoneTree*> BoneMap;
	

private:
	FSocket* Socket = nullptr;
	UBoneTree* BoneRoot = nullptr;
};
