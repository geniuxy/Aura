// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);

	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
	}
};

static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DamageStatics;
	return DamageStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
}

namespace DamageCalcUtil
{
	// 统一抓取属性并保证非负
	static float GetCapturedMagnitude(const FGameplayEffectCustomExecutionParameters& ExecParams,
	                                  const FGameplayEffectAttributeCaptureDefinition& Def,
	                                  const FAggregatorEvaluateParameters& Params)
	{
		float Value = 0.f;
		ExecParams.AttemptCalculateCapturedAttributeMagnitude(Def, Params, Value);
		return FMath::Max(Value, 0.f);
	}

	// 根据 Curve 获取系数
	static float GetCoefficient(const UObject* WorldContext, int32 Level, FName CurveName)
	{
		const UCharacterClassInfo* Info = UAuraAbilitySystemLibrary::GetCharacterClassInfo(WorldContext);
		const FRealCurve* Curve = Info->DamageCalculationCoefficients->FindCurve(CurveName, FString());
		return Curve ? Curve->Eval(Level) : 0.f;
	}
}

// -----------------------------------------------------------------
// 1) Armor + ArmorPenetration
static float ApplyArmor(float Damage,
                        float TargetArmor,
                        float SourceArmorPen,
                        const UObject* WorldContext,
                        int32 SourceLevel,
                        int32 TargetLevel)
{
	const float PenCoef = DamageCalcUtil::GetCoefficient(WorldContext, SourceLevel, TEXT("ArmorPenetration"));
	const float EffectiveArmor = TargetArmor * (100.f - SourceArmorPen * PenCoef) / 100.f;

	const float ArmorCoef = DamageCalcUtil::GetCoefficient(WorldContext, TargetLevel, TEXT("EffectiveArmor"));
	return Damage * (100.f - EffectiveArmor * ArmorCoef) / 100.f;
}

// -----------------------------------------------------------------
// 2) Critical Hit
static float ApplyCritical(float Damage, float SourceCritChance, float SourceCritDamage, float TargetCritResist,
                           const UObject* WorldContext, int32 TargetLevel)
{
	const float ResistCoef = DamageCalcUtil::GetCoefficient(WorldContext, TargetLevel, TEXT("CriticalHitResistance"));
	const float Chance = SourceCritChance - TargetCritResist * ResistCoef;
	const bool bCrit = FMath::RandRange(1, 100) < Chance;
	return bCrit ? 2.f * Damage + SourceCritDamage : Damage;
}

// -----------------------------------------------------------------
// 3) Block
static float ApplyBlock(float Damage, float TargetBlockChance)
{
	const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;
	return bBlocked ? Damage * 0.5f : Damage;
}

// -----------------------------------------------------------------
void UExecCalc_Damage::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// 1. 通用准备
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	ICombatInterface* SourceCI = Cast<ICombatInterface>(SourceAvatar);
	ICombatInterface* TargetCI = Cast<ICombatInterface>(TargetAvatar);

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters Params;
	Params.SourceTags = SourceTags;
	Params.TargetTags = TargetTags;

	// 2. 抓取所有需要的属性
	float Damage = Spec.GetSetByCallerMagnitude(AuraGameplayTags::Damage);
	float TargetArmor = DamageCalcUtil::GetCapturedMagnitude(ExecutionParams, DamageStatics().ArmorDef, Params);
	float SourcePen =
		DamageCalcUtil::GetCapturedMagnitude(ExecutionParams, DamageStatics().ArmorPenetrationDef, Params);
	float SourceCritC =
		DamageCalcUtil::GetCapturedMagnitude(ExecutionParams, DamageStatics().CriticalHitChanceDef, Params);
	float SourceCritD =
		DamageCalcUtil::GetCapturedMagnitude(ExecutionParams, DamageStatics().CriticalHitDamageDef, Params);
	float TargetCritRes =
		DamageCalcUtil::GetCapturedMagnitude(ExecutionParams, DamageStatics().CriticalHitResistanceDef, Params);
	float TargetBlock = DamageCalcUtil::GetCapturedMagnitude(ExecutionParams, DamageStatics().BlockChanceDef, Params);

	// 3. 计算
	Damage = ApplyArmor(Damage, TargetArmor, SourcePen, SourceAvatar, SourceCI->GetLevel(), TargetCI->GetLevel());
	Damage = ApplyCritical(Damage, SourceCritC, SourceCritD, TargetCritRes, SourceAvatar, TargetCI->GetLevel());
	Damage = ApplyBlock(Damage, TargetBlock);

	// 4. 输出
	FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(),
	                                             EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
