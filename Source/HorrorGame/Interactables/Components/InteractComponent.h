// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "Interactables/UserInterfaces/BaseSubMenu.h"
#include "Curves/CurveFloat.h"
#include "InteractComponent.generated.h"

#define DEFAULTDEBUGCOLOR Red
#define ONSCREENDEBUGTEXT(Text, Color) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Color, Text)
#define DISPLAYDEBUGTEXT(Text) ONSCREENDEBUGTEXT(Text, DEFAULTDEBUGCOLOR)


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionSigniture, bool, interactedWith, AActor*, otherActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableFocusedSigniture, AActor*, otherActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractableEndFocusedSigniture, AActor*, otherActor);

UCLASS( ClassGroup=(Interactions), meta=(BlueprintSpawnableComponent) )
class CRLTSIM_API UInteractComponent : public USphereComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType;

public:
	UFUNCTION(BlueprintCallable, Category = "Interactable")
	void Interact();

	//INteract with a specific actor
	UFUNCTION(BlueprintCallable, Category = "Interactable")
	void InteractWith(AActor* actor);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interactable")
	AActor* GetCurrentInteractable() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interactable")
	TArray<AActor*> GetInteractableActors() const { return InteractableActors; }

	UFUNCTION(BlueprintCallable, Category = "Interactable")
	void Focus(AActor* item);

	UFUNCTION(BlueprintCallable, Category = "Interactable")
	void FocusNext();

	UFUNCTION(BlueprintCallable, Category = "Interactable")
	void FocusPrevious();

	UFUNCTION(BlueprintCallable, Category = "Interactable")
	void ToggleSubMenu();

	UFUNCTION()
	void ShowItemMenu();

	UFUNCTION()
	void HideItemMenu();

public:

	UPROPERTY(BlueprintAssignable)
	FInteractionSigniture OnInteraction;

	UPROPERTY(BlueprintAssignable)
	FInteractableFocusedSigniture OnBeginFocus;

	UPROPERTY(BlueprintAssignable)
	FInteractableEndFocusedSigniture OnEndFocus;

private:

	UFUNCTION()
	void AddOverlapBindings(UPrimitiveComponent* component);

	UFUNCTION()
	void ChangeFocusedItem(int32 index);

	UFUNCTION()
	const bool IsItemInRange(AActor* item);

	UPROPERTY()
	int32 InteractablesIndex;

	UPROPERTY()
	bool bIsShowingSubMenu;

	UFUNCTION()
	void OnCloseMenu();

	UFUNCTION()
	void OnOpenMenu();

	ACharacter* CharacterOwner;

	APlayerController* PlayerController;

private:
	UPROPERTY(VisibleAnywhere, Category = "Debug")
		AActor* CurrentInteractable;

	UPROPERTY(VisibleAnywhere, Category = "Debug")
		TArray<AActor*> InteractableActors;	

	UPROPERTY(VisibleAnywhere, Category = "Debug")
		TMap<TSubclassOf<UBaseSubMenu>, UBaseSubMenu*> InteractableWidgetInstances;

	UPROPERTY(VisibleAnywhere, Category = "Debug")
		UBaseSubMenu* CurrentDisplayedMenu;

protected:
	UFUNCTION()
		void OnActorBeginOverlap(AActor* OverlappedActor, AActor* otherActor);

	UFUNCTION()
		void OnActorEndOverlap(AActor* OverlappedActor, AActor* otherActor);
};
