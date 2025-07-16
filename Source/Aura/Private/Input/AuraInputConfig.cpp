// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/AuraInputConfig.h"

#include "DebugHelper.h"

const UInputAction* UAuraInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag,
                                                                   bool bLogNotFound) const
{
	for (const FAuraInputActionInfo& Info : AbilityInputActionInfos)
	{
		if (Info.InputAction && Info.InputTag.MatchesTagExact(InputTag))
		{
			return Info.InputAction;
		}
	}

	if (bLogNotFound)
	{
		Debug::Print(FString::Printf(
			TEXT("Can't find InputAction for InputTag [%s] on AuraInputConfig [%s]."), *InputTag.ToString(),
			*GetNameSafe(this)));
	}

	return nullptr;
}
