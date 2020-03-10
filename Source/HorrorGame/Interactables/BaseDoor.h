// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Curves/CurveFloat.h"
#include "BaseInteractable.h"
#include "Interactables/Interfaces/LockInterface.h"
#include "Interactables/Components/AnimationComponent.h"
#include "Engine/EngineTypes.h"
#include "BaseDoor.generated.h"


UENUM(BlueprintType)
enum class EDoorState : uint8
{
	CLOSED	UMETA(DisplayName = "Closed"),
	OPENED	UMETA(DisplayName = "Opened"),
	CLOSING	UMETA(DisplayName = "Closing"),
	OPENING	UMETA(DisplayName = "Opening")
};

UENUM(BlueprintType)
enum class EDoorType : uint8
{
	DEFAULT		UMETA(DisplayName = "Default"),
	AUTOMATIC	UMETA(DisplayName = "Automatic")
};

/*
Base class for door assets, has an animation component to allow door animations,
Note:this actor class should only have a door mesh applied and another component added to the root to 
allow for more animations that aren't the door
*/
UCLASS()
class CRLTSIM_API ABaseDoor : public ABaseInteractable, public ILockInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Spawns the nav link proxy, which can allow navigation "through" the door
	void SpawnNavLinkProxy(UWorld * World);

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UStaticMeshComponent* GetHighlightedMesh_Implementation() const override { return DoorMesh; };

protected:

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
		EDoorType DoorType;

	/*Array of actors that door will automatically open for, door doesn't have to be automatic*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Automatic")
		TArray<TSubclassOf<AActor>> AutomaticFor;

	/*Time it takes for the door to close*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Automatic ")
		float AutomaticDoorTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
		EDoorState DoorState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
		USoundWave* OpenSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
		USoundWave* CloseSound;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Door")
		UAnimationComponent* AnimationComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Door")
		UStaticMeshComponent* DoorMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Door")
		UBoxComponent* ActorDetection;

	UFUNCTION(BlueprintCallable, Category = "Door")
		void Open();

	UFUNCTION(BlueprintCallable, Category = "Door")
		void Close();

	UFUNCTION(BlueprintCallable, Category = "Door")
		virtual void OnAnimationFinished(int32 TimesPlayed);

#if WITH_EDITOR
	/*Overridden from UObject*/
	virtual bool CanEditChange( const UProperty* InProperty ) const override;
#endif
	/*Overridden from interaction interface*/
	virtual void OnInteractWith_Implementation(class AActor* actor) override;

	void OnLocked_Implementation() override;

	void OnUnlocked_Implementation() override;

protected:
	UFUNCTION(Category = "Door|Automatic")
		virtual void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(Category = "Door|Automatic")
		virtual void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, Category = "Door|Automatic ")
		void AutoCloseDoor();

	FTimerHandle CloseDoorTimerHandle;

	// Sometimes the doors are so large, that even when open, they block the navmesh from being able to generate a path
	// through the doors.  When the below is true, we'll disable collision when the door is in a fully opened state
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Door")
	bool bDisableCollisionWhenOpen = true;

	// Should the nav link proxy be spawned??
	// NOTE: we are not going to detect value change!  If a door COULD ever have a nav-link
	// proxy on it, we should just incur that expense initially vs. really negatively affecting
	// the navmesh generation at runtime... MEANING: this only matters at begin play!
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Nav Proxy")
	bool bShouldSpawnNavProxy = true;

	// Start of nav-link proxy smartlink
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Nav Proxy")
	FVector NavProxyOffsetStart = FVector(150.f, 0.f, 0.f);

	// End of the nav-link proxy smartlink
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Nav Proxy")
	FVector NavProxyOffsetEnd = FVector(-150.f, 0.f, 0.f);

	// Custom nav-link proxy with a bit of intelligence & functionality exposed to be able
	// to configure the links at runtime.  Note that this actor is attached at runtime to 
	// the door actor.  As such, it's important to get the NavProxyOffset Start/End correct
	// to account for origin offsets.  It's likely you'll want either a +/- 50 offset in Y,
	// but since left/right doors are mirrored, it will likely never become automatic...
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Transient, Category = "Nav Proxy")
	class ATurnBasedNavProxy* NavLinkProxy;

	// Status of door collision upon begin-play, used to reset collision as needed when opening/closing
	ECollisionEnabled::Type DefaultDoorCollision;
};
