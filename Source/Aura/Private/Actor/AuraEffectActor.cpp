// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "DebugHelper.h"
#include "Kismet/KismetMathLibrary.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));

	SpawnTimeline = CreateDefaultSubobject<UTimelineComponent>("SpawnTimeline");
}

void AAuraEffectActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (SpawnTimeline)
	{
		SpawnTimeline->TickComponent(DeltaSeconds, LEVELTICK_All, nullptr);
	}

	RunningTime += DeltaSeconds;
	const float SinePeriod = 2 * PI / SinePeriodConstant;
	if (RunningTime > SinePeriod)
	{
		RunningTime = 0.f;
	}

	ItemMovement(DeltaSeconds);
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();

	InitialLocation = GetActorLocation();
	CalculatedLocation = InitialLocation;
	CalculatedRotation = GetActorRotation();

	if (SpawnJumpHeightCurve)
	{
		UCurveFloat* PrivateHeightCurve =
			DuplicateObject<UCurveFloat>(SpawnJumpHeightCurve, this, TEXT("PrivateHeightCurve"));
		SpawnJumpHeightUpdate.BindUFunction(this, "OnHeightUpdate");
		SpawnTimeline->AddInterpFloat(PrivateHeightCurve, SpawnJumpHeightUpdate);
	}

	if (SpawnScaleCurve)
	{
		UCurveFloat* PrivateScaleCurve =
			DuplicateObject<UCurveFloat>(SpawnScaleCurve, this, TEXT("PrivateScaleCurve"));
		SpawnScaleUpdate.BindUFunction(this, "OnScaleUpdate");
		SpawnTimeline->AddInterpFloat(PrivateScaleCurve, SpawnScaleUpdate);
	}

	SpawnJumpFinish.BindUFunction(this, "OnSpawnFinished");
	SpawnTimeline->SetTimelineFinishedFunc(SpawnJumpFinish);
	SpawnTimeline->SetLooping(false);
	SpawnTimeline->PlayFromStart();
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	if (!bApplyEffectToEnemies && TargetActor->ActorHasTag(FName("Enemy"))) return;

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC) return;

	check(GameplayEffectClass);
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpec =
		TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);
	const FActiveGameplayEffectHandle ActiveEffectHandle =
		TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());

	bool bIsInfinite = EffectSpec.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if (bIsInfinite && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
	}

	if (bDestroyOnEffectApplication && !bIsInfinite)
	{
		Destroy();
	}
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if (!bApplyEffectToEnemies && TargetActor->ActorHasTag(FName("Enemy"))) return;

	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffect);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffect);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffect);
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (!bApplyEffectToEnemies && TargetActor->ActorHasTag(FName("Enemy"))) return;

	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffect);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffect);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffect);
	}
	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC)) return;

		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		for (TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveHandlePair : ActiveEffectHandles)
		{
			if (TargetASC == ActiveHandlePair.Value)
			{
				TargetASC->RemoveActiveGameplayEffect(ActiveHandlePair.Key, 1);
				HandlesToRemove.Add(ActiveHandlePair.Key);
			}
		}

		for (FActiveGameplayEffectHandle& Handle : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}
	}
}

void AAuraEffectActor::StartSinusoidalMovement()
{
	bSinusoidalMovement = true;
	InitialLocation = GetActorLocation();
	CalculatedLocation = InitialLocation;
}

void AAuraEffectActor::StartRotation()
{
	bRotates = true;
	CalculatedRotation = GetActorRotation();
}

void AAuraEffectActor::OnScaleUpdate(float InScale)
{
	SetActorScale3D(FVector(InScale));
}

void AAuraEffectActor::OnHeightUpdate(float JumpHeight)
{
	CalculatedLocation = FVector(InitialLocation.X, InitialLocation.Y, InitialLocation.Z + JumpHeight * 8.f);
}

void AAuraEffectActor::OnSpawnFinished()
{
	StartSinusoidalMovement();
	StartRotation();
}

void AAuraEffectActor::ItemMovement(float DeltaTime)
{
	if (bRotates)
	{
		const FRotator DeltaRotation(0.f, DeltaTime * RotationRate, 0.f);
		CalculatedRotation = UKismetMathLibrary::ComposeRotators(CalculatedRotation, DeltaRotation);
	}
	if (bSinusoidalMovement)
	{
		const float Sine = SineAmplitude * FMath::Sin(RunningTime * SinePeriodConstant);
		CalculatedLocation = InitialLocation + FVector(0.f, 0.f, Sine);
	}

	FTransform CalculatedTransform;
	CalculatedTransform.SetLocation(CalculatedLocation);
	CalculatedTransform.SetRotation(CalculatedRotation.Quaternion());
	SetActorTransform(CalculatedTransform);
}
