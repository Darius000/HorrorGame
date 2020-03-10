// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InteractionAnimationData.generated.h"


/**
 * 
 */
UCLASS()
class CRLTSIM_API UInteractionAnimationData : public UDataAsset
{
	GENERATED_BODY()

	UInteractionAnimationData()
		:Montage(nullptr),MontageName(NAME_None), MontagePlayRate(1.0f),
		bShouldMoveToLocation(false)
	{

	}

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monatge")
		class UAnimMontage* Montage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monatge")
		FName MontageName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monatge")
		float MontagePlayRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
		bool bShouldMoveToLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
		FVector AnimationLocation;
};
