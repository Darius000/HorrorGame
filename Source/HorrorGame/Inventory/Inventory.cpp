// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.


#include "Inventory.h"

// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#include "Interactables/UserInterfaces/BaseSubMenu.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Private/PropertyNode.h"
#include "Engine/World.h"
#include "Engine/DataTable.h"
#include "Grid/GridManager.h"

FInventoryItemSlot::FInventoryItemSlot()
{
	
}


void FInventoryItemSlot::SetStackSize(int32 size)
{
	StackSize = size;
}

void FInventoryItemSlot::SetSpaceTaken(FInventoryGridSize spaceTaken)
{
	SpaceTaken = spaceTaken;
}

void FInventoryItemSlot::SetItem(UInventoryItem* newItem)
{
	Item = newItem;
}

void FInventoryItemSlot::SetWeight(float newWeight)
{
	Weight = newWeight;
}

void UContainerComponent::BeginPlay()
{
	Super::BeginPlay();

	if(InventorySearchTable)
	{ 
		for (auto It = Slots.CreateConstIterator(); It; ++It)
		{
			int32 Index = It.GetIndex();
			UInventoryItem* Item = Slots[Index].GetItem();

			if (Item != nullptr)
			{
				FInventoryItemData Data = *InventorySearchTable->FindRow<FInventoryItemData>(Item->GetItemID(), "No Row Found");
				if (Data.bDisplayable || Data.bEquippable)
				{
					SpawnEquipment(Data.WorldItemClass, Data.UnEquipSocketName, Index);		
				}

				ItemClassesAdded.Add(Item->GetClass());
				UsedIndices.Add(Index);
			}
			else
			{
				UnusedIndices.Add(Index);
			}
		}
	}
}

bool UContainerComponent ::AddItem(FName Id, int32 StackSize, int32& ItemIndex)
{
	if (!InventorySearchTable || Id == NAME_None) return false;

	FInventoryItemData* Data = InventorySearchTable->FindRow<FInventoryItemData>(Id, "No Row Found");
	if (!Data) return false;

	TSubclassOf<UObject> ItemClass = Data->InventoryItemClass;

	if (!ItemClass) return false;

	//check if the item isn't listed as one instance only and if so see if its already in the inventory
	bool IsInInventory = ItemClassesAdded.Contains(ItemClass);
	if (Data->bCanOnlyHaveOneInstance && IsInInventory) return false;

	//Check if the weight is too much for the inventory before adding it
	float EstimatedNewWeight = Data->Weight + CurrentWeight;
	if (EstimatedNewWeight > MaxWeight && !(MaxWeight <= 0.0f)) return false;

	FInventoryItemSlot* FoundSlot = Slots.FindByPredicate([ItemClass](const FInventoryItemSlot& slot)
	{
		if (slot.GetItem())
		{
			return slot.GetItem()->GetClass() == ItemClass;
		}

		return false;
	}
	);

	//Stack items together
	if (IsInInventory && Data->bStackable && !(FoundSlot->StackSize >= Data->MaxStackSize))
	{
		//TODO
		/*int32 EstimatedNewStackSize = FoundSlot->StackSize + StackSize;
		int32 Remainder = EstimatedNewStackSize - Data->MaxStackSize;
		int32 NewStackSize = EstimatedNewStackSize - FMath::Clamp(Remainder, 0, Remainder);

		FoundSlot->StackSize = NewStackSize;
		FoundSlot->SetWeight(FoundSlot->GetWeight() + Data->Weight);

		CurrentWeight = EstimatedNewWeight;

		if (Remainder > 0)
		{
			AddItem(Id, Remainder, ItemIndex);
		}

		return true;*/
	}


	ItemIndex = GetNextOpenIndex();

	if (ItemIndex == -1) return false;

	if (SupportedItems.Num() > 0 && !SupportedItems.Find(Data->ItemType)) return false;

	ItemClassesAdded.Add(Data->InventoryItemClass);

	Id.ToString().AppendInt(ItemIndex);

	//Create the inventory item and add a new slot
	UInventoryItem* item = NewObject<UInventoryItem>(this, ItemClass, Id);

	FInventoryItemSlot* Slot = &Slots[ItemIndex];

	Slot->SetItem(item);
	Slot->SetWeight(Data->Weight);
	Slot->SetSpaceTaken(Data->SpaceNeeded);
	Slot->SetStackSize(StackSize);

	CurrentWeight = EstimatedNewWeight;

	OnInventoryUpdated.Broadcast(InventorySearchTable, Slots);

	if(Data->bEquippable || Data->bDisplayable)
	{
		SpawnEquipment(Data->WorldItemClass, Data->UnEquipSocketName, ItemIndex);
	}

	OnItemAdded.Broadcast(*Slot, *Data);

	return true;
}

