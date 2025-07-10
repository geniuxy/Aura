// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "DebugHelper.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);
}

void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
                                                const FGameplayEffectSpec& GameplayEffectSpec,
                                                FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	FGameplayTagContainer AssetTagContainer;
	FGameplayTagContainer GrantedTagContainer;
	GameplayEffectSpec.GetAllAssetTags(AssetTagContainer);
	GameplayEffectSpec.GetAllGrantedTags(GrantedTagContainer);

	for (const FGameplayTag& AssetTag : AssetTagContainer)
	{
		Debug::Print(FString::Printf(TEXT("Asset GE Tag: %s"), *AssetTag.ToString()));
	}

	for (const FGameplayTag& GrantedTag : GrantedTagContainer)
	{
		Debug::Print(FString::Printf(TEXT("Granted GE Tag: %s"), *GrantedTag.ToString()));
	}
}
