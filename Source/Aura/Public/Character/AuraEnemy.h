// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraCharacterBase.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class UWidgetComponent;

UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();

	/** Enemy Interfaces */
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	/** End Enemy Interfaces */

	/** Combat Interfaces */
	virtual int32 GetLevel() override;
	/** End Combat Interfaces */

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedDelegate OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedDelegate OnMaxHealthChanged;

protected:
	virtual void BeginPlay() override;

	virtual void InitAbilityActorInfo() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy Defaults")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;
};