void UContainerComponent ::Drop(int32 ItemIndex)
{
	if(ItemIndex < 0 && ItemIndex >= Slots.Num()) return;

	FInventoryItemSlot* Slot = &Slots[ItemIndex];
	UInventoryItem* Item = Slots[ItemIndex].GetItem();
	if(!InventorySearchTable || !Item) return;

	FName RowName = Slots[ItemIndex].GetItem()->GetItemID();

	FInventoryItemData* Data = InventorySearchTable->FindRow<FInventoryItemData>(Slot->GetItem()->GetItemID(), "No Row Found");
	if (!Data) return;

	if (!Data->bDroppable) return;

	TSubclassOf<AInventoryWorldItem> ItemClass = Data->WorldItemClass;

	if(!ItemClass) return;

	AGridManager* GridManager = AGridManager::GetGridManager(GetOwner());

	FTransform Transform;
	if(GetOwner())
	{ 
		FVector OwnerLocation = GetOwner()->GetActorLocation();
		FVector OwnerForward = GetOwner()->GetActorForwardVector();
		float Offset = GridManager ?  GridManager->GetGridSize() : 50.0f ;
		FVector DropLocation = OwnerLocation + (OwnerForward * Offset);
		DropLocation.Z = 0.0f;

		if (GridManager != nullptr)
		{ 
			FVector GridLocation = FVector();
			if (GridManager->DoesPointExistOnGrid(DropLocation, GridLocation))
			{
				Transform.SetLocation(GridLocation);
			}
		}
		else
		{
			Transform.SetLocation(DropLocation);
		}
	}

	if (Data->bEquippable || Data->bDisplayable)
	{
		int32 index = EquippableIndices.Find(ItemIndex);
		if (index == INDEX_NONE) return;

		FDetachmentTransformRules DetachmentRules = FDetachmentTransformRules::KeepWorldTransform;
		AActor* EquippableActor = EquippableActors[index];
		EquippableActor->DetachFromActor(DetachmentRules);
		EquippableActor->SetActorTransform(Transform);
		EquippableActor->SetActorEnableCollision(true);

		if (EquippedActor == EquippableActor) EquippedActor = nullptr;
		EquippableActors.RemoveAt(index);
		EquippableIndices.Remove(ItemIndex);
	}
	else
	{
		AActor* SpawnedActor = SpawnItem(ItemClass,  Transform);
		Item->Drop(SpawnedActor, Transform);
	}

	if (Data->bConsumable)
	{
		int32 newSize = Slot->GetStackSize() - 1;

		Slot->SetStackSize(newSize);

		return;
	}

	ItemClassesAdded.Remove(Item->GetClass());

	OnItemDropped.Broadcast(*Slot, *Data);

	RemoveItem(ItemIndex);

	return;
}

void UContainerComponent ::Swap(UContainerComponent* other, int32 index, int32 otherIndex)
{
	if(index < 0 && index >= Slots.Num() && otherIndex < 0 && otherIndex >= other->Slots.Num())
		return;

	FInventoryItemSlot* SlotA = &Slots[index];
	FInventoryItemSlot* SlotB = &other->Slots[otherIndex];
	FInventoryItemSlot CopySlotA = *SlotA;
	FInventoryItemSlot CopySlotB = *SlotB;

	SlotB = &CopySlotA;
	SlotA = &CopySlotB;


	OnInventoryUpdated.Broadcast(InventorySearchTable, Slots);
	other->OnInventoryUpdated.Broadcast(other->InventorySearchTable, other->Slots);

	FInventoryItemData SlotAData = SlotA->GetItem() ? *InventorySearchTable->FindRow<FInventoryItemData>(SlotA->GetItem()->GetItemID(), "No Row Found") : FInventoryItemData();
	FInventoryItemData SlotBData = SlotB->GetItem() ? *InventorySearchTable->FindRow<FInventoryItemData>(SlotB->GetItem()->GetItemID(), "No Row Found") : FInventoryItemData();

	OnItemsSwapped.Broadcast(*SlotA, SlotAData, *SlotB, SlotBData );
}


