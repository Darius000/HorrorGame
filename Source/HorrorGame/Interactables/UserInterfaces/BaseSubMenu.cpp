// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#include "CrltSim.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Interactables/Components/InteractComponent.h"
#include "BaseSubMenu.h"


FText UBaseSubMenu::GetInteractableName()
{
	if (GetInteractComponent())
		return FText::FromName(GetInteractComponent()->GetCurrentInteractable()->GetFName());

	return FText::GetEmpty();
}

void UBaseSubMenu::SetInteractComponent(UInteractComponent* component)
{
	InteractComponent = component;
}

UInteractComponent* UBaseSubMenu::GetInteractComponent() const
{
	return InteractComponent;
}

void UBaseSubMenu::Display()
{
	SetVisibility(ESlateVisibility::Visible);

	OnDisplayMenu.Broadcast();

	AddToViewport();
}

void UBaseSubMenu::Exit()
{
	SetVisibility(ESlateVisibility::Hidden);

	OnExit.Broadcast();

	RemoveFromViewport();
}
