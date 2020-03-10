// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CloseButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FExitSigniture);
/**
 * 
 */
UCLASS()
class CRLTSIM_API UCloseButton : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UButton* CloseButton;

	UFUNCTION(BlueprintCallable)
	void Exit();

	UPROPERTY(BlueprintAssignable)
	FExitSigniture OnExit;
};
