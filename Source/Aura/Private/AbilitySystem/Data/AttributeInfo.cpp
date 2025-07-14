// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/AttributeInfo.h"

#include "DebugHelper.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTaf(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
	for (auto AttributeInfo : AttributeInfos)
	{
		if (AttributeInfo.AttributeTag.MatchesTagExact(AttributeTag))
		{
			return AttributeInfo;
		}
	}

	if (bLogNotFound)
	{
		Debug::Print(FString::Printf(
			TEXT("Can't find Info for AttributeTag [%s] on AttributeInfo [%s]."), *AttributeTag.ToString(),
			*GetNameSafe(this)));
	}

	return FAuraAttributeInfo();
}
