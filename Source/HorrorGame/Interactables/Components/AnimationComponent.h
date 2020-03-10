// Copyright (c) Crisis Response Leader Training, Inc.  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"
#include "AnimationComponent.generated.h"


UENUM(BlueprintType)
enum class EPlayBackMode : uint8
{
	ONCE			UMETA(DisplayName = "Once"),
	LOOP			UMETA(DisplayName = "Loop"),
	REVERSABLE		UMETA(DisplayName = "Reversable")
};

UENUM(BlueprintType)
enum class EPlayBackStart : uint8 {
	PLAY			UMETA(DisplayName = "Play"),
	PLAYFROMSTART	UMETA(DisplayName = "Play From Start"),
	REVERSE			UMETA(DisplayName = "Reverse"),
	REVERSEFROMEND	UMETA(DisplayName = "Reverse From End")
};


USTRUCT(BlueprintType)
struct FRuntimeVectorCurve
{
	GENERATED_BODY()

	FRuntimeVectorCurve() :bIsAdditive(true)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRuntimeFloatCurve X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRuntimeFloatCurve Y;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRuntimeFloatCurve Z;

	/*Does the curve add to the current animation offsets*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsAdditive;

	void GetValues(float Time, FVector& Outputs) const;
	void GetValues(float Time, FRotator& Outputs) const;

	/*Get the the max animation key time for x,y,z curves*/
	const float GetMaxTime() const;
};

USTRUCT(BlueprintType)
struct FRuntimeTransformCurve
{
	GENERATED_BODY()

	FRuntimeTransformCurve(){}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRuntimeVectorCurve Translation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRuntimeVectorCurve Rotation;

	/*Get the the max animation key time for rotation and translation curves*/
	const float GetMaxTime() const;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAnimationFinishedSigniture, int32, TimesPlayed);

/*
	This is a scene component that can be used to animate  components attached to this.
	Use case examples is a door animation, where the door mesh is attached to this component.
	Animating this component will not affect the relative location and rotation of the components attached
*/
UCLASS( ClassGroup=(Animation), meta=(BlueprintSpawnableComponent) )
class CRLTSIM_API UAnimationComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAnimationComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	/*Animation curves to affect this component*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRuntimeTransformCurve RuntimeTransformCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimationComponent|Playback Settings")
		float PlayBackRate = 1.0f;

	/*Should animations play in reverse*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimationComponent|Playback Settings")
		bool bReverse = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimationComponent|Playback Settings")
		bool bPlayOnStart = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimationComponent|Playback Settings")
		float StartTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimationComponent|Playback Settings")
		EPlayBackMode PlayBackMode = EPlayBackMode::LOOP;

	UFUNCTION()
		void InitializeComponent();

	UFUNCTION(BlueprintCallable , Category = "AnimationComponent")
		void Play(EPlayBackStart playbackStart);

	UFUNCTION(BlueprintCallable, Category = "AnimationComponent")
		const float GetAnimationMaxTime() const;

	UFUNCTION(BlueprintCallable, Category = "AnimationComponent")
		void SetAnimationStartTime(float time);

	UFUNCTION(BlueprintCallable, Category = "AnimationComponent")
		void SetAnimationCurvesTime(float time);
	
	UFUNCTION(BlueprintCallable, Category = "AnimationComponent")
		void SetPlaybackPosition(float time);

	UFUNCTION()
		void TimelineCallback(float deltaTime);

	UFUNCTION()
		void TimelineFinishedCallback();

	/*What to do when animation has finished current playback*/
	UPROPERTY(BlueprintAssignable, Category = "AnimationComponent")
		FAnimationFinishedSigniture OnFinished;

private:

	UPROPERTY()
	float MaxAnimationTime;

	UPROPERTY()
		FTimeline Timeline;

	UPROPERTY()
		UCurveFloat* AnimationTimeCurve;

};
