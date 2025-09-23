// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

class AController;

UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()

public:
	AAuraCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	/** Player Interfaces */
	virtual void AddToPlayerXP_Implementation(int32 InXP) override;
	virtual void LevelUp_Implementation() override;
	/** End Player Interfaces */

	/** Combat Interfaces */
	virtual int32 GetLevel_Implementation() override;
	/** End Combat Interfaces */

private:
	virtual void InitAbilityActorInfo() override;
};
