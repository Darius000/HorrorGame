// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.


#include "BaseInteractable.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Interactables/UserInterfaces/ToolTipWidget.h"

// Sets default values
ABaseInteractable::ABaseInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<UArrowComponent>(FName("Root"));
	SetRootComponent(Root);

	DefaultCollision = CreateDefaultSubobject<UBoxComponent>(FName("DefaultCollision"));

	bIsInteractable = true;
}

// Called when the game starts or when spawned
void ABaseInteractable::BeginPlay()
{
	Super::BeginPlay();

	OnBeginCursorOver.AddDynamic(this, &ABaseInteractable::BeginCursorOver);
	OnEndCursorOver.AddDynamic(this, &ABaseInteractable::EndCursorOver);
	OnClicked.AddDynamic(this, &ABaseInteractable::Clicked);
}

// Called every frame
void ABaseInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseInteractable::OnInteractWith_Implementation(class AActor* actor)
{
	FString OtherName = "";
	FString MyName = "";
	actor->GetName(OtherName);
	GetName(MyName);

	UE_LOG(LogTemp, Display, TEXT("%s interacted with %s"), *OtherName, *MyName);
}

bool ABaseInteractable::CanInteractWith_Implementation() const
{
	return bIsInteractable;
}

void ABaseInteractable::SetCanInteract_Implementation(bool bInteractable)
{
	bIsInteractable = bInteractable;
}

FString ABaseInteractable::GetInteractionText_Implementation() const
{
	return FString("Interact with ") + GetName();
}

FString ABaseInteractable::GetToolTipText_Implementation() const
{
	return "This is a tool tip for " + GetName();
}

void ABaseInteractable::Highlight_Implementation()
{
	UStaticMeshComponent* HighlightedMesh = GetHighlightedMesh();
	if(HighlightedMesh) HighlightedMesh->SetRenderCustomDepth(true);
}

void ABaseInteractable::UnHighlight_Implementation()
{
	UStaticMeshComponent* HighlightedMesh = GetHighlightedMesh();
	if (HighlightedMesh) HighlightedMesh->SetRenderCustomDepth(false);
}

UStaticMeshComponent* ABaseInteractable::GetHighlightedMesh_Implementation() const
{
	return nullptr;
}

void ABaseInteractable::BeginCursorOver(AActor* TouchedActor)
{
	//IInteractInterface::Execute_Highlight(this);

	if(!ToolTipWidget && ToolTipWidgetClass) 
	{
		ToolTipWidget =  Cast<UToolTipWidget>(UWidgetBlueprintLibrary::Create(this, ToolTipWidgetClass, UGameplayStatics::GetPlayerController(this, 0 )));

		if(!ToolTipWidget) return;

		FVector2D screenPosition;
		UGameplayStatics::ProjectWorldToScreen(UGameplayStatics::GetPlayerController(this,0), TouchedActor->GetActorLocation(), screenPosition);

		auto Anchors = FAnchors(screenPosition.X - 125, screenPosition.Y - 125, screenPosition.X + 125, screenPosition.Y + 125);
		ToolTipWidget->SetAnchorsInViewport(Anchors);
		ToolTipWidget->SetPositionInViewport(FVector2D(screenPosition.X + (125 / 2), screenPosition.Y - (125 / 2)));
	}

	if (!ToolTipWidget) return;

	FString ToolTip = IInteractInterface::Execute_GetToolTipText(this);
	ToolTipWidget->SetTipText(FText::FromString(ToolTip));
	ToolTipWidget->AddToViewport();
}

void ABaseInteractable::EndCursorOver(AActor* TouchedActor)
{
	//IInteractInterface::Execute_UnHighlight(this);

	if(!ToolTipWidget) return;
	ToolTipWidget->RemoveFromViewport();
}

void ABaseInteractable::Clicked(AActor* TouchedActor, FKey ButtonPressed)
{
	
}

