// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interactables/Items/InventoryItem.h"
#include "Interactables/UserInterfaces/BaseSubMenu.h"
#include "Inventory.generated.h"

class UContainerComponent;
class UInventoryItemSlot;

#define LOGINVENTORY(text, ...) UE_LOG(LogTemp, Log, TEXT(text), __VA_ARGS__)


USTRUCT(BlueprintType)
struct CRLTSIM_API FInventoryItemSlot 
{
	GENERATED_BODY()

public:
	FInventoryItemSlot();

	UPROPERTY()
		UContainerComponent * Inventory = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
		UInventoryItem* Item = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 StackSize = 0;

		int32 GetStackSize() const { return StackSize;}

		void SetStackSize(int32 size);

		void SetSpaceTaken(FInventoryGridSize spaceTaken);

		FInventoryGridSize GetSpaceTaken() const { return SpaceTaken;}

		void SetItem(UInventoryItem* newItem);

		UInventoryItem* GetItem() const {return Item;}

private:
	UPROPERTY()
		float Weight;

	UPROPERTY()
		FInventoryGridSize SpaceTaken;

public:
	void SetWeight(float newWeight);

	float GetWeight() const { return Weight;}
};


/**
 * 
 */
 DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventoryUpdateSigniture, UDataTable*, InventoryDataTable, const TArray<FInventoryItemSlot>&, InventorySlots);
 DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FInventoryItemSwap, const FInventoryItemSlot& , SlotA, FInventoryItemData, SlotAData, const FInventoryItemSlot&, SlotB, FInventoryItemData, SlotBData);
 DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventoryItemDrop, const FInventoryItemSlot&, Slot, FInventoryItemData, Data);
 DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventoryItemAdded, const FInventoryItemSlot& , Slot, FInventoryItemData, Data);

UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class CRLTSIM_API UContainerComponent  : public UActorComponent
{
	GENERATED_BODY()

protected:

	void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FInventoryGridSize MaxSpace = FInventoryGridSize(10, 10);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxWeight = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FInventoryItemSlot> Slots;

	//List of objects that can be put in the inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<EInventoryItemType> SupportedItems;

private:
	UPROPERTY()
		int32 CurrentSpaceTaken = 0;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
		float CurrentWeight = 0.0f;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
		TArray<TSubclassOf<UObject>> ItemClassesAdded;


	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
		int32 NextItemIndex = -1;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
		int32 EquippedItemIndex = -1;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
		TArray<int32> UnusedIndices;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
		TArray<int32> UsedIndices;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
		AInventoryWorldItem* EquippedActor;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
		TArray<AInventoryWorldItem*> EquippableActors;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
		TArray<int32> EquippableIndices;

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintAssignable)
		FInventoryUpdateSigniture OnInventoryUpdated;

	UPROPERTY(VisibleDefaultsOnly, BlueprintAssignable)
		FInventoryItemAdded OnItemAdded;

	UPROPERTY(VisibleDefaultsOnly, BlueprintAssignable)
		FInventoryItemDrop OnItemDropped;

	UPROPERTY(VisibleDefaultsOnly, BlueprintAssignable)
		FInventoryItemSwap OnItemsSwapped;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
		UDataTable* InventorySearchTable;

	UFUNCTION( BlueprintCallable,  Category = "Inventory")
		bool AddItem(FName Id, int32 StackSize, int32& ItemIndex);

	UFUNCTION(BlueprintCallable,  Category = "Inventory")
		void Drop(int32 itemIndex);

	UFUNCTION(BlueprintCallable,  Category = "Inventory")
		void Swap(UContainerComponent* other, int32 index, int32 otherIndex);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		TArray<int32> GetUnusedIndices() const;

	UFUNCTION()
		int32 GetNextOpenIndex();

	UFUNCTION()
		void RemoveIndex(int32 index);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		TArray<int32> GetUsedIndices() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		float GetCurrentWeight() const { return CurrentWeight; }

	UFUNCTION()
		void RemoveItem(int32 index);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void Equip(int32 index, ESocketName Socket);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void UnEquip();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void EquipNext();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void EquipPrevious();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		AInventoryWorldItem* GetEquippedActor() { return EquippedActor; };

//protected:
	UFUNCTION(BlueprintCallable)
		AActor* SpawnItemOnSocket(TSubclassOf<AActor> ItemClass, ESocketName Socket);

	UFUNCTION(BlueprintCallable)
		AActor* SpawnItem(TSubclassOf<AActor> ItemClass, const FTransform& Transform);

private:

	UFUNCTION()
		void InitilaizeDefaultIndices();


	UFUNCTION()
		void SpawnEquipment(TSubclassOf<AActor> Equipmentlass, ESocketName UnEquipSocketName, int32 Index);

protected:
	class USkeletalMeshComponent* GetSkeletalOwner();
};
