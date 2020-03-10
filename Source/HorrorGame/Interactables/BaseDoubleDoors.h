// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseDoubleDoors.generated.h"

/* Base class for setting up double door actors*/
UCLASS()
class CRLTSIM_API ABaseDoubleDoors : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseDoubleDoors();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
		USceneComponent* Root;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
		UStaticMeshComponent* Frame;

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadWrite)
		UChildActorComponent* LeftDoor;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite)
		UChildActorComponent* RightDoor;
};
