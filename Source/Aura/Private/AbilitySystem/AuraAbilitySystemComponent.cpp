// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "DebugHelper.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Interaction/PlayerInterface.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
}

void UAuraAbilitySystemComponent::AddAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
			AbilitySpec.DynamicAbilityTags.AddTag(AuraGameplayTags::Ability_Status_Equipped);
			GiveAbility(AbilitySpec);
		}
	}
	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AddPassiveAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (AbilitySpec.IsActive())
			{
				InvokeReplicatedEvent(
					EAbilityGenericReplicatedEvent::InputPressed,
					AbilitySpec.Handle,
					AbilitySpec.ActivationInfo.GetActivationPredictionKey()
				);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag) && AbilitySpec.IsActive())
		{
			// 只是将Spec.InputPressed = false;一个作用
			AbilitySpecInputReleased(AbilitySpec);
			InvokeReplicatedEvent(
				EAbilityGenericReplicatedEvent::InputReleased,
				AbilitySpec.Handle,
				AbilitySpec.ActivationInfo.GetActivationPredictionKey()
			);
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			// 只是将Spec.InputPressed = true;一个作用
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::CancelAllSkillAbility()
{
	FGameplayTagContainer TagContainer =
		UAuraAbilitySystemLibrary::GetAllGameplayTagWithPrefix(TEXT("Ability.Skill"));
	const TArray<FGameplayTag>& AbilityTags = TagContainer.GetGameplayTagArray();

	for (const FGameplayTag& AbilityTag : AbilityTags)
	{
		FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag);
		if (AbilitySpec && AbilitySpec->IsActive())
		{
			CancelAbilityHandle(AbilitySpec->Handle);
		}
	}
}

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	// 在作用域内对当前对象的“能力列表”加锁，防止并发修改。
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			Debug::Print(FString::Printf(TEXT("Failed to execute delegate in %hs"), __FUNCTION__));
		}
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (const FGameplayTag& Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Ability"))))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (const FGameplayTag& Tag : AbilitySpec.DynamicAbilityTags)
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (const FGameplayTag& StatusTag : AbilitySpec.DynamicAbilityTags)
	{
		if (StatusTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Ability.Status"))))
		{
			return StatusTag;
		}
	}
	return FGameplayTag();
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromInputTag(const FGameplayTag& InputTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			return &AbilitySpec;
		}
	}
	return nullptr;
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(AbilityTag))
			{
				return &AbilitySpec;
			}
		}
	}
	return nullptr;
}

bool UAuraAbilitySystemComponent::IsSlotOfInputTagEmpty(const FGameplayTag& InputTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilityHasEquipped(&AbilitySpec, InputTag))
		{
			return false;
		}
	}
	return true;
}

bool UAuraAbilitySystemComponent::IsPassiveAbility(const FGameplayAbilitySpec& Spec)
{
	const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	const FGameplayTag AbilityTag = GetAbilityTagFromSpec(Spec);
	const FAuraAbilityInfo& Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	const FGameplayTag AbilityType = Info.AbilityType;
	return AbilityType.MatchesTagExact(AuraGameplayTags::Ability_Type_Passive);
}

bool UAuraAbilitySystemComponent::CanAbilityBeLearned(const FGameplayAbilitySpec& InAbilitySpec)
{
	if (InAbilitySpec.Ability->GetAbilityLevel() != 1)
	{
		return false;
	}
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.Ability == InAbilitySpec.Ability &&
			AbilitySpec.DynamicAbilityTags.HasTagExact(AuraGameplayTags::Ability_Status_Eligible))
		{
			return true;
		}
	}
	return false;
}

bool UAuraAbilitySystemComponent::AbilityHasEquipped(const FGameplayAbilitySpec* AbilitySpec,
                                                     const FGameplayTag& InputTag)
{
	return AbilitySpec->DynamicAbilityTags.HasTagExact(InputTag);
}

bool UAuraAbilitySystemComponent::AbilityHasAnyInputTag(const FGameplayAbilitySpec* AbilitySpec)
{
	return AbilitySpec->DynamicAbilityTags.HasTag(FGameplayTag::RequestGameplayTag(FName("InputTag")));
}

void UAuraAbilitySystemComponent::AssignInputTagToAbility(FGameplayAbilitySpec& AbilitySpec,
                                                          FGameplayTag InputTag)
{
	ClearEquippedSpell(&AbilitySpec);
	AbilitySpec.DynamicAbilityTags.AddTag(InputTag);
}

void UAuraAbilitySystemComponent::ClearAbilityOfCurrentEquippedSpell(FGameplayTag InputTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (AbilityHasEquipped(&Spec, InputTag))
		{
			ClearEquippedSpell(&Spec);
		}
	}
}

void UAuraAbilitySystemComponent::ClearEquippedSpell(FGameplayAbilitySpec* AbilitySpec)
{
	const FGameplayTag InputTag = GetInputTagFromSpec(*AbilitySpec);
	AbilitySpec->DynamicAbilityTags.RemoveTag(InputTag);
}

bool UAuraAbilitySystemComponent::GetDescriptionsByAbilityTag(
	const FGameplayTag& AbilityTag,
	FString& OutDescription,
	FString& OutNextLevelDescription)
{
	if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec->Ability))
		{
			if (CanAbilityBeLearned(*AbilitySpec))
			{
				OutDescription = AuraAbility->GetCurrentLevelDescription(AbilitySpec->Level - 1);
				OutNextLevelDescription = AuraAbility->GetNextLevelDescription(AbilitySpec->Level);
				return true;
			}
			OutDescription = AuraAbility->GetCurrentLevelDescription(AbilitySpec->Level);
			OutNextLevelDescription = AuraAbility->GetNextLevelDescription(AbilitySpec->Level + 1);
			return true;
		}
	}
	const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	OutDescription =
		UAuraGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfoForTag(AbilityTag).LevelRequirement);
	OutNextLevelDescription = FString();
	return false;
}

