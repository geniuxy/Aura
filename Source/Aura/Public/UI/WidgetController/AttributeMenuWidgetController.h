// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

struct FGameplayAttribute;
struct FGameplayTag;
struct FAuraAttributeInfo;
class UAttributeInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoDelegate, const FAuraAttributeInfo&, Info);
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API UAttributeMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FAttributeInfoDelegate AttributeInfoDelegate;

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAttributeInfo> AttributeInfos;

private:
	void BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const;
};
