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

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                                    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

private:
	void DetermineDebuff(const FGameplayEffectSpec& Spec,
	                     const FGameplayEffectCustomExecutionParameters& ExecParams,
	                     const FAggregatorEvaluateParameters& EvalParams) const;
	static float CalcInitialDamage(const FGameplayEffectSpec& Spec,
	                               const FGameplayEffectCustomExecutionParameters& ExecParams,
	                               const FAggregatorEvaluateParameters& EvalParams);
};
