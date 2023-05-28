// BoneTree.h

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BoneTree.generated.h"

UCLASS()
class JUSTDANCE_PROJECT_API UBoneTree : public UObject
{
	GENERATED_BODY()

public:
	UBoneTree();
	UBoneTree(UBoneTree* parent, FVector LandmarkVector);
	~UBoneTree();

	void SetLandmarkVector(FVector LandmarkVector);
	void SetAdjacentRotation(FRotator AdjacentRotation);
	void SetRelativeVector();
	FVector GetLandmarkVector() const { return BoneLandmarkVector; }
	FVector GetRelativeVector() const { return BoneRelativeVector; }

	UFUNCTION(BlueprintCallable)
	FRotator GetAdjacentRotation() const { return AdjacentRotation; }

	UBoneTree* GetParent() const { return Parent; }
	TArray<UBoneTree*> GetChildren() const { return Children; }

	static UBoneTree* CreateUBoneTreeObject(UBoneTree* parent, FVector landmarkVector);
	static UBoneTree* BuildBoneTree(TMap<int32, UBoneTree*>& BoneMap);

private:
	UBoneTree* Parent;
	TArray<UBoneTree*> Children;
	FVector BoneRelativeVector;
	FVector BoneLandmarkVector;
	FRotator AdjacentRotation;
};
