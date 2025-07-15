// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	checkf(AttributeInfos, TEXT("AttributeInfos is not set in %s"), *GetName());

	for (auto& Pair : AuraAttributeSet->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	checkf(AttributeInfos, TEXT("AttributeInfos is not set in %s"), *GetName());

	for (auto& Pair : AuraAttributeSet->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			}
		);
	}
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag,
                                                            const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info = AttributeInfos->FindAttributeInfoForTaf(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
