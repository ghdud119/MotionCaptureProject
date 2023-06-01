// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyAnimInstance.generated.h"


class AMyCharacter;

UCLASS()
class JUSTDANCE_PROJECT_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:


	UFUNCTION()
	virtual void NativeInitializeAnimation() override;
	
	UFUNCTION()
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Ref")
	TObjectPtr<AMyCharacter> CurCharRef;

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
	FVector Loc_LeftFoot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Loc_RightFoot;

	
	
};
