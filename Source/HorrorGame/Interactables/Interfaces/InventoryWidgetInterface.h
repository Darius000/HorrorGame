// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Engine/DataTable.h"
#include "Inventory/UIContainerComponent.h"
#include "InventoryWidgetInterface.generated.h"

class UInventoryComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInventoryWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CRLTSIM_API IInventoryWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Inventory")
		void SetContainerComponent(class UUIContainerComponent* ContainerComponent);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Inventory")
		class UUIContainerComponent* GetContainerComponent() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Inventory")
		void UpdateUI(class UDataTable* InventoryDataTable, const TArray<FInventoryItemSlot>& inventorySlots);
};
