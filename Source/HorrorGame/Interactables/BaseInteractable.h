// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactables/Interfaces/InteractInterface.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BaseInteractable.generated.h"

UCLASS()
class CRLTSIM_API ABaseInteractable : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseInteractable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	void OnInteractWith_Implementation(class AActor* actor) override;

	bool CanInteractWith_Implementation() const override;

	void SetCanInteract_Implementation(bool bInteractable) override;

	FString GetInteractionText_Implementation() const override;

	FString GetToolTipText_Implementation() const override;

	void Highlight_Implementation() override;

	void UnHighlight_Implementation() override;


	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
		UArrowComponent* Root;

		/* USed for interact component detection*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
		UBoxComponent* DefaultCollision;
		
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		UStaticMeshComponent* GetHighlightedMesh() const;
		virtual UStaticMeshComponent* GetHighlightedMesh_Implementation() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class UToolTipWidget> ToolTipWidgetClass;

	UFUNCTION()
	void BeginCursorOver(  AActor* TouchedActor );

	UFUNCTION()
	void EndCursorOver (AActor* TouchedActor );

	UFUNCTION()
	void Clicked(AActor* TouchedActor, FKey ButtonPressed);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable")
	bool bIsInteractable;

	UPROPERTY()
	class UToolTipWidget* ToolTipWidget;
};
