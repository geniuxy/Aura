#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
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
	GetAuraASC()->AbilityStatusChangedDelegate.AddLambda(
		[this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag)
		{
			if (CurrentSelectedSpell.AbilityTag.MatchesTagExact(AbilityTag))
			{
				CurrentSelectedSpell.StatusTag = StatusTag;
				bool bEnableSpendPoints = false;
				bool bEnableEquip = false;
				ShouldEnableButtons(StatusTag, CurrentSpellPoints, bEnableSpendPoints, bEnableEquip);
				OnSpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip);
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

			bool bEnableSpendPoints = false;
			bool bEnableEquip = false;
			ShouldEnableButtons(CurrentSelectedSpell.StatusTag, CurrentSpellPoints, bEnableSpendPoints, bEnableEquip);
			OnSpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip);
		}
	);
}

void USpellMenuWidgetController::SelectSpellGlobe(UAuraUserWidget* SpellGlobe)
{
	SpellGlobeSelectedDelegate.Broadcast(SpellGlobe);
}

void USpellMenuWidgetController::OnSpellGlobeSelected(const FGameplayTag& AbilityTag)
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
	CurrentSelectedSpell.StatusTag = SpellStatus;

	bool bEnableSpendPoints = false;
	bool bEnableEquip = false;
	ShouldEnableButtons(SpellStatus, SpellPoints, bEnableSpendPoints, bEnableEquip);
	OnSpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip);
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
