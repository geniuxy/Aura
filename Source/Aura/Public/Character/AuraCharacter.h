// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

class UNiagaraComponent;
class USpringArmComponent;
class UCameraComponent;
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
	virtual int32 GetXP_Implementation() const override;
	virtual int32 FindLevelForXP_Implementation(int32 InXP) const override;
	virtual int32 GetAttributePointsReward_Implementation(int32 InCurLevel) const override;
	virtual int32 GetSpellPointsReward_Implementation(int32 InCurLevel) const override;
	virtual int32 GetAttributePoints_Implementation() const override;
	virtual int32 GetSpellPoints_Implementation() const override;
	virtual void AddToPlayerLevel_Implementation(int32 InLevel) override;
	virtual void AddToAttributePoints_Implementation(int InAttributePoints) override;
	virtual void AddToSpellPoints_Implementation(int InSpellPoints) override;
	virtual void ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial = nullptr) override;
	virtual void HideMagicCircle_Implementation() override;
	virtual void SaveProgress_Implementation(const FName& CheckpointTag) override;
	/** End Player Interfaces */

	/** Combat Interfaces */
	virtual int32 GetLevel_Implementation() override;
	/** End Combat Interfaces */
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> TopDownCameraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	virtual void InitAbilityActorInfo() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelUpParticles() const;
};
