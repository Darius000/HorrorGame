// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "Inventory/Inventory.h"
#include "InventorySlotDragDropOperation.generated.h"

/**
 * 
 */
UCLASS()
class CRLTSIM_API UInventorySlotDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory", meta = (ExposeOnSpawn))
	FInventoryItemSlot Slot;
};
