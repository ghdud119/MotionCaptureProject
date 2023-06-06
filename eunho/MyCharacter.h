// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

class UInputComponent;

UCLASS()
class JUSTDANCE_PROJECT_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

	UFUNCTION(BlueprintCallable, Category = "Networking")
		bool Connect(FString Host, int32 Port);

	UFUNCTION(BlueprintCallable, Category = "Networking")
		void Disconnect();

	UFUNCTION(BlueprintCallable, Category = "Networking")
		bool ReceiveData(TArray<float>& OutData);

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Loc_Head;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Loc_LeftShoulder;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Loc_RightShoulder;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Loc_LeftElbow;
    	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Loc_RightElbow;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Loc_LeftHand;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Loc_RightHand;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Loc_LeftHip;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Loc_RightHip;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Loc_LeftKnee;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Loc_RightKnee;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Loc_LeftFoot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Loc_RightFoot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<ACharacter> AIChar;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;


	UPROPERTY(BlueprintReadOnly)
	FString pakitChecker;

	UPROPERTY(BlueprintReadOnly)
	TArray<FVector> LandmarkVectors;

	//TArray<FVector> GetLandmarkVecotrs() { return LandmarkVectors; }

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<UAnimMontage*> AnyMtgs;

	UFUNCTION(BlueprintCallable)
	void PlayAIMontage(UAnimMontage* AnyMtg);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector AnyVectorA{13.3f, 15.9f, 23.7f};
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector AnyVectorB{33.3f, 47.9f, 4.7f};

	UFUNCTION(BlueprintCallable)
	double TestFunc(FVector AnyVA, FVector AnyVB);

private:
	FSocket* Socket = nullptr;

	
};


