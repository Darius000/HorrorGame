// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#include "CrltSim.h"
#include "BaseDoubleDoors.h"

// Sets default values
ABaseDoubleDoors::ABaseDoubleDoors()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	SetRootComponent(Root);

	Frame = CreateDefaultSubobject<UStaticMeshComponent>(FName("Frame"));
	Frame->SetupAttachment(Root);
	Frame->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	LeftDoor = CreateDefaultSubobject<UChildActorComponent>(FName("Door(Left)"));
	LeftDoor->SetupAttachment(Root);
	LeftDoor->SetRelativeLocation(FVector(0.0f, 32.0f, 0.0f));

	RightDoor = CreateDefaultSubobject<UChildActorComponent>(FName("Door(Right)"));
	RightDoor->SetupAttachment(Root);
	RightDoor->SetRelativeLocation(FVector(0.0f, -32.0f, 0.0f));
}

// Called when the game starts or when spawned
void ABaseDoubleDoors::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseDoubleDoors::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

