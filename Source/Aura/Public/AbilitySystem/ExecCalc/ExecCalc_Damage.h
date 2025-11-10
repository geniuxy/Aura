// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_Damage.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UExecCalc_Damage();

	virtual void Execute_Implementation(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput
	) const override;

private:
	void DetermineDebuff(
		const FGameplayEffectSpec& Spec,
		const FGameplayEffectCustomExecutionParameters& ExecParams,
		const FAggregatorEvaluateParameters& EvalParams
	) const;

	float CalcInitialDamage(
		const FGameplayEffectSpec& Spec,
		const FGameplayEffectCustomExecutionParameters& ExecParams,
		const FAggregatorEvaluateParameters& EvalParams
	) const;

	void PrepareExecutionContext(const FGameplayEffectCustomExecutionParameters& ExecutionParams) const;

	float ApplyArmor(float Damage, float TargetArmor, float SourceArmorPen) const;

	float ApplyCritical(float Damage, float SourceCritChance, float SourceCritDamage,
	                    float TargetCritResist, FGameplayEffectContextHandle ContextHandle) const;

	float ApplyBlock(float Damage, float TargetBlockChance, FGameplayEffectContextHandle ContextHandle) const;

	UPROPERTY()
	mutable UAbilitySystemComponent* SourceASC = nullptr;
	UPROPERTY()
	mutable UAbilitySystemComponent* TargetASC = nullptr;
	UPROPERTY()
	mutable TObjectPtr<AActor> SourceAvatar = nullptr;
	UPROPERTY()
	mutable TObjectPtr<AActor> TargetAvatar = nullptr;
	mutable int32 SourceLevel = 1;
	mutable int32 TargetLevel = 1;
};
