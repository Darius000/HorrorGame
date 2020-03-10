// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactables/DataAssets/InteractionAnimationData.h"
#include "InteractInterface.generated.h"

class UBaseSubMenu;


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CRLTSIM_API IInteractInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	//UFUNCTION(BlueprintCallable , BlueprintImplementableEvent, Category = "Interactable")
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable")
	void OnInteractWith(class AActor* actor) ;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable")
	bool CanInteractWith() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable")
	void SetCanInteract(bool bInteractable);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable")
	FString GetInteractionText() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable")
	FString GetToolTipText() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable|Focus")
	void BeginFocus();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable|Focus")
	void EndFocus();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable|Focus")
	bool IsFocused();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable|Highlight")
	void Highlight();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable|Highlight")
	void UnHighlight();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable|Menu")
	TSubclassOf<UBaseSubMenu> GetSubMenuClass() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable|Animation")
	bool ShouldCharacterWalkTo() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interactable|Animation")
	FVector GetAnimationStartLocation() const;
};

