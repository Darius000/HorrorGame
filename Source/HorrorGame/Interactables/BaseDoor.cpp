// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#include "CrltSim.h"
#include "BaseDoor.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "TurnBasedNavProxy.h"
#include "NavLinkCustomComponent.h"
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "LevelEditorViewport.h"

ABaseDoor::ABaseDoor()
{
	PrimaryActorTick.bCanEverTick = true;

	AnimationComponent = CreateDefaultSubobject<UAnimationComponent>(FName("Door Animation"));
	AnimationComponent->SetupAttachment(Root);
	AnimationComponent->PlayBackMode = EPlayBackMode::ONCE;
	AnimationComponent->RuntimeTransformCurve.Rotation.bIsAdditive = false;

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Door Mesh"));
	DoorMesh->SetupAttachment(AnimationComponent);

	ActorDetection = CreateDefaultSubobject<UBoxComponent>(FName("Actor Detection"));
	ActorDetection->SetupAttachment(Root);

	DoorState = EDoorState::CLOSED;
}

void ABaseDoor::BeginPlay()
{
	Super::BeginPlay();

	DefaultDoorCollision = DoorMesh->GetCollisionEnabled();
	AnimationComponent->OnFinished.AddDynamic(this, &ABaseDoor::OnAnimationFinished);

	if (DoorState == EDoorState::OPENED && DoorType != EDoorType::AUTOMATIC)
	{
		AnimationComponent->SetPlaybackPosition(AnimationComponent->GetAnimationMaxTime());
	}

	ActorDetection->OnComponentBeginOverlap.AddDynamic(this, &ABaseDoor::OnComponentBeginOverlap);
	ActorDetection->OnComponentEndOverlap.AddDynamic(this, &ABaseDoor::OnComponentEndOverlap);

	// spawn nav proxy, if configured
	if (bShouldSpawnNavProxy && HasAuthority())
	{
		auto World = GetWorld();
		if (World == nullptr) return;

		SpawnNavLinkProxy(World);

		NavLinkProxy->ConfigureLinks(NavProxyOffsetStart, NavProxyOffsetEnd, true);
	}
}

void ABaseDoor::SpawnNavLinkProxy(UWorld * World)
{
	if (World == nullptr) 
	{
		World = GetWorld();
	}
	if (!ensure(World != nullptr)) return;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	NavLinkProxy = World->SpawnActor<ATurnBasedNavProxy>(SpawnParameters);
	if (NavLinkProxy == nullptr) return;
	NavLinkProxy->AttachToActor(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
}

void ABaseDoor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void ABaseDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ABaseDoor::Open()
{
	if(DoorState == EDoorState::CLOSED && IInteractInterface::Execute_CanInteractWith(this))
	{ 
		DoorState = EDoorState::OPENING;

		if (OpenSound) UGameplayStatics::PlaySoundAtLocation(this, OpenSound, GetActorLocation());

		AnimationComponent->Play(EPlayBackStart::PLAY);
	}
}

void ABaseDoor::Close()
{
	if (bDisableCollisionWhenOpen)
	{
		DoorMesh->SetCollisionEnabled(DefaultDoorCollision);
		DoorMesh->SetCanEverAffectNavigation(true);
	}

	if(DoorState == EDoorState::OPENED && IInteractInterface::Execute_CanInteractWith(this))
	{ 
		DoorState = EDoorState::CLOSING;

		if (CloseSound) UGameplayStatics::PlaySoundAtLocation(this, CloseSound, GetActorLocation());

		AnimationComponent->Play(EPlayBackStart::REVERSE);
	}
}

void ABaseDoor::OnAnimationFinished(int32 TimesPlayed)
{
	if(DoorState == EDoorState::CLOSING) DoorState = EDoorState::CLOSED;
	else if(DoorState == EDoorState::OPENING) DoorState = EDoorState::OPENED;

	if (bDisableCollisionWhenOpen)
	{
		DoorMesh->SetCollisionEnabled(DoorState == EDoorState::CLOSED ? DefaultDoorCollision : ECollisionEnabled::NoCollision);
		DoorMesh->SetCanEverAffectNavigation(DoorState != EDoorState::OPENED);
	}
}

#if WITH_EDITOR
bool ABaseDoor::CanEditChange(const UProperty* InProperty) const
{
	const bool canEdit = Super::CanEditChange(InProperty);

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(ABaseDoor, DoorState))
	{
		return DoorType != EDoorType::AUTOMATIC;
	}

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(ABaseDoor, AutomaticDoorTimer))
	{
		return DoorType == EDoorType::AUTOMATIC || AutomaticFor.Num() > 0;
	}

	return canEdit;
}
#endif

void ABaseDoor::OnInteractWith_Implementation(class AActor* actor)
{
	if (DoorState == EDoorState::CLOSED)
		Open();
	else if (DoorState == EDoorState::OPENED)
		Close();
}

void ABaseDoor::OnLocked_Implementation()
{
	IInteractInterface::Execute_SetCanInteract(this, false);
}

void ABaseDoor::OnUnlocked_Implementation()
{
	IInteractInterface::Execute_SetCanInteract(this, true);
}

void ABaseDoor::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (DoorType == EDoorType::AUTOMATIC || (AutomaticFor.Num() > 0 && AutomaticFor.Find(OtherActor->StaticClass())))
	{
		Open();
	}
}

void ABaseDoor::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(DoorType == EDoorType::AUTOMATIC || (AutomaticFor.Num() > 0 && AutomaticFor.Find(OtherActor->StaticClass())))
	{ 
		AutoCloseDoor();
	}
}

void ABaseDoor::AutoCloseDoor()
{
	GetWorld()->GetTimerManager().SetTimer(CloseDoorTimerHandle, this, &ABaseDoor::Close, AutomaticDoorTimer, false);
}
