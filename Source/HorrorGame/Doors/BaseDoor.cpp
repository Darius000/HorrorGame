// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#include "CrltSim.h"
#include "BaseDoor.h"


#if WITH_EDITOR

bool UDoorSectionData::CanEditChange(const UProperty* InProperty) const
{
	const bool ParentVal = Super::CanEditChange(InProperty);

	FName PropertyName = (InProperty != nullptr) ? InProperty->GetFName() : NAME_None;
	
	if (PropertyName == GET_MEMBER_NAME_CHECKED(UDoorSectionData, Mesh))
	{
		return Type != EDoorSectionType::NONE;
	}

	return ParentVal;
}

#endif

void UDoorSectionData::ConstructPart(AActor* owner, USceneComponent* parentComponent, TArray<USceneComponent*>& components, 
	TArray<FAnimatedComponent>& animatedComponents)
{
	USceneComponent* component = NewObject<USceneComponent>(owner, ComponentClass, SectionName);
	component->AttachToComponent(parentComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, ParentSocketName);
	component->SetRelativeLocation(RelativeOffset);
	component->RegisterComponent();
	component->CreationMethod = EComponentCreationMethod::UserConstructionScript;
	if (ComponentClass == UStaticMeshComponent::StaticClass()) Cast<UStaticMeshComponent>(component)->SetStaticMesh(Mesh);
	if (Type != EDoorSectionType::NONE || Type != EDoorSectionType::FRAME)  component->SetMobility(EComponentMobility::Movable);
	components.Add(component);

	if (bIsAnimated)
	{
		FAnimatedComponent animatedComponent = FAnimatedComponent(AnimationCurves, component );

		animatedComponents.Add(animatedComponent);
	}

	UE_LOG(LogTemp, Log, TEXT("%s constructed"), *SectionName.ToString());

	for (auto child : Children)
	{
		child->ConstructPart(owner, component, components, animatedComponents);
	}
}

UDoorSectionData* UDoorSectionData::FindChildDataByName(const FName& name) const
{
	for (auto child : Children)
	{
		if (name == child->SectionName)
		{
			return child;
		}
	}

	return nullptr;
}

ABaseDoor::ABaseDoor()
{
	Root = CreateDefaultSubobject<USceneComponent>(FName("Root"));

	SetRootComponent(Root);

	DoorState = EDoorState::CLOSED;
}

void ABaseDoor::BeginPlay()
{
	Super::BeginPlay();

	SetMaxAnimationTime();
}

void ABaseDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	static float currentTime = 0.0f;

	if (DoorState == EDoorState::OPENING)
	{
		PlayAnimation(DeltaTime, currentTime);
	}
	else if (DoorState == EDoorState::CLOSING)
	{
		PlayAnimation(DeltaTime, currentTime, 1.0f, true);
	}
}

void ABaseDoor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	RemoveUnusedDoorComponents();
}

void ABaseDoor::RemoveUnusedDoorComponents()
{
	SceneComponents.Empty();

	AnimatedComponents.Empty();

	if(Data) Data->ConstructDoorFromData(this, Root, SceneComponents, AnimatedComponents);
}

void ABaseDoor::PlayAnimation(float deltaTime, float& currentTime, float speed, bool bReverse)
{
	currentTime += bReverse ? speed * -deltaTime : speed * deltaTime;

	//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Purple, FString::Printf(TEXT("%f"), currentTime));

	for (FAnimatedComponent animatedComponent : AnimatedComponents)
	{
		FCurveFloat3* TCurve = &animatedComponent.Curves.Translation;
		FCurveFloat3* RCurve = &animatedComponent.Curves.Rotation;

		float x = TCurve->X.GetRichCurveConst()->Eval(currentTime);
		float y = TCurve->Y.GetRichCurveConst()->Eval(currentTime);
		float z = TCurve->Z.GetRichCurveConst()->Eval(currentTime);

		float roll	= RCurve->X.GetRichCurveConst()->Eval(currentTime);
		float yaw	= RCurve->Y.GetRichCurveConst()->Eval(currentTime);
		float pitch = RCurve->Z.GetRichCurveConst()->Eval(currentTime);

		FVector tranlation = FVector(x ,y , z);
		FVector rotation = FVector(roll, yaw, pitch);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Purple, FString::Printf(TEXT("%s ; T : %s, R : %s"),
				*animatedComponent.Component->GetFName().ToString(), *tranlation.ToString(), *rotation.ToString()));
		}

		animatedComponent.Component->SetRelativeLocationAndRotation(tranlation, FQuat::MakeFromEuler(rotation));
	}

	if (currentTime >= MaxTime)
	{
		currentTime = MaxTime;
		DoorState = EDoorState::OPENED;
	}

	if (currentTime <= 0.0f)
	{
		currentTime = 0.0f;
		DoorState = EDoorState::CLOSED;
	}
}

void ABaseDoor::Open()
{
	DoorState = EDoorState::OPENING;
}

void ABaseDoor::Close()
{
	DoorState = EDoorState::CLOSING;
}

void ABaseDoor::SetMaxAnimationTime()
{
	MaxTime = 0.0f;

	for (int32 i = 0; i < AnimatedComponents.Num(); i++)
	{
		MaxTime = FMath::Max(AnimatedComponents[i].Curves.GetMaxTime(), MaxTime);
	}
}

void UDoorData::ConstructDoorFromData(AActor* owner, USceneComponent* root, TArray<USceneComponent*>& components, TArray<FAnimatedComponent>& animatedComponents)
{
	for (auto section : Sections)
	{
		section->ConstructPart(owner, root ,components, animatedComponents);
	}
}

const float FAnimationCurves::GetMaxTime()
{
	return FMath::Max(Translation.GetMaxTime(), Rotation.GetMaxTime());
}

const float FCurveFloat3::GetMaxTime()
{

	float xTime = X.GetRichCurveConst()->Keys.Num() > 0 ? X.GetRichCurveConst()->GetLastKey().Time : 0.0f;
	float yTime = Y.GetRichCurveConst()->Keys.Num() > 0 ? Y.GetRichCurveConst()->GetLastKey().Time : 0.0f;
	float zTime = Z.GetRichCurveConst()->Keys.Num() > 0 ? Z.GetRichCurveConst()->GetLastKey().Time : 0.0f;

	return FMath::Max3(xTime, yTime, zTime);
}