TArray<int32> UContainerComponent ::GetUnusedIndices() const
{
	return UnusedIndices;
}

int32 UContainerComponent ::GetNextOpenIndex()
{
	if (UnusedIndices.Num() > 0)
	{
		int32 firstIndex = UnusedIndices[0];
		UnusedIndices.Remove(firstIndex);
		UnusedIndices.Sort();
		UsedIndices.AddUnique(firstIndex);
		UsedIndices.Sort();
		return firstIndex;
	}

	if(NextItemIndex < Slots.Num())
	{
		NextItemIndex++;
		UsedIndices.AddUnique(NextItemIndex);
		UsedIndices.Sort();	
		return NextItemIndex;
	}

	return -1;
}

void UContainerComponent ::RemoveIndex(int32 index)
{
	UnusedIndices.AddUnique(index);
	UnusedIndices.Sort();
	UsedIndices.Remove(index);
	UsedIndices.Sort();
}

TArray<int32> UContainerComponent ::GetUsedIndices() const
{
	return UsedIndices;
}


void UContainerComponent ::RemoveItem(int32 index)
{
	if (index >= Slots.Num() || index < 0)
		return;

	RemoveIndex(index);

	CurrentWeight -= Slots[index].GetWeight();	

	Slots[index] = FInventoryItemSlot();
	
	OnInventoryUpdated.Broadcast(InventorySearchTable, Slots);
}

void UContainerComponent::Equip(int32 index, ESocketName Socket)
{
	if(index >= EquippableIndices.Num() && index  < 0) return;

	int32 Key = EquippableIndices[index];

	FInventoryItemData* data = InventorySearchTable->FindRow<FInventoryItemData>(EquippableActors[Key]->GetItemID(), "No Row Found");
	if(!data) return;

	auto SkeletalOwner = GetSkeletalOwner();

	UnEquip();

	if(!data->bDisplayable)
	{ 
		EquippedActor = Cast<AInventoryWorldItem>(SpawnItemOnSocket(data->WorldItemClass, Socket));
	}
	else if(SkeletalOwner)
	{
		EquippedActor = EquippableActors[Key];
		FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
		AttachmentRules.SnapToTargetIncludingScale;

		FName SocketName = EnumToName(ESocketName, data->EquipSocketName);
		EquippedActor->AttachToComponent(SkeletalOwner, AttachmentRules, SocketName);
	}
}

void UContainerComponent::UnEquip()
{
	if (EquippedActor)
	{
		FInventoryItemData* data = InventorySearchTable->FindRow<FInventoryItemData>(EquippedActor->GetItemID(), "No Row Found");
		if (!data) return;

		auto SkeletalOwner = GetSkeletalOwner();

		if (SkeletalOwner && data->bDisplayable)
		{
			FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
			AttachmentRules.SnapToTargetIncludingScale;

			FName SocketName = EnumToName(ESocketName, data->UnEquipSocketName);
			EquippedActor->AttachToComponent(SkeletalOwner, AttachmentRules, SocketName);
		}
		else
		{
			EquippedActor->Destroy();
		}
	}
}

void UContainerComponent::EquipNext()
{
	if(EquippableIndices.Num() == 0) return;

	EquippedItemIndex += 1;
	if(EquippedItemIndex >= EquippableIndices.Num()) EquippedItemIndex = 0;
	int32 Key = EquippableIndices[EquippedItemIndex];
	FInventoryItemData* data = InventorySearchTable->FindRow<FInventoryItemData>(EquippableActors[Key]->GetItemID(), "No Row Found");
	if (!data) return;
	Equip(EquippedItemIndex, data->EquipSocketName);
}

