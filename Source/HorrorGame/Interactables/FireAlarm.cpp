// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.


#include "FireAlarm.h"
#include "Interactables/Components/AnimationComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AFireAlarm::AFireAlarm()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AlarmMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Alarm"));
	AlarmMesh->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void AFireAlarm::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFireAlarm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
