// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ToolTipWidget.generated.h"

/**
 * 
 */
UCLASS()
class CRLTSIM_API UToolTipWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Text;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ToolTipTextBlock;

	bool Initialize() override;

	UFUNCTION(BlueprintCallable)
	FText GetTipText() const { return Text; }

	UFUNCTION(BlueprintCallable)
	void SetTipText(FText newText);
};
