// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/BaseInteractable.h"
#include "Inventory/Inventory.h"
#include "Container.generated.h"

/**
 * 
 */
UCLASS()
class CRLTSIM_API AContainer : public ABaseInteractable
{
	GENERATED_BODY()
	
public:
	AContainer();

	void OnInteractWith_Implementation(class AActor* actor) override;


	FString GetInteractionText_Implementation() const override
	{
		return "Interacted and Opened Item Box";
	}

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
		UContainerComponent* ContainerComponent;
};
