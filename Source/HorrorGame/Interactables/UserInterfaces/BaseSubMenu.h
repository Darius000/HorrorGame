// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseSubMenu.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FExitMenu);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDisplayMenu);

class UInteractComponent;
/**
 * 
 */
UCLASS()
class CRLTSIM_API UBaseSubMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Menu")
	FText GetInteractableName();

	UFUNCTION(BlueprintCallable, Category = "Menu")
		void SetInteractComponent(UInteractComponent* component);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Menu")
		UInteractComponent* GetInteractComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void Display();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void Exit();

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Menu")
	FExitMenu OnExit;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Menu")
	FDisplayMenu OnDisplayMenu;

protected:

	UPROPERTY()
	UInteractComponent* InteractComponent;
};