void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
		{
			ServerUpgradeAttribute(AttributeTag);
		}
	}
}

void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);

	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1);
	}
}

void UAuraAbilitySystemComponent::UpdateAbilityStatuses(int32 Level)
{
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	for (const FAuraAbilityInfo& Info : AbilityInfo->AbilityInfos)
	{
		if (!Info.AbilityTag.IsValid()) continue;
		if (Level < Info.LevelRequirement) continue;
		if (GetSpecFromAbilityTag(Info.AbilityTag) == nullptr)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);
			AbilitySpec.DynamicAbilityTags.AddTag(AuraGameplayTags::Ability_Status_Eligible);
			GiveAbility(AbilitySpec);
			MarkAbilitySpecDirty(AbilitySpec);
			ClientUpdateAbilityStatus(Info.AbilityTag, Info.AbilityType, AuraGameplayTags::Ability_Status_Eligible, 1);
		}
	}
}

void UAuraAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag,
                                                                       const FGameplayTag& AbilityType)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (GetAvatarActor()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);
		}

		FGameplayTag StatusTag = GetStatusFromSpec(*AbilitySpec);
		if (StatusTag.MatchesTagExact(AuraGameplayTags::Ability_Status_Eligible))
		{
			AbilitySpec->DynamicAbilityTags.RemoveTag(AuraGameplayTags::Ability_Status_Eligible);
			AbilitySpec->DynamicAbilityTags.AddTag(AuraGameplayTags::Ability_Status_Unlocked);
			StatusTag = AuraGameplayTags::Ability_Status_Unlocked;
		}
		else if (StatusTag.MatchesTagExact(AuraGameplayTags::Ability_Status_Equipped) ||
			StatusTag.MatchesTagExact(AuraGameplayTags::Ability_Status_Unlocked))
		{
			AbilitySpec->Level += 1;
		}
		MarkAbilitySpecDirty(*AbilitySpec);
		ClientUpdateAbilityStatus(AbilityTag, AbilityType, StatusTag, AbilitySpec->Level);
	}
}

void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag,
                                                                    const FGameplayTag& InputTag)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		FGameplayTag PrevInputTag = GetInputTagFromSpec(*AbilitySpec);
		const FGameplayTag& AbilityStatus = GetStatusFromSpec(*AbilitySpec);

		const bool bStatusValid = AbilityStatus == AuraGameplayTags::Ability_Status_Equipped ||
			AbilityStatus == AuraGameplayTags::Ability_Status_Unlocked;
		if (bStatusValid)
		{
			// 如果即将装备的技能槽中已有技能
			if (!IsSlotOfInputTagEmpty(InputTag))
			{
				if (FGameplayAbilitySpec* SpecWithInputTag = GetSpecFromInputTag(InputTag))
				{
					// is that ability the same as this ability? If so, we can return early.
					if (AbilityTag.MatchesTagExact(GetAbilityTagFromSpec(*SpecWithInputTag)))
					{
						ClientEquipAbility(AbilityTag, InputTag, PrevInputTag);
						return;
					}

					// Handle activation/deactivation for passive abilities
					if (IsPassiveAbility(*SpecWithInputTag))
					{
						MulticastActivatePassiveEffect(GetAbilityTagFromSpec(*SpecWithInputTag), false);
						DeactivatePassiveAbilityDelegate.Broadcast(GetAbilityTagFromSpec(*SpecWithInputTag));
					}

					ClearEquippedSpell(SpecWithInputTag);
				}
			}

			if (!AbilityHasAnyInputTag(AbilitySpec)) // Ability doesn't yet have a slot (it's not active)
			{
				if (IsPassiveAbility(*AbilitySpec))
				{
					TryActivateAbility(AbilitySpec->Handle);
					MulticastActivatePassiveEffect(AbilityTag, true);
				}
			}
			AssignInputTagToAbility(*AbilitySpec, InputTag);
			MarkAbilitySpecDirty(*AbilitySpec);
		}
		ClientEquipAbility(AbilityTag, InputTag, PrevInputTag);
	}
}

void UAuraAbilitySystemComponent::ClientEquipAbility_Implementation(FGameplayTag AbilityTag, FGameplayTag InputTag,
                                                                    FGameplayTag PrevInputTag)
{
	AbilityEquippedDelegate.Broadcast(AbilityTag, InputTag, PrevInputTag);
}

void UAuraAbilitySystemComponent::MulticastActivatePassiveEffect_Implementation(const FGameplayTag& AbilityTag,
	bool bActivate)
{
	OnPassiveActivateDelegate.Broadcast(AbilityTag, bActivate);
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilitiesGivenDelegate.Broadcast();
	}
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(
	UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayEffectSpec& GameplayEffectSpec,
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	FGameplayTagContainer AssetTagContainer;
	GameplayEffectSpec.GetAllAssetTags(AssetTagContainer);

	EffectAssetsTagDelegate.Broadcast(AssetTagContainer);
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(
	const FGameplayTag& AbilityTag,
	const FGameplayTag& AbilityType,
	const FGameplayTag& StatusTag,
	int32 AbilityLevel)
{
	AbilityStatusChangedDelegate.Broadcast(AbilityTag, AbilityType, StatusTag, AbilityLevel);
}
