// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#include "CrltSim.h"
#include "InteractComponent.h"
#include "Interactables/Interfaces/InteractInterface.h"
#include "AIController.h"
#include "Engine/Engine.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"


// Sets default values for this component's properties
UInteractComponent::UInteractComponent()
	:InteractablesIndex(0), bIsShowingSubMenu(false) , CurrentInteractable(nullptr)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInteractComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...	

	CharacterOwner = Cast<ACharacter>(GetOwner());
	PlayerController = CharacterOwner ? Cast<APlayerController>(CharacterOwner->GetController()) : GetWorld()->GetFirstPlayerController();

	AddOverlapBindings(this);
}


// Called every frame
void UInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	
}

void UInteractComponent::Interact()
{
	if (CurrentInteractable)
	{
		InteractWith(CurrentInteractable);	
	}
}

void UInteractComponent::InteractWith(AActor* actor)
{
	if (UKismetSystemLibrary::DoesImplementInterface(actor, UInteractInterface::StaticClass()))
	{
		if (IInteractInterface::Execute_CanInteractWith(actor))
		{
			IInteractInterface::Execute_OnInteractWith(actor, GetOwner());

			FString text = IInteractInterface::Execute_GetInteractionText(actor);
			UE_LOG(LogTemp, Display, TEXT("%s"), *text);

			OnInteraction.Broadcast(true, actor);
		}
	
		return;
	}

	return;
}

AActor* UInteractComponent::GetCurrentInteractable() const
{
	return CurrentInteractable;
}

void UInteractComponent::Focus(AActor* item)
{
	int32 index;

	if (InteractableActors.Find(item, index))
	{
		ChangeFocusedItem(index);
	}
}

void UInteractComponent::FocusNext()
{
	if (InteractableActors.Num() == 0)
		return;

	InteractablesIndex++;

	InteractablesIndex %= InteractableActors.Num();

	ChangeFocusedItem(InteractablesIndex);
}

void UInteractComponent::FocusPrevious()
{
	if (InteractableActors.Num() == 0)
		return;

	InteractablesIndex--;

	if(InteractablesIndex < 0)
		InteractablesIndex = InteractableActors.Num() - 1;

	ChangeFocusedItem(InteractablesIndex);

}

void UInteractComponent::ToggleSubMenu()
{
	bIsShowingSubMenu ? HideItemMenu() : ShowItemMenu();
}

void UInteractComponent::ShowItemMenu()
{
	if(CurrentInteractable == nullptr) return;
	
	TSubclassOf<UBaseSubMenu> widgetClass = IInteractInterface::Execute_GetSubMenuClass(CurrentInteractable);

	UBaseSubMenu* menu = InteractableWidgetInstances.FindOrAdd(widgetClass);

	if (menu == nullptr)
	{
		if (!widgetClass.Get()->IsValidLowLevel()) return;

		menu = CreateWidget<UBaseSubMenu>(PlayerController, widgetClass);
		InteractableWidgetInstances.Add(widgetClass, menu);
		
		menu->OnExit.AddDynamic(this, &UInteractComponent::OnCloseMenu);
		menu->OnDisplayMenu.AddDynamic(this, &UInteractComponent::OnOpenMenu);
	}
	
	if (menu == nullptr) return;

	CurrentDisplayedMenu = menu;
	CurrentDisplayedMenu->SetInteractComponent(this);
	
	if (!menu->IsInViewport())
	{
		menu->Display();
	}
}

void UInteractComponent::HideItemMenu()
{
	if (CurrentDisplayedMenu != nullptr && CurrentDisplayedMenu->IsValidLowLevel() && CurrentDisplayedMenu->GetVisibility() != ESlateVisibility::Hidden)
	{
		CurrentDisplayedMenu->Exit();
	}
}


void UInteractComponent::AddOverlapBindings(UPrimitiveComponent* component)
{
	if (component != nullptr)
	{
		//component->OnComponentBeginOverlap.AddDynamic(this, &UInteractComponent::OnComponentBeginOverlap);
		//component->OnComponentEndOverlap.AddDynamic(this, &UInteractComponent::OnComponentEndOverlap);
		component->GetOwner()->OnActorBeginOverlap.AddDynamic(this, &UInteractComponent::OnActorBeginOverlap);
		component->GetOwner()->OnActorEndOverlap.AddDynamic(this, &UInteractComponent::OnActorEndOverlap);
	}
}

