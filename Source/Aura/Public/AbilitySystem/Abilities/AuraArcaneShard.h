// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraDamageGameplayAbility.h"
#include "AuraArcaneShard.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraArcaneShard : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
public:
	virtual FString GetCurrentLevelDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;
};
