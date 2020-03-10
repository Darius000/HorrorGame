// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactables/Interfaces/LockInterface.h"
#include "BaseInteractable.h"
#include "Lock.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUnlockedSigniture);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLockedSigniture);

/*
Lock actor that locks another actor if it implements the Lock Interface
Lockable Actor can be null and set when attached to another scene actor
*/
UCLASS( )
class CRLTSIM_API ALock : public ABaseInteractable
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ALock();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void Lock(FName key);

	UFUNCTION(BlueprintCallable)
		void UnLock(FName key);

	/*The actor that implements the ILockInterface this lock controls
	Note: can be null and set when attached to another scene actor
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AActor* LockableActor;

	/*Setting this name on multiple locks: sets up a sibling lock connection*/
	UPROPERTY(EditAnywhere , BlueprintReadWrite)
		TArray<ALock*> Siblings;

	/* the name of the key that can control this component, ex. Name or PassCode*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsLocked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USoundWave* LockSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USoundWave* UnlockSound;

	UFUNCTION(BlueprintCallable)
		bool GetIsActorLocked() const;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
		UStaticMeshComponent* LockMesh;

public:
	void OnInteractWith_Implementation(class AActor* actor) override;

	UStaticMeshComponent* GetHighlightedMesh_Implementation() const override { return LockMesh; };

};
