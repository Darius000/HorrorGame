// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.


#include "ToolTipWidget.h"
#include "Components/TextBlock.h"

bool UToolTipWidget::Initialize()
{
	if(!Super::Initialize()) return false;

	return true;
}

void UToolTipWidget::SetTipText(FText newText)
{
	Text = newText;
	ToolTipTextBlock->Text = newText;
}

