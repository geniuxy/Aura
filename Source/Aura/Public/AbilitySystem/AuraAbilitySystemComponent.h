#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetsTagDelegate, const FGameplayTagContainer&);
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven);
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&);
DECLARE_MULTICAST_DELEGATE_FourParams(FAbilityStatusChanged, const FGameplayTag& /* AbilityTag */,
                                      const FGameplayTag& /* AbilityType */, const FGameplayTag& /* StatusTag */,
                                      int32 /* AbilityLevel */);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityEquipped, const FGameplayTag& /*AbilityTag*/,
                                       const FGameplayTag& /*InputTag*/, const FGameplayTag& /*PrevInputTag*/)
DECLARE_MULTICAST_DELEGATE_OneParam(FDeactivatePassiveAbility, const FGameplayTag& /*AbilityTag*/)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPassiveActivate, const FGameplayTag& /*AbilityTag*/, bool /*bActivate*/)

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void AbilityActorInfoSet();

	FEffectAssetsTagDelegate EffectAssetsTagDelegate;
	FAbilitiesGiven AbilitiesGivenDelegate;
	FAbilityStatusChanged AbilityStatusChangedDelegate;
	FAbilityEquipped AbilityEquippedDelegate;
	FDeactivatePassiveAbility DeactivatePassiveAbilityDelegate;
	FOnPassiveActivate OnPassiveActivateDelegate;

	void AddAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	void AddPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);
	bool bStartupAbilitiesGiven = false;

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);

	void ForEachAbility(const FForEachAbility& Delegate);

	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	FGameplayAbilitySpec* GetSpecFromInputTag(const FGameplayTag& InputTag);
	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);
	bool IsSlotOfInputTagEmpty(const FGameplayTag& InputTag);
	bool IsPassiveAbility(const FGameplayAbilitySpec& Spec);
	static bool AbilityHasEquipped(const FGameplayAbilitySpec* AbilitySpec, const FGameplayTag& InputTag);
	static bool AbilityHasAnyInputTag(const FGameplayAbilitySpec* AbilitySpec);
	static void AssignInputTagToAbility(FGameplayAbilitySpec& AbilitySpec, FGameplayTag InputTag);
	void ClearAbilityOfCurrentEquippedSpell(FGameplayTag InputTag);
	static void ClearEquippedSpell(FGameplayAbilitySpec* AbilitySpec);

	bool GetDescriptionsByAbilityTag(
		const FGameplayTag& AbilityTag,
		FString& OutDescription,
		FString& OutNextLevelDescription
	);

	void UpgradeAttribute(const FGameplayTag& AttributeTag);

	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);

	void UpdateAbilityStatuses(int32 Level);

	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoint(const FGameplayTag& AbilityTag, const FGameplayTag& AbilityType);

	UFUNCTION(Server, Reliable)
	void ServerEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag);

	UFUNCTION(Client, Reliable)
	void ClientEquipAbility(FGameplayTag AbilityTag, FGameplayTag InputTag, FGameplayTag PrevInputTag);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastActivatePassiveEffect(const FGameplayTag& AbilityTag, bool bActivate);

protected:
	virtual void OnRep_ActivateAbilities() override;

	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(
		UAbilitySystemComponent* AbilitySystemComponent,
		const FGameplayEffectSpec& GameplayEffectSpec,
		FActiveGameplayEffectHandle ActiveGameplayEffectHandle
	);

	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& AbilityType,
	                               const FGameplayTag& StatusTag, int32 AbilityLevel);
};
