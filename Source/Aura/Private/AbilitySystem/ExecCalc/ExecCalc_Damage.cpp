// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "DebugHelper.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"

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

void UExecCalc_Damage::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// 1. 通用准备
	PrepareExecutionContext(ExecutionParams);

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle ContextHandle = Spec.GetContext();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters Params;
	Params.SourceTags = SourceTags;
	Params.TargetTags = TargetTags;

	// 2. 计算debuff, 考虑抗性
	DetermineDebuff(Spec, ExecutionParams, Params);

	// 2. 初步计算伤害, 考虑抗性
	float Damage = CalcInitialDamage(Spec, ExecutionParams, Params);

	// 3. 抓取所有需要的属性
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

	// 4. 进一步计算伤害
	Damage = ApplyArmor(Damage, TargetArmor, SourceArmorPen);
	Damage = ApplyCritical(Damage, SourceCritC, SourceCritD, TargetCritRes, ContextHandle);
	Damage = ApplyBlock(Damage, TargetBlock, ContextHandle);

	// 5. 输出
	FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(),
	                                             EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}

void UExecCalc_Damage::DetermineDebuff(const FGameplayEffectSpec& Spec,
                                       const FGameplayEffectCustomExecutionParameters& ExecParams,
                                       const FAggregatorEvaluateParameters& EvalParams) const
{
	// 1. 拿到 Damage -> Debuff 映射 和 Damage -> Resistance 映射
	const TMap<FGameplayTag, FGameplayTag>& DamageToDebuff = UAuraAbilitySystemLibrary::GetDamageToDebuffMap();
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
		const FGameplayTag* DebuffTag = DamageToDebuff.Find(DamageTypeTag);
		const FGameplayTag* ResistanceTag = DamageToResistance.Find(DamageTypeTag);
		// 原始伤害
		float RawDamage = Spec.GetSetByCallerMagnitude(DamageTypeTag, false, 0.f);
		if (DebuffTag && ResistanceTag && RawDamage > 0.f)
		{
			// Determine if there was a successful debuff
			const float SourceDebuffChance = Spec.GetSetByCallerMagnitude(AuraGameplayTags::Debuff_Chance, false, -1.f);

			// 抗性值
			float Resistance =
				DamageCalcUtil::GetCapturedMagnitude(ExecParams, TagsToCaptureDefs[*ResistanceTag], EvalParams);
			const float EffectiveDebuffChance = SourceDebuffChance * (100 - Resistance) / 100.f;
			const bool bDebuff = FMath::RandRange(1, 100) < EffectiveDebuffChance;
			if (bDebuff)
			{
				FGameplayEffectContextHandle ContextHandle = Spec.GetContext();

				UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(ContextHandle, true);
				UAuraAbilitySystemLibrary::SetDamageType(ContextHandle, DamageTypeTag);

				const float DebuffDamage =
					Spec.GetSetByCallerMagnitude(AuraGameplayTags::Debuff_Damage, false, -1.f);
				const float DebuffDuration =
					Spec.GetSetByCallerMagnitude(AuraGameplayTags::Debuff_Duration, false, -1.f);
				const float DebuffFrequency =
					Spec.GetSetByCallerMagnitude(AuraGameplayTags::Debuff_Frequency, false, -1.f);

				UAuraAbilitySystemLibrary::SetDebuffDamage(ContextHandle, DebuffDamage);
				UAuraAbilitySystemLibrary::SetDebuffDuration(ContextHandle, DebuffDuration);
				UAuraAbilitySystemLibrary::SetDebuffFrequency(ContextHandle, DebuffFrequency);
			}
		}
	}
}