void UContainerComponent::EquipPrevious()
{
	if (EquippableIndices.Num() == 0) return;

	EquippedItemIndex -= 1;
	if(EquippedItemIndex < 0 ) EquippedItemIndex = EquippableIndices.Num() - 1;
	int32 Key = EquippableIndices[EquippedItemIndex];
	FInventoryItemData* data = InventorySearchTable->FindRow<FInventoryItemData>(EquippableActors[Key]->GetItemID(), "No Row Found");
	if (!data) return;
	Equip(EquippedItemIndex, data->EquipSocketName);
}

AActor* UContainerComponent::SpawnItemOnSocket(TSubclassOf<AActor> ItemClass, ESocketName Socket)
{
	AActor* Owner = GetOwner();
	if (!Owner) return nullptr;

	TArray<UActorComponent*> Components;
	Owner->GetComponents(UMeshComponent::StaticClass(), Components, false);

	if(Components.Num() == 0) return nullptr;

	FName SocketName = EnumToName(ESocketName, Socket);

	const USkeletalMeshSocket* SkeletalMeshSocket = nullptr;
	const UStaticMeshSocket* StaticMeshSocket = nullptr;
	UStaticMeshComponent* StaticMeshWithSocket = nullptr;
	USkeletalMeshComponent* SkeletalMeshWithSocket = nullptr;

	for (UActorComponent* ActorComponent : Components)
	{
		if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(ActorComponent))
		{
			StaticMeshSocket = StaticMeshComponent->GetSocketByName(SocketName);
			if (StaticMeshSocket)
			{
				StaticMeshWithSocket = StaticMeshComponent;
				break;
			}
		}
		else if (USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(ActorComponent))
		{
			SkeletalMeshSocket = SkeletalMeshComponent->GetSocketByName(SocketName);
			if (SkeletalMeshSocket)
			{
				SkeletalMeshWithSocket = SkeletalMeshComponent;
				break;
			}
		}
	}

	if(!StaticMeshSocket && !SkeletalMeshSocket) return nullptr;

	AActor* SpawnedActor = nullptr;

	SpawnedActor = SpawnItem(ItemClass, GetOwner()->GetTransform());
		
	if(!SpawnedActor) return nullptr;

	FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
	AttachmentRules.SnapToTargetIncludingScale;

	SpawnedActor->SetActorEnableCollision(false);

	if (StaticMeshSocket)
	{
		SpawnedActor->AttachToComponent(StaticMeshWithSocket, AttachmentRules, SocketName);
	}
	else if (SkeletalMeshSocket)
	{
		SpawnedActor->AttachToComponent(SkeletalMeshWithSocket, AttachmentRules, SocketName);
	}
	
	return SpawnedActor;
}

AActor* UContainerComponent::SpawnItem(TSubclassOf<AActor> ItemClass, const FTransform& Transform)
{
	auto World = GetWorld();

	if (!ItemClass || !World) return nullptr;

	FActorSpawnParameters SpawnParameters = FActorSpawnParameters();
	SpawnParameters.Owner = GetOwner();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* SpawnedActor = World->SpawnActor<AActor>(ItemClass, Transform, SpawnParameters);

	return SpawnedActor;
}

void UContainerComponent::InitilaizeDefaultIndices()
{
	UsedIndices.Reset(0);
	UnusedIndices.Reset(0);

	//Store the indices of existing slots in the inventory

	for (int32 i = 0; i < Slots.Num(); i++)
	{

		if (Slots[i].GetItem())
		{
			UsedIndices.AddUnique(i);
		}
		else if (!Slots[i].GetItem())
		{
			UnusedIndices.AddUnique(i);
		}
		NextItemIndex = i + 1;

		LOGINVENTORY("%d", NextItemIndex);
	}
}

void UContainerComponent::SpawnEquipment(TSubclassOf<AActor> EquipmentClass, ESocketName UnEquipSocketName, int32 Index)
{
	AInventoryWorldItem* DisplayableItem = Cast<AInventoryWorldItem>(SpawnItemOnSocket(EquipmentClass, UnEquipSocketName));
	int32 index = EquippableActors.Add(DisplayableItem);
	EquippableIndices.Add(Index);
}

class USkeletalMeshComponent* UContainerComponent::GetSkeletalOwner()
{
	if (ACharacter* characterOwner = Cast<ACharacter>(GetOwner()))
	{
		return characterOwner->GetMesh();
	}

	return nullptr;
}
