// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.


#include "Container.h"

AContainer::AContainer()
{
	ContainerComponent = CreateDefaultSubobject<UContainerComponent>(FName("Inventory"));
}

void AContainer::OnInteractWith_Implementation(class AActor* actor)
{
	//ContainerComponent->ToggleInventory(Cast);
}
