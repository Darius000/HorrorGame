// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Curves/CurveFloat.h"
#include "Interactables/Interfaces/InteractInterface.h"
#include "Components/TimelineComponent.h"
#include "Engine/DataAsset.h"
#include "BaseDoor.generated.h"


UENUM(BlueprintType)
enum class EDoorSectionType : uint8 
{
	NONE,
	FRAME,
	HINDGE,
	DOOR,
	HANDLE,
	WINDOW
};

USTRUCT(BlueprintType)
struct FCurveFloat3
{
	GENERATED_BODY()

		FCurveFloat3()
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRuntimeFloatCurve X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRuntimeFloatCurve Y;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRuntimeFloatCurve Z;

	const float GetMaxTime();
};

USTRUCT(BlueprintType)
struct FAnimationCurves
{
	GENERATED_BODY()

	FAnimationCurves() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FCurveFloat3 Translation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FCurveFloat3 Rotation;

	const float GetMaxTime();
};

USTRUCT(BlueprintType)
struct FAnimatedComponent
{
	GENERATED_BODY()

		FAnimatedComponent() {}

	FAnimatedComponent(FAnimationCurves curves, USceneComponent* component)
		:Curves(curves), Component(component)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FAnimationCurves Curves;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* Component;
};

UCLASS(BlueprintType, editinlinenew, Blueprintable)
class UDoorSectionData : public UDataAsset
{
	GENERATED_BODY()

public:
	UDoorSectionData():
		SectionName(EName::NAME_None), ParentSocketName(EName::NAME_None), Type(EDoorSectionType::NONE),
		ComponentClass(UStaticMeshComponent::StaticClass()), Mesh(nullptr), bIsAnimated(false)
	{}

#if WITH_EDITOR

	virtual bool CanEditChange(const UProperty* InProperty) const override;
#endif
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName SectionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName ParentSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector RelativeOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EDoorSectionType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<USceneComponent> ComponentClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsAnimated;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bIsAnimated"))
		FAnimationCurves AnimationCurves;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UDoorSectionData*> Children;

	UFUNCTION()
		void ConstructPart(AActor* owner, USceneComponent* parentComponent, TArray<USceneComponent*>& components, TArray<FAnimatedComponent>& animatedComponents);

	UFUNCTION(BlueprintCallable)
		UDoorSectionData* FindChildDataByName(const FName& name) const;
};


UCLASS(BlueprintType, editinlinenew, Blueprintable)
class UDoorData : public UDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UDoorSectionData*> Sections;

	UFUNCTION(BlueprintCallable)
		void ConstructDoorFromData(AActor* owner, USceneComponent* root , TArray<USceneComponent*>& components, TArray<FAnimatedComponent>& animatedComponents);
};

UENUM(BlueprintType)
enum class EDoorState : uint8
{
	CLOSED,
	OPENED,
	CLOSING,
	OPENING
};

UENUM(BlueprintType)
enum class ELockStatus : uint8
{
	UNLOCKED,
	LOCKED
};


UCLASS()
class CRLTSIM_API ABaseDoor : public AActor//, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	//void OnInteractWith_Implementation(class AActor* actor) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	void RemoveUnusedDoorComponents();

	UFUNCTION(BlueprintCallable)
	void PlayAnimation(float deltaTime, float& currentTime, float speed = 1.0f, bool bReverse = false);

	//bool CanInteractWith_Implementation() const override;


	//void SetCanInteract_Implementation(bool bInteractable) override;


	//bool ShouldCharacterWalkTo_Implementation() const override;


	//FVector GetAnimationStartLocation_Implementation() const override;

//public:
	UFUNCTION(BlueprintCallable, Category = "Door")
	void Open();

	UFUNCTION(BlueprintCallable, Category = "Door")
	void Close();

//public:

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
		//bool bIsInteractable;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
		//bool bIsOpen;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
		UDoorData* Data;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
		EDoorState DoorState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Door")
		float MaxTime;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
		ELockStatus LockStatus;*/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Door")
		TArray<USceneComponent*> SceneComponents;

	UPROPERTY()
		TArray<FAnimatedComponent> AnimatedComponents;

	void SetMaxAnimationTime();
};
