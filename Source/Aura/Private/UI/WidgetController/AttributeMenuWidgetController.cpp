// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	checkf(AttributeInfos, TEXT("AttributeInfos is not set in %s"), *GetName());

	FAuraAttributeInfo Info = AttributeInfos->FindAttributeInfoForTaf(AuraGameplayTags::Attributes_Primary_Strength);
	Info.AttributeValue = AuraAttributeSet->GetStrength();
	AttributeInfoDelegate.Broadcast(Info);
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
}
