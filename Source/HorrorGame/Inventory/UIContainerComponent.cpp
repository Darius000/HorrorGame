// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.


#include "UIContainerComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Interactables/Interfaces/InventoryWidgetInterface.h"

void UUIContainerComponent::BeginPlay()
{
	Super::BeginPlay();

	OnInventoryUpdated.AddDynamic(this, &UUIContainerComponent::UpdateInventoryUI);
}

void UUIContainerComponent::ShowInventory(APlayerController* PlayerController)
{
	if(!InventoryWidgetClass && !InventoryWidget) return;

	if (!InventoryWidget && PlayerController)
	{
		InventoryWidget = UWidgetBlueprintLibrary::Create(this, InventoryWidgetClass, PlayerController);
		IInventoryWidgetInterface::Execute_SetContainerComponent(InventoryWidget, this);

		if (InventorySearchTable)
			OnInventoryUpdated.Broadcast(InventorySearchTable, Slots);
	}

	if (!InventoryWidget || !PlayerController) return;

	InventoryWidget->SetVisibility(ESlateVisibility::Visible);

	InventoryWidget->AddToViewport();
}

void UUIContainerComponent::HideInventory()
{
	if(!InventoryWidget) return;

	InventoryWidget->SetVisibility(ESlateVisibility::Hidden);

	InventoryWidget->RemoveFromViewport();
}

void UUIContainerComponent::ToggleInventory(APlayerController* PlayerController)
{
	if (!PlayerController) return;

	if(!bDisplayedInventory)
		ShowInventory(PlayerController); 
	else
		HideInventory();

	bDisplayedInventory = !bDisplayedInventory;
}

void UUIContainerComponent::UpdateInventoryUI(UDataTable* InventoryDataTable, const TArray<FInventoryItemSlot>& InventorySlots)
{
	if (InventoryWidget)
	{
		IInventoryWidgetInterface::Execute_UpdateUI(InventoryWidget, InventoryDataTable, InventorySlots);
	}
}

