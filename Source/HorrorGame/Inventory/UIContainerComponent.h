// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Inventory.h"
#include "UIContainerComponent.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class CRLTSIM_API UUIContainerComponent : public UContainerComponent
{
	GENERATED_BODY()

protected:
	void BeginPlay() override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MustImplement = "InventoryWidgetInterface"))
		TSubclassOf<UUserWidget> InventoryWidgetClass;
private:
	UPROPERTY()
		UUserWidget* InventoryWidget;

	UPROPERTY()
		bool bDisplayedInventory;

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void ShowInventory(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void HideInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void ToggleInventory(APlayerController* PlayerController);

	UFUNCTION()
		void UpdateInventoryUI(UDataTable* InventoryDataTable, const TArray<FInventoryItemSlot>& InventorySlots);
};