float UExecCalc_Damage::CalcInitialDamage(const FGameplayEffectSpec& Spec,
                                          const FGameplayEffectCustomExecutionParameters& ExecParams,
                                          const FAggregatorEvaluateParameters& EvalParams) const
{
	float FinalDamage = 0.f;
	FGameplayEffectContextHandle ContextHandle = Spec.GetContext();

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
			if (RawDamage <= 0.f)
			{
				continue;
			}

			// 抗性值
			float Resistance =
				DamageCalcUtil::GetCapturedMagnitude(ExecParams, TagsToCaptureDefs[*ResistanceTag], EvalParams);

			// 应用抗性
			RawDamage *= (100.f - FMath::Clamp(Resistance, 0.f, 100.f)) / 100.f;

			// 范围伤害技能计算伤害
			if (UAuraAbilitySystemLibrary::IsRadialDamage(ContextHandle))
			{
				// 1. override TakeDamage in AuraCharacterBase. *
				// 2. create delegate OnDamageDelegate, broadcast damage received in TakeDamage *
				// 3. Bind lambda to OnDamageDelegate on the Victim here. *
				// 4. Call UGameplayStatics::ApplyRadialDamageWithFalloff to cause damage (this will result in TakeDamage being called
				//		on the Victim, which will then broadcast OnDamageDelegate)
				// 5. In Lambda, set DamageTypeValue to the damage received from the broadcast *

				if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(TargetAvatar))
				{
					CombatInterface->GetOnDamageSignature()->AddLambda([&](float DamageAmount)
					{
						RawDamage = DamageAmount;
					});
				}
				UGameplayStatics::ApplyRadialDamageWithFalloff(
					TargetAvatar,
					RawDamage,
					0.f,
					UAuraAbilitySystemLibrary::GetRadialDamageOrigin(ContextHandle),
					UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(ContextHandle),
					UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(ContextHandle),
					1.f,
					UDamageType::StaticClass(),
					TArray<AActor*>(),
					SourceAvatar,
					nullptr);
			}

			FinalDamage += RawDamage;
		}
	}

	return FinalDamage;
}

void UExecCalc_Damage::PrepareExecutionContext(const FGameplayEffectCustomExecutionParameters& ExecutionParams) const
{
	SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	SourceLevel = (SourceAvatar && SourceAvatar->Implements<UCombatInterface>())
		              ? ICombatInterface::Execute_GetLevel(SourceAvatar)
		              : 1;

	TargetLevel = (TargetAvatar && TargetAvatar->Implements<UCombatInterface>())
		              ? ICombatInterface::Execute_GetLevel(TargetAvatar)
		              : 1;
}

// -----------------------------------------------------------------
// 1) Armor + ArmorPenetration
float UExecCalc_Damage::ApplyArmor(float Damage, float TargetArmor, float SourceArmorPen) const
{
	const float PenCoef = DamageCalcUtil::GetCoefficient(SourceAvatar, SourceLevel, CURVE_ArmorPenetration);
	const float EffectiveArmor = TargetArmor * (100.f - SourceArmorPen * PenCoef) / 100.f;

	const float ArmorCoef = DamageCalcUtil::GetCoefficient(SourceAvatar, TargetLevel, CURVE_EffectiveArmor);
	return Damage * (100.f - EffectiveArmor * ArmorCoef) / 100.f;
}

// -----------------------------------------------------------------
// 2) Critical Hit
float UExecCalc_Damage::ApplyCritical(
	float Damage,
	float SourceCritChance,
	float SourceCritDamage,
	float TargetCritResist,
	FGameplayEffectContextHandle ContextHandle) const
{
	const float ResistCoef = DamageCalcUtil::GetCoefficient(SourceAvatar, TargetLevel, CURVE_CriticalHitResistance);
	const float Chance = SourceCritChance - TargetCritResist * ResistCoef;
	const bool bCrit = FMath::RandRange(1, 100) < Chance;
	UAuraAbilitySystemLibrary::SetIsCriticalHit(ContextHandle, bCrit);
	return bCrit ? 2.f * Damage + SourceCritDamage : Damage;
}

// -----------------------------------------------------------------
// 3) Block
float UExecCalc_Damage::ApplyBlock(
	float Damage,
	float TargetBlockChance,
	FGameplayEffectContextHandle ContextHandle) const
{
	const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;
	UAuraAbilitySystemLibrary::SetIsBlockedHit(ContextHandle, bBlocked);
	return bBlocked ? Damage * 0.5f : Damage;
}
