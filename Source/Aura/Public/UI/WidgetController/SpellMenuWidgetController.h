#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayTags.h"
#include "GameplayTagContainer.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

struct FGameplayTag;
class UAuraUserWidget;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpellGlobeSelectedSignature, UAuraUserWidget*, SpellGlobe);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpellGlobeSelectedSignature,
                                             bool, bSpendPointsButtonEnabled, bool, bEquipButtonEnabled);

struct SelectedSpell
{
	FGameplayTag AbilityTag = FGameplayTag();
	FGameplayTag StatusTag = FGameplayTag();
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category="GAS|Spells")
	FOnPlayerStatChangedSignature SpellPointsChangeDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|Spells")
	FSpellGlobeSelectedSignature SpellGlobeSelectedDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|Spells")
	FOnSpellGlobeSelectedSignature OnSpellGlobeSelectedDelegate;

	UFUNCTION(BlueprintCallable, Category = "GAS|Spells")
	void SelectSpellGlobe(UAuraUserWidget* SpellGlobe);

	UFUNCTION(BlueprintCallable, Category = "GAS|Spells")
	void OnSpellGlobeSelected(const FGameplayTag& AbilityTag);

private:
	static void ShouldEnableButtons(
		const FGameplayTag& AbilityStatus,
		int32 SpellPoints,
		bool& bShouldEnableSpellPointsButton,
		bool& bShouldEnableEquipButton
	);

	SelectedSpell CurrentSelectedSpell = {AuraGameplayTags::Ability_None, AuraGameplayTags::Ability_Status_Locked};
	int32 CurrentSpellPoints = 0;
};
