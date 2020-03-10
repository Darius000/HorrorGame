// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#include "CrltSim.h"
#include "InventoryItem.h"
#include "Inventory/Inventory.h"
#include "Interactables/Components/InteractComponent.h"

UAction::UAction()
{

}

void UAction::Execute(UObject* Instigator)
{
	OnBeginExecute(Instigator);
	OnExecute(Instigator);
	OnEndExecute(Instigator);
}

void UAction::OnBeginExecute_Implementation(UObject* Instigator)
{
	OnBeginExecution.Broadcast(Instigator);
}

void UAction::OnExecute_Implementation(UObject* Instigator)
{
	OnExecution.Broadcast(Instigator);
}

void UAction::OnEndExecute_Implementation(UObject* Instigator)
{
	OnEndExecution.Broadcast(Instigator);
}

// Sets default values
AInventoryWorldItem::AInventoryWorldItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AInventoryWorldItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInventoryWorldItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FName AInventoryWorldItem::GetItemID() const
{
	return ItemID;
}

int32 AInventoryWorldItem::GetStackSize() const
{
	return StackSize;
}

void AInventoryWorldItem::OnInteractWith_Implementation(class AActor* actor)
{
	Super::OnInteractWith_Implementation(actor);

	if (UContainerComponent* component = Cast<UContainerComponent>(actor->GetComponentByClass(UContainerComponent::StaticClass())))
	{
		int32 index = -1;
		if (component->AddItem(ItemID, StackSize, index))
		{
			LOGINVENTORY("Added Pickup %s", *ItemID.ToString());

			if (UInteractComponent* component2 = Cast<UInteractComponent>(actor->GetComponentByClass(UInteractComponent::StaticClass())))
			{
				component2->FocusNext();
			}

			Destroy();
		}
	}
}

UInventoryItem::UInventoryItem()
{

}

FName UInventoryItem::GetItemID() const
{
	return ItemID;
}


void UInventoryItem::Use(AActor* Instigator, AActor* OtherActor, UAction* ActionToExecute)
{
	int32 AbilityIndex = INDEX_NONE;
	bool HaveActionAbility = Abilities.Find(ActionToExecute->StaticClass(), AbilityIndex);

	if (HaveActionAbility)
	{
		OnUse(Instigator, OtherActor, ActionToExecute);
	}	
}

void UInventoryItem::Drop(AActor* WorldItemClass, const FTransform& Transform)
{
	OnDrop(WorldItemClass, Transform);
}

void UInventoryItem::OnUse_Implementation(AActor* Instigator, AActor* OtherActor, UAction* ActionToExecute)
{

}

void UInventoryItem::OnDrop_Implementation(AActor* WorldItemClass, const FTransform& Transform)
{

}



