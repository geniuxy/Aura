// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraPassiveAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

void UAuraPassiveAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);


	UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo()));
	if (AuraASC)
	{
		AuraASC->DeactivatePassiveAbilityDelegate.AddUObject(this, &UAuraPassiveAbility::ReceiveDeactivate);
	}
}

void UAuraPassiveAbility::ReceiveDeactivate(const FGameplayTag& InAbilityTag)
{
	if (AbilityTags.HasTagExact(InAbilityTag))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}
