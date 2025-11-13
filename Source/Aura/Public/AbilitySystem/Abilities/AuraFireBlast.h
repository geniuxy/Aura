// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraDamageGameplayAbility.h"
#include "AuraFireBlast.generated.h"

class AAuraFireBall;
/**
 * 
 */
UCLASS()
class AURA_API UAuraFireBlast : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

public:
	virtual FString GetCurrentLevelDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;

	UFUNCTION(BlueprintCallable)
	TArray<AAuraFireBall*> SpawnFireBalls();

protected:
	virtual FString BuildDescription(int32 Level, const FString& TitleTag) override;

	UPROPERTY(EditDefaultsOnly)
	int32 NumFireBalls = 5;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AAuraFireBall> FireBallClass;
};