void UInteractComponent::ChangeFocusedItem(int32 index)
{
	if (InteractableActors.Num() > 0)
	{
		if (CurrentInteractable)
		{
			IInteractInterface::Execute_EndFocus(CurrentInteractable);
			OnEndFocus.Broadcast(CurrentInteractable);
		}

		CurrentInteractable = InteractableActors[index];

		IInteractInterface::Execute_BeginFocus(CurrentInteractable);
		OnBeginFocus.Broadcast(CurrentInteractable);

		if (bIsShowingSubMenu) 
		{
			HideItemMenu();
			ShowItemMenu();
		}
	}
}

const bool UInteractComponent::IsItemInRange(AActor* item)
{
	TArray<AActor*> ignoredActors;
	FHitResult hitResult;
	FVector BoxExtent, Origin;

	item->GetActorBounds(false, Origin, BoxExtent);

	//send a trace form the item to owner to check if it is not blocked from view
	UKismetSystemLibrary::LineTraceSingle(item, item->GetActorLocation() + (item->GetActorForwardVector() * 2.0f), 
		GetOwner()->GetActorLocation(), ETraceTypeQuery::TraceTypeQuery2, true, ignoredActors,
		DrawDebugType, hitResult, true);

	if(!hitResult.GetActor()) return false;

	//ONSCREENDEBUGTEXT(hitResult.GetActor()->GetFName().ToString(), Purple);
	//UE_LOG(LogTemp, Display, TEXT("Interact with: %s"), *hitResult.GetActor()->GetFName().ToString());

	if (hitResult.GetActor() == GetOwner()) return true;

	return false;
}

void UInteractComponent::OnCloseMenu()
{
	CurrentDisplayedMenu = nullptr;

	UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);

	PlayerController->bShowMouseCursor = false;

	bIsShowingSubMenu = false;
}

void UInteractComponent::OnOpenMenu()
{
	FVector2D screenPosition;
	UGameplayStatics::ProjectWorldToScreen(PlayerController, CurrentInteractable->GetActorLocation(), screenPosition);

	auto Anchors = FAnchors(screenPosition.X - 125, screenPosition.Y - 125, screenPosition.X + 125, screenPosition.Y + 125);
	CurrentDisplayedMenu->SetAnchorsInViewport(Anchors);
	CurrentDisplayedMenu->SetPositionInViewport(FVector2D(screenPosition.X + (125 / 2), screenPosition.Y - (125 / 2)));

	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController, CurrentDisplayedMenu);

	PlayerController->bShowMouseCursor = true;

	bIsShowingSubMenu = true;
}

void UInteractComponent::OnActorBeginOverlap(AActor* OverlappedActor, AActor* otherActor)
{
	//ONSCREENDEBUGTEXT("Begin Overlap" + OverlappedActor->GetFName().ToString(), Purple);
	//ONSCREENDEBUGTEXT("Begin Overlap" + otherActor->GetFName().ToString(), Purple);
	UE_LOG(LogTemp, Display, TEXT("Begin Overlap with: %s"), *otherActor->GetFName().ToString())

	if (!otherActor)
		return;

	if (!UKismetSystemLibrary::DoesImplementInterface(otherActor, UInteractInterface::StaticClass())) return;

	if (IsItemInRange(otherActor))
	{
		InteractableActors.AddUnique(otherActor);

		IInteractInterface::Execute_Highlight(otherActor);

		ChangeFocusedItem(0);
	}

	return;
}

void UInteractComponent::OnActorEndOverlap(AActor* OverlappedActor, AActor* otherActor)
{
	//ONSCREENDEBUGTEXT("End Overlap", Purple);
	UE_LOG(LogTemp, Display, TEXT("End Overlap with: %s"), *otherActor->GetFName().ToString())

	int32 index = 0;

	if (InteractableActors.Find(otherActor, index))
	{
		if (IInteractInterface::Execute_IsFocused(otherActor))
		{
			IInteractInterface::Execute_EndFocus(otherActor);
		}

		CurrentInteractable = nullptr;

		if (bIsShowingSubMenu) HideItemMenu();

		IInteractInterface::Execute_UnHighlight(otherActor);

		InteractableActors.Remove(otherActor);
	}

	return;
}


