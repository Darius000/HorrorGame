// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#include "CrltSim.h"
#include "AnimationComponent.h"

void FRuntimeVectorCurve::GetValues(float Time, FVector& Outputs) const
{
	float x = X.GetRichCurveConst()->Eval(Time);
	float y = Y.GetRichCurveConst()->Eval(Time);
	float z = Z.GetRichCurveConst()->Eval(Time);
	Outputs = FVector(x, y, z);
}

void FRuntimeVectorCurve::GetValues(float Time, FRotator& Outputs) const
{
	float x = X.GetRichCurveConst()->Eval(Time);
	float y = Y.GetRichCurveConst()->Eval(Time);
	float z = Z.GetRichCurveConst()->Eval(Time);
	Outputs = FRotator(y, z, x);
}

const float FRuntimeVectorCurve::GetMaxTime() const
{
	float maxXTime = (X.GetRichCurveConst() && X.GetRichCurveConst()->Keys.Num() > 0) ? X.GetRichCurveConst()->GetLastKey().Time : 0.0f;
	float maxYTime = (Y.GetRichCurveConst() && Y.GetRichCurveConst()->Keys.Num() > 0) ? Y.GetRichCurveConst()->GetLastKey().Time : 0.0f;
	float maxZTime = (Z.GetRichCurveConst() && Z.GetRichCurveConst()->Keys.Num() > 0) ? Z.GetRichCurveConst()->GetLastKey().Time : 0.0f;

	return FMath::Max3(maxXTime, maxYTime, maxZTime);
}

const float FRuntimeTransformCurve::GetMaxTime() const
{
	return FMath::Max(Translation.GetMaxTime(), Rotation.GetMaxTime());
}



// Sets default values for this component's properties
UAnimationComponent::UAnimationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UAnimationComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	InitializeComponent();

	if (bPlayOnStart)
	{
		if (bReverse)
		{
			Timeline.Reverse();
		}
		else
		{
			Timeline.Play();
		}
	}
 }

// Called every frame
void UAnimationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (Timeline.IsPlaying())
	{
		Timeline.TickTimeline(DeltaTime);

		//UE_LOG(LogTemp, Display, TEXT("IsPlaying, position = %f, time = %f"), Timeline.GetPlaybackPosition(), DeltaTime);
	}
}

void UAnimationComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UAnimationComponent, RuntimeTransformCurve))
	{
		MaxAnimationTime = RuntimeTransformCurve.GetMaxTime();

		//UE_LOG(LogTemp, Display, TEXT("New Max time is %f"), MaxAnimationTime);
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UAnimationComponent::InitializeComponent()
{
	FOnTimelineEventStatic OnTimeLineFinishedCallback;
	OnTimeLineFinishedCallback.BindUFunction(this, FName("TimelineFinishedCallback"));

	FOnTimelineFloatStatic OnTimelineCallback;
	OnTimelineCallback.BindUFunction(this, FName("TimelineCallback"));

	bool LoopPlayback = PlayBackMode == EPlayBackMode::LOOP;
	Timeline.SetLooping(LoopPlayback);
	Timeline.SetPlayRate(PlayBackRate);
	Timeline.SetTimelineLength(GetAnimationMaxTime());
	Timeline.SetTimelineFinishedFunc(OnTimeLineFinishedCallback);
	AnimationTimeCurve = NewObject<UCurveFloat>(this, TEXT("AnimationTimeCurve"));
	AnimationTimeCurve->FloatCurve.AddKey(0.0f, 0.0f);
	AnimationTimeCurve->FloatCurve.AddKey(GetAnimationMaxTime(), GetAnimationMaxTime());
	Timeline.AddInterpFloat(AnimationTimeCurve, OnTimelineCallback);
}

void UAnimationComponent::Play(EPlayBackStart playbackStart)
{
	switch (playbackStart)
	{
	case EPlayBackStart::PLAY:
		Timeline.Play();
		break;
	case EPlayBackStart::PLAYFROMSTART:
		Timeline.PlayFromStart();
		break;
	case EPlayBackStart::REVERSE:
		Timeline.Reverse();
		break;
	case EPlayBackStart::REVERSEFROMEND:
		Timeline.ReverseFromEnd();
		break;
	default:
		break;
	}
	
}

void UAnimationComponent::SetPlaybackPosition(float time)
{
	Timeline.SetPlaybackPosition(time, true);
}

const float UAnimationComponent::GetAnimationMaxTime() const
{
	return RuntimeTransformCurve.GetMaxTime();
}

void UAnimationComponent::SetAnimationStartTime(float time)
{
	StartTime = time;

	Timeline.SetNewTime(StartTime);
}

void UAnimationComponent::SetAnimationCurvesTime(float time)
{
	FVector translation = FVector(0.0f);
	FRotator rotation = FRotator(0.0f);

	RuntimeTransformCurve.Translation.GetValues(time, translation);
	FVector newLocation = RuntimeTransformCurve.Translation.bIsAdditive ?  GetRelativeLocation() + translation : translation;
	SetRelativeLocation(newLocation);
	
	RuntimeTransformCurve.Rotation.GetValues(time, rotation);
	FRotator newRotation = RuntimeTransformCurve.Rotation.bIsAdditive ? GetRelativeRotation() + rotation : rotation;
	SetRelativeRotation(newRotation);
}

void UAnimationComponent::TimelineCallback(float deltaTime)
{
	SetAnimationCurvesTime(deltaTime);

	//UE_LOG(LogTemp, Display, TEXT("Playing, %f"), deltaTime);
}

void UAnimationComponent::TimelineFinishedCallback()
{
	static int32 timesPlayed = 0;

	if (PlayBackMode == EPlayBackMode::ONCE)
	{
		Timeline.Stop();
	}
	else if (PlayBackMode == EPlayBackMode::REVERSABLE)
	{
		Timeline.Reverse();
	}

	timesPlayed ++;

	OnFinished.Broadcast(timesPlayed);
}
