// BoneTree.cpp

#include "BoneTree.h"

UBoneTree::UBoneTree()
	: Parent(nullptr), BoneRelativeVector(FVector::ZeroVector), BoneLandmarkVector(FVector::ZeroVector)
{
}

UBoneTree::UBoneTree(UBoneTree* parent, FVector LandmarkVector)
	: Parent(parent), BoneLandmarkVector(LandmarkVector)
{
	if (Parent)
	{
		BoneRelativeVector = BoneLandmarkVector - Parent->BoneLandmarkVector;
	}
}

UBoneTree::~UBoneTree()
{
	/*for (int i = 0; i < Children.Num(); i++)
	{
		delete Children[i];
	}*/
}

void UBoneTree::SetLandmarkVector(FVector LandmarkVector)
{
	this->BoneLandmarkVector = LandmarkVector;
}

void UBoneTree::SetAdjacentRotation(FRotator adjacentRotation)
{
	this->AdjacentRotation = adjacentRotation;
}

void UBoneTree::SetRelativeVector()
{
	this->BoneRelativeVector = this->BoneLandmarkVector - this->Parent->BoneLandmarkVector;
}

UBoneTree* UBoneTree::CreateUBoneTreeObject(UBoneTree* parent, FVector landmarkVector)
{
	UBoneTree* RawBoneTree = NewObject<UBoneTree>();
	RawBoneTree->Parent = parent;
	RawBoneTree->BoneLandmarkVector = landmarkVector;
	if (parent)
	{
		RawBoneTree->BoneRelativeVector = landmarkVector - parent->BoneLandmarkVector;
	}
	return RawBoneTree;
}


UBoneTree* UBoneTree::BuildBoneTree(TMap<int32, UBoneTree*>& BoneMap)
{
	UBoneTree* Pelvis = CreateUBoneTreeObject(nullptr, FVector(0, 0, 0));
	// Çã¹÷Áö - ¹ß
	UBoneTree* thigh_r = CreateUBoneTreeObject(Pelvis, FVector(0, 0, 0));
	UBoneTree* thigh_l = CreateUBoneTreeObject(Pelvis, FVector(0, 0, 0));
	UBoneTree* calf_r = CreateUBoneTreeObject(thigh_r, FVector(0, 0, 0));
	UBoneTree* calf_l = CreateUBoneTreeObject(thigh_l, FVector(0, 0, 0));
	UBoneTree* foot_r = CreateUBoneTreeObject(calf_r, FVector(0, 0, 0));
	UBoneTree* foot_l = CreateUBoneTreeObject(calf_l, FVector(0, 0, 0));
	// °ñ¹Ý - ¸Ó¸®
	UBoneTree* spine_03 = CreateUBoneTreeObject(Pelvis, FVector(0, 0, 0));
	UBoneTree* head = CreateUBoneTreeObject(spine_03, FVector(0, 0, 0));
	// °¡½¿ - ÆÈ
	UBoneTree* upperarm_r = CreateUBoneTreeObject(spine_03, FVector(0, 0, 0));
	UBoneTree* upperarm_l = CreateUBoneTreeObject(spine_03, FVector(0, 0, 0));
	UBoneTree* lowerarm_r = CreateUBoneTreeObject(upperarm_r, FVector(0, 0, 0));
	UBoneTree* lowerarm_l = CreateUBoneTreeObject(upperarm_l, FVector(0, 0, 0));
	UBoneTree* hand_r = CreateUBoneTreeObject(lowerarm_r, FVector(0, 0, 0));
	UBoneTree* hand_l = CreateUBoneTreeObject(lowerarm_l, FVector(0, 0, 0));

	// ³ëµå ¿¬°á
	Pelvis->Children.Add(thigh_r);
	Pelvis->Children.Add(thigh_l);
	thigh_r->Children.Add(calf_r);
	thigh_l->Children.Add(calf_l);
	calf_r->Children.Add(foot_r);
	calf_l->Children.Add(foot_l);

	Pelvis->Children.Add(spine_03);
	spine_03->Children.Add(head);

	spine_03->Children.Add(upperarm_r);
	spine_03->Children.Add(upperarm_l);
	upperarm_r->Children.Add(lowerarm_r);
	upperarm_l->Children.Add(lowerarm_l);
	lowerarm_r->Children.Add(hand_r);
	lowerarm_l->Children.Add(hand_l);

	// ¾÷µ¥ÀÌÆ®¸¦ À§ÇÑ ¸Ê »ý¼º
	BoneMap.Add(33, Pelvis);
	BoneMap.Add(34, spine_03);
	BoneMap.Add(0, head);
	BoneMap.Add(11, upperarm_r);
	BoneMap.Add(12, upperarm_l);
	BoneMap.Add(13, lowerarm_r);
	BoneMap.Add(14, lowerarm_l);
	BoneMap.Add(15, hand_r);
	BoneMap.Add(16, hand_l);
	BoneMap.Add(23, thigh_r);
	BoneMap.Add(24, thigh_l);
	BoneMap.Add(25, calf_r);
	BoneMap.Add(26, calf_l);
	BoneMap.Add(27, foot_r);
	BoneMap.Add(28, foot_l);

	return Pelvis;
}
