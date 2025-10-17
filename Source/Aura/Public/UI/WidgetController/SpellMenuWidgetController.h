#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayTags.h"
#include "GameplayTagContainer.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

struct FGameplayTag;
class UAuraUserWidget;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpellGlobeSelectedSignature, UAuraUserWidget*, SpellGlobe);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpellEquippedSignature, const FGameplayTag&, AbilityTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FOnSpellGlobeSelectedSignature,
	bool, bSpendPointsButtonEnabled,
	bool, bEquipButtonEnabled,
	FString, CurrentLevelDescriptionString,
	FString, NextLevelDescriptionString
);

struct SelectedSpell
{
	FGameplayTag AbilityTag = FGameplayTag();
	FGameplayTag AbilityType = FGameplayTag();
	FGameplayTag StatusTag = FGameplayTag();
};

struct SelectedEquippedSpell
{
	FGameplayTag AbilityType = FGameplayTag();
	FGameplayTag InputTag = FGameplayTag();
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
	FSpellGlobeSelectedSignature EquippedSpellGlobeSelectedDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|Spells")
	FOnSpellGlobeSelectedSignature OnSpellGlobeSelectedDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|Spells")
	FOnSpellEquippedSignature OnSpellEquippedDelegate;

	UFUNCTION(BlueprintCallable, Category = "GAS|Spells")
	void SelectSpellGlobe(UAuraUserWidget* SpellGlobe);

	UFUNCTION(BlueprintCallable, Category = "GAS|Spells")
	void SelectEquippedSpellGlobe(UAuraUserWidget* EquippedSpellGlobe);

	UFUNCTION(BlueprintCallable, Category = "GAS|Spells")
	void OnSpellGlobeSelected(const FGameplayTag& AbilityTag, const FGameplayTag& AbilityType);

	UFUNCTION(BlueprintCallable, Category = "GAS|Spells")
	void SpendSpellPointsButtonPressed();

	UFUNCTION(BlueprintCallable, Category = "GAS|Spells")
	void EquippedButtonPressed();

	UFUNCTION(BlueprintCallable, Category = "GAS|Spells")
	void SetCurrentSelectedEquippedSpell(const FGameplayTag& InAbilityType, const FGameplayTag& InInputTag);

	UFUNCTION(BlueprintCallable, Category = "GAS|Spells")
	void GlobeDeselect();

	UFUNCTION(BlueprintCallable, Category = "GAS|Spells")
	void EquippedGlobeDeselect();

private:
	void UpdateSelectedSpellTreeUI(int32 SpellPoints);
	static void ShouldEnableButtons(
		const FGameplayTag& AbilityStatus,
		int32 SpellPoints,
		bool& bShouldEnableSpellPointsButton,
		bool& bShouldEnableEquipButton
	);
	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag,
	                       const FGameplayTag& PrevInputTag);

	SelectedSpell CurrentSelectedSpell = {
		AuraGameplayTags::Ability_None, AuraGameplayTags::Ability_Type_None, AuraGameplayTags::Ability_Status_Locked
	};
	SelectedEquippedSpell CurrentSelectedEquippedSpell = {
		AuraGameplayTags::Ability_Type_None, AuraGameplayTags::InputTag_None
	};
	int32 CurrentSpellPoints = 0;
};
