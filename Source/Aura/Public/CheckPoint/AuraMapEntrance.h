// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CheckPoint/AuraCheckPoint.h"
#include "AuraMapEntrance.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraMapEntrance : public AAuraCheckPoint
{
	GENERATED_BODY()

public:
	AAuraMapEntrance(const FObjectInitializer& ObjectInitializer);

	/* Highlight Interface */
	virtual void HighlightActor_Implementation() override;
	/* Highlight Interface */

	/* Save Interface */
	virtual void LoadActor_Implementation() override;
	/* end Save Interface */

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> DestinationMap;

	UPROPERTY(EditAnywhere)
	FName DestinationPlayerStartTag;

protected:
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;
};
