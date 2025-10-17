#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "DebugHelper.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();

	SpellPointsChangeDelegate.Broadcast(GetAuraPS()->GetPlayerSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraASC()->AbilityEquippedDelegate.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);

	GetAuraASC()->AbilityStatusChangedDelegate.AddLambda(
		[this](const FGameplayTag& AbilityTag, const FGameplayTag& AbilityType, const FGameplayTag& StatusTag,
		       int32 AbilityLevel)
		{
			if (CurrentSelectedSpell.AbilityTag.MatchesTagExact(AbilityTag))
			{
				CurrentSelectedSpell.StatusTag = StatusTag;
				CurrentSelectedSpell.AbilityType = AbilityType;
				UpdateSelectedSpellTreeUI(CurrentSpellPoints);
			}

			if (AbilityInfo)
			{
				FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
				Info.StatusTag = StatusTag;
				AbilityInfoDelegate.Broadcast(Info);
			}
		}
	);

	GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda(
		[this](int32 SpellPoints)
		{
			CurrentSpellPoints = SpellPoints;
			SpellPointsChangeDelegate.Broadcast(SpellPoints);

			UpdateSelectedSpellTreeUI(CurrentSpellPoints);
		}
	);
}

void USpellMenuWidgetController::SelectSpellGlobe(UAuraUserWidget* SpellGlobe)
{
	SpellGlobeSelectedDelegate.Broadcast(SpellGlobe);
}

void USpellMenuWidgetController::SelectEquippedSpellGlobe(UAuraUserWidget* EquippedSpellGlobe)
{
	EquippedSpellGlobeSelectedDelegate.Broadcast(EquippedSpellGlobe);
}

void USpellMenuWidgetController::OnSpellGlobeSelected(const FGameplayTag& AbilityTag, const FGameplayTag& AbilityType)
{
	const int32 SpellPoints = GetAuraPS()->GetPlayerSpellPoints();
	FGameplayTag SpellStatus;

	const bool bTagValid = AbilityTag.IsValid();
	const bool bTagNone = AbilityTag.MatchesTag(AuraGameplayTags::Ability_None);
	const FGameplayAbilitySpec* AbilitySpec = GetAuraASC()->GetSpecFromAbilityTag(AbilityTag);
	const bool bSpecValid = AbilitySpec != nullptr;

	if (!bTagValid || bTagNone || !bSpecValid)
	{
		SpellStatus = AuraGameplayTags::Ability_Status_Locked;
	}
	else
	{
		SpellStatus = GetAuraASC()->GetStatusFromSpec(*AbilitySpec);
	}

	CurrentSelectedSpell.AbilityTag = AbilityTag;
	CurrentSelectedSpell.AbilityType = AbilityType;
	CurrentSelectedSpell.StatusTag = SpellStatus;

	UpdateSelectedSpellTreeUI(SpellPoints);
}

void USpellMenuWidgetController::SpendSpellPointsButtonPressed()
{
	if (GetAuraASC())
	{
		GetAuraASC()->ServerSpendSpellPoint(CurrentSelectedSpell.AbilityTag, CurrentSelectedSpell.AbilityType);
	}
}

void USpellMenuWidgetController::EquippedButtonPressed()
{
	if (CurrentSelectedEquippedSpell.AbilityType == AuraGameplayTags::Ability_Type_None ||
		CurrentSelectedEquippedSpell.InputTag == AuraGameplayTags::InputTag_None)
	{
		return;
	}

	if (CurrentSelectedSpell.AbilityTag == AuraGameplayTags::Ability_None ||
		CurrentSelectedSpell.StatusTag == AuraGameplayTags::Ability_Status_Eligible ||
		CurrentSelectedSpell.StatusTag == AuraGameplayTags::Ability_Status_Locked)
	{
		return;
	}

	if (CurrentSelectedSpell.AbilityType != CurrentSelectedEquippedSpell.AbilityType)
	{
		return;
	}

	GetAuraASC()->ServerEquipAbility(CurrentSelectedSpell.AbilityTag, CurrentSelectedEquippedSpell.InputTag);
}

void USpellMenuWidgetController::SetCurrentSelectedEquippedSpell(const FGameplayTag& InAbilityType,
                                                                 const FGameplayTag& InInputTag)
{
	CurrentSelectedEquippedSpell.AbilityType = InAbilityType;
	CurrentSelectedEquippedSpell.InputTag = InInputTag;
}

void USpellMenuWidgetController::GlobeDeselect()
{
	CurrentSelectedSpell.AbilityTag = AuraGameplayTags::Ability_None;
	CurrentSelectedSpell.AbilityType = AuraGameplayTags::Ability_Type_None;
	CurrentSelectedSpell.StatusTag = AuraGameplayTags::Ability_Status_Locked;

	OnSpellGlobeSelectedDelegate.Broadcast(false, false, FString(), FString());
}

void USpellMenuWidgetController::EquippedGlobeDeselect()
{
	CurrentSelectedEquippedSpell.AbilityType = AuraGameplayTags::Ability_Type_None;
	CurrentSelectedEquippedSpell.InputTag = AuraGameplayTags::InputTag_None;
}

void USpellMenuWidgetController::UpdateSelectedSpellTreeUI(const int32 SpellPoints)
{
	if (CurrentSelectedSpell.AbilityTag == AuraGameplayTags::Ability_None) return;
	bool bEnableSpendPoints = false;
	bool bEnableEquip = false;
	ShouldEnableButtons(CurrentSelectedSpell.StatusTag, SpellPoints, bEnableSpendPoints, bEnableEquip);
	FString CurrentDescription;
	FString NextLevelDescription;
	GetAuraASC()->
		GetDescriptionsByAbilityTag(CurrentSelectedSpell.AbilityTag, CurrentDescription, NextLevelDescription);
	OnSpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, CurrentDescription, NextLevelDescription);
}

void USpellMenuWidgetController::ShouldEnableButtons(
	const FGameplayTag& AbilityStatus,
	int32 SpellPoints,
	bool& bShouldEnableSpellPointsButton,
	bool& bShouldEnableEquipButton)
{
	bShouldEnableSpellPointsButton = false;
	bShouldEnableEquipButton = false;
	if (AbilityStatus.MatchesTagExact(AuraGameplayTags::Ability_Status_Equipped) ||
		AbilityStatus.MatchesTagExact(AuraGameplayTags::Ability_Status_Unlocked))
	{
		bShouldEnableEquipButton = true;
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPointsButton = true;
		}
	}
	else if (AbilityStatus.MatchesTagExact(AuraGameplayTags::Ability_Status_Eligible))
	{
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPointsButton = true;
		}
	}
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag,
                                                   const FGameplayTag& PrevInputTag)
{
	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = AuraGameplayTags::Ability_Status_Unlocked;
	LastSlotInfo.InputTag = PrevInputTag;
	LastSlotInfo.AbilityTag = AuraGameplayTags::Ability_None;
	// Broadcast empty info if PreviousSlot is a valid slot. Only if equipping an already-equipped spell
	AbilityInfoDelegate.Broadcast(LastSlotInfo);

	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	Info.StatusTag = AuraGameplayTags::Ability_Status_Equipped;
	Info.InputTag = InputTag;
	AbilityInfoDelegate.Broadcast(Info);

	OnSpellEquippedDelegate.Broadcast(AbilityTag);
	GlobeDeselect();
	EquippedGlobeDeselect();
}
