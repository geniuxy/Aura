// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetsTagDelegate, const FGameplayTagContainer&);

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

	void AddAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);

	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);

protected:
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(
		UAbilitySystemComponent* AbilitySystemComponent,
		const FGameplayEffectSpec& GameplayEffectSpec,
		FActiveGameplayEffectHandle ActiveGameplayEffectHandle
	);
};
