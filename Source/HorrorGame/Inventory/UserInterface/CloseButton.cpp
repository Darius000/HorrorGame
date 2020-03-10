// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.


#include "CloseButton.h"
#include "Components/PanelWidget.h"

void UCloseButton::Exit()
{
	UPanelWidget* Parent = GetParent();

	if(Parent)
		Parent->RemoveFromParent();

	OnExit.Broadcast();
}