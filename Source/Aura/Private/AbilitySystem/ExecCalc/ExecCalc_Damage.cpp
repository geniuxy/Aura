// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

#define CURVE_ArmorPenetration        TEXT("ArmorPenetration")
#define CURVE_EffectiveArmor          TEXT("EffectiveArmor")
#define CURVE_CriticalHitResistance   TEXT("CriticalHitResistance")

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);

	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);

	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);
	}


	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> GetTagsToCaptureDefsMap()
	{
		TagsToCaptureDefs.Add(AuraGameplayTags::Attributes_Resistance_Fire, FireResistanceDef);
		TagsToCaptureDefs.Add(AuraGameplayTags::Attributes_Resistance_Lightning, LightningResistanceDef);
		TagsToCaptureDefs.Add(AuraGameplayTags::Attributes_Resistance_Arcane, ArcaneResistanceDef);
		TagsToCaptureDefs.Add(AuraGameplayTags::Attributes_Resistance_Physical, PhysicalResistanceDef);
		return TagsToCaptureDefs;
	}

private:
	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
};

static const AuraDamageStatics& GetDamageStatics()
{
	static AuraDamageStatics DamageStatics;
	return DamageStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(GetDamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().CriticalHitResistanceDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(GetDamageStatics().PhysicalResistanceDef);
}

namespace DamageCalcUtil
{
	// 统一抓取属性并保证非负
	float GetCapturedMagnitude(const FGameplayEffectCustomExecutionParameters& ExecParams,
	                           const FGameplayEffectAttributeCaptureDefinition& Def,
	                           const FAggregatorEvaluateParameters& Params)
	{
		float Value = 0.f;
		ExecParams.AttemptCalculateCapturedAttributeMagnitude(Def, Params, Value);
		return FMath::Max(Value, 0.f);
	}

	// 根据 Curve 获取系数
	float GetCoefficient(const UObject* WorldContext, int32 Level, FName CurveName)
	{
		const UCharacterClassInfo* Info = UAuraAbilitySystemLibrary::GetCharacterClassInfo(WorldContext);
		const FRealCurve* Curve = Info->DamageCalculationCoefficients->FindCurve(CurveName, FString());
		return Curve ? Curve->Eval(Level) : 0.f;
	}
}

// -----------------------------------------------------------------
// 0) Init Damage
static float CalcInitialDamage(const FGameplayEffectSpec& Spec,
                               const FGameplayEffectCustomExecutionParameters& ExecParams,
                               const FAggregatorEvaluateParameters& EvalParams)
{
	float FinalDamage = 0.f;

	// 1. 拿到 Damage -> Resistance 映射
	const TMap<FGameplayTag, FGameplayTag>& DamageToResistance = UAuraAbilitySystemLibrary::GetDamageToResistanceMap();

	// 2. 所有以 Damage 为根的子 Tag
	const FGameplayTagContainer AllDamageTags =
		UGameplayTagsManager::Get().RequestGameplayTagChildren(AuraGameplayTags::Damage);

	// 3. 抗性 Tag → CaptureDef 的映射
	const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& TagsToCaptureDefs =
		AuraDamageStatics().GetTagsToCaptureDefsMap();

	// 4. 遍历每种伤害类型
	for (const FGameplayTag& DamageTypeTag : AllDamageTags)
	{
		if (const FGameplayTag* ResistanceTag = DamageToResistance.Find(DamageTypeTag))
		{
			check(TagsToCaptureDefs.Contains(*ResistanceTag));

			// 原始伤害
			float RawDamage = Spec.GetSetByCallerMagnitude(DamageTypeTag, false, 0.f);

			// 抗性值
			float Resistance = DamageCalcUtil::GetCapturedMagnitude(ExecParams, TagsToCaptureDefs[*ResistanceTag], EvalParams);

			// 应用抗性
			RawDamage *= (100.f - FMath::Clamp(Resistance, 0.f, 100.f)) / 100.f;

			FinalDamage += RawDamage;
		}
	}

	return FinalDamage;
}

// 1) Armor + ArmorPenetration
static float ApplyArmor(float Damage, float TargetArmor, float SourceArmorPen, const UObject* WorldContext,
                        int32 SourceLevel, int32 TargetLevel)
{
	const float PenCoef = DamageCalcUtil::GetCoefficient(WorldContext, SourceLevel, CURVE_ArmorPenetration);
	const float EffectiveArmor = TargetArmor * (100.f - SourceArmorPen * PenCoef) / 100.f;

	const float ArmorCoef = DamageCalcUtil::GetCoefficient(WorldContext, TargetLevel, CURVE_EffectiveArmor);
	return Damage * (100.f - EffectiveArmor * ArmorCoef) / 100.f;
}

// -----------------------------------------------------------------
// 2) Critical Hit
static float ApplyCritical(float Damage, float SourceCritChance, float SourceCritDamage, float TargetCritResist,
                           const UObject* WorldContext, int32 TargetLevel, FGameplayEffectContextHandle ContextHandle)
{
	const float ResistCoef = DamageCalcUtil::GetCoefficient(WorldContext, TargetLevel, CURVE_CriticalHitResistance);
	const float Chance = SourceCritChance - TargetCritResist * ResistCoef;
	const bool bCrit = FMath::RandRange(1, 100) < Chance;
	UAuraAbilitySystemLibrary::SetIsCriticalHit(ContextHandle, bCrit);
	return bCrit ? 2.f * Damage + SourceCritDamage : Damage;
}

// -----------------------------------------------------------------
// 3) Block
static float ApplyBlock(float Damage, float TargetBlockChance, FGameplayEffectContextHandle ContextHandle)
{
	const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;
	UAuraAbilitySystemLibrary::SetIsBlockedHit(ContextHandle, bBlocked);
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
	float Damage = CalcInitialDamage(Spec, ExecutionParams, Params);

	FGameplayEffectContextHandle ContextHandle = Spec.GetContext();
	float TargetArmor =
		DamageCalcUtil::GetCapturedMagnitude(ExecutionParams, GetDamageStatics().ArmorDef, Params);
	float SourceArmorPen =
		DamageCalcUtil::GetCapturedMagnitude(ExecutionParams, GetDamageStatics().ArmorPenetrationDef, Params);
	float SourceCritC =
		DamageCalcUtil::GetCapturedMagnitude(ExecutionParams, GetDamageStatics().CriticalHitChanceDef, Params);
	float SourceCritD =
		DamageCalcUtil::GetCapturedMagnitude(ExecutionParams, GetDamageStatics().CriticalHitDamageDef, Params);
	float TargetCritRes =
		DamageCalcUtil::GetCapturedMagnitude(ExecutionParams, GetDamageStatics().CriticalHitResistanceDef, Params);
	float TargetBlock =
		DamageCalcUtil::GetCapturedMagnitude(ExecutionParams, GetDamageStatics().BlockChanceDef, Params);

	// 3. 计算
	Damage = ApplyArmor(Damage, TargetArmor, SourceArmorPen, SourceAvatar, SourceCI->GetLevel(), TargetCI->GetLevel());
	Damage = ApplyCritical(Damage, SourceCritC, SourceCritD, TargetCritRes,
	                       SourceAvatar, TargetCI->GetLevel(), ContextHandle);
	Damage = ApplyBlock(Damage, TargetBlock, ContextHandle);

	// 4. 输出
	FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(),
	                                             EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
