// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interactables/Interfaces/InventoryWidgetInterface.h"
#include "CloseButton.h"
#include "InventorySlotWidget.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class CRLTSIM_API UInventoryWidget : public UUserWidget, public IInventoryWidgetInterface
{
	GENERATED_BODY()

public:
	
	UUIContainerComponent* UIContainerComponent;

	void SetContainerComponent_Implementation(class UUIContainerComponent* ContainerComponent) override;

	class UUIContainerComponent* GetContainerComponent_Implementation() const override;

	void UpdateUI_Implementation(class UDataTable* InventoryDataTable, const TArray<FInventoryItemSlot>& inventorySlots) override;

	UPROPERTY(meta=(BindWidget))
	UCloseButton* CloseButton;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* InventoryPanel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxRows;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxColumns;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UInventorySlotWidget> SlotClass;
};
