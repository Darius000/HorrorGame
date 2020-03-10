// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#include "CrltSim.h"
#include "Lock.h"

// Sets default values for this component's properties
ALock::ALock()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryActorTick.bCanEverTick = true;

	// ...
	bIsLocked = false;

	LockMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Lock"));

	LockMesh->SetupAttachment(GetRootComponent());
}


// Called when the game starts
void ALock::BeginPlay()
{
	Super::BeginPlay();

	// ...

	if(!LockableActor && GetAttachParentActor()) LockableActor = GetAttachParentActor();
	
	bIsLocked ?Lock(Key) : UnLock(Key);
}


// Called every frame
void ALock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ...
}

void ALock::Lock(FName key)
{
	if((Key != key) && (Key != NAME_None)) return;

	bIsLocked = true;

	if (LockSound) UGameplayStatics::PlaySoundAtLocation(this, LockSound, GetActorLocation());

	if (!LockableActor) return;

	if(!UKismetSystemLibrary::DoesImplementInterface(LockableActor, ULockInterface::StaticClass())) return;

	if (GetIsActorLocked() || bIsLocked)
	{
		ILockInterface::Execute_OnLocked(LockableActor);
	}
}

void ALock::UnLock(FName key)
{
	if ((Key != key) && (Key != NAME_None)) return;

	bIsLocked = false;

	if (UnlockSound) UGameplayStatics::PlaySoundAtLocation(this, UnlockSound, GetActorLocation());

	if(!LockableActor) return;

	if (!UKismetSystemLibrary::DoesImplementInterface(LockableActor, ULockInterface::StaticClass())) return;
	
	bool unlocked = !GetIsActorLocked() && !bIsLocked;
	if (unlocked)
	{
		ILockInterface::Execute_OnUnlocked(LockableActor);
	}
}

bool ALock::GetIsActorLocked() const
{
	bool isLocked = false;

	for (ALock* lock : Siblings)
	{
		if(lock->bIsLocked == true) isLocked = true;
	}

	return isLocked ;
}

void ALock::OnInteractWith_Implementation(class AActor* actor)
{
	bIsLocked ? Lock(Key) : UnLock(Key);
}

