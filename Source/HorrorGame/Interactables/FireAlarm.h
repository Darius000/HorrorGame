// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseInteractable.h"
#include "FireAlarm.generated.h"


UCLASS()
class CRLTSIM_API AFireAlarm : public ABaseInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFireAlarm();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "FireAlarm")
		class UStaticMeshComponent* AlarmMesh;

	UStaticMeshComponent* GetHighlightedMesh_Implementation() const override { return AlarmMesh; };

};
