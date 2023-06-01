// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"

#include "JustDance_Project/MyCharacter.h"

void UMyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if(CurCharRef == nullptr)
	{
		CurCharRef = Cast<AMyCharacter>(TryGetPawnOwner());
	}
}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(CurCharRef != nullptr)
	{
		CurCharRef->Loc_Head = Loc_Head;
		CurCharRef->Loc_LeftShoulder = Loc_LeftShoulder;
		CurCharRef->Loc_RightShoulder = Loc_RightShoulder;
		CurCharRef->Loc_LeftElbow = Loc_LeftElbow;
		CurCharRef->Loc_RightElbow = Loc_RightElbow;
		CurCharRef->Loc_LeftHand = Loc_LeftHand;
		CurCharRef->Loc_RightHand = Loc_RightHand;
		CurCharRef->Loc_LeftHip = Loc_LeftHip;
		CurCharRef->Loc_RightHip = Loc_RightHip;
		CurCharRef->Loc_LeftKnee = Loc_LeftKnee;
		//CurCharRef->Loc_RightKnee= Loc_RightKnee;
		CurCharRef->Loc_LeftFoot = Loc_LeftFoot;
		CurCharRef->Loc_RightFoot = Loc_RightFoot;
	}
}
