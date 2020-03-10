// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.


#include "InventoryWidget.h"
#include "Components/PanelWidget.h"
#include "Components/UniformGridSlot.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UInventoryWidget::SetContainerComponent_Implementation(class UUIContainerComponent* ContainerComponent)
{
	UIContainerComponent = ContainerComponent;
}

class UUIContainerComponent* UInventoryWidget::GetContainerComponent_Implementation() const
{
	return UIContainerComponent;
}

void UInventoryWidget::UpdateUI_Implementation(class UDataTable* InventoryDataTable, const TArray<FInventoryItemSlot>& inventorySlots)
{
	if(!InventoryPanel) return;

	InventoryPanel->ClearChildren();

	int32 row = 0;
	int32 column = 0;

	for (FInventoryItemSlot slot : inventorySlots)
	{
		UInventorySlotWidget* SlotWidget = Cast<UInventorySlotWidget>(UWidgetBlueprintLibrary::Create(this, SlotClass, GetOwningPlayer()));

		if(!SlotWidget) break;

		SlotWidget->InventorySlotData = slot;
		UPanelSlot* PanelSlot = InventoryPanel->AddChild(SlotWidget);
		UUniformGridSlot* GridPanel = Cast<UUniformGridSlot>(PanelSlot);

		if (GridPanel)
		{
			GridPanel->SetRow(row);
			GridPanel->SetColumn(column);
		}

		if (row != MaxRows)
		{
			column ++;

			if (column == MaxColumns)
			{
				row++;
				column = 0;
			}
		}
	}
}
