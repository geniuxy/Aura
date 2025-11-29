// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckPoint/AuraMapEntrance.h"

#include "Components/SphereComponent.h"
#include "Game/AuraGameInstance.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

AAuraMapEntrance::AAuraMapEntrance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Sphere->SetupAttachment(MoveToComponent);
}

void AAuraMapEntrance::HighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(true);
}

void AAuraMapEntrance::LoadActor_Implementation()
{
	// Do nothing when loading a Map Entrance
}

void AAuraMapEntrance::OnSphereOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UPlayerInterface>())
	{
		AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
		if (AuraGameMode)
		{
			AuraGameMode->SaveWorldState(GetWorld());
			if (UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(AuraGameMode->GetGameInstance()))
			{
				AuraGameInstance->CurMapName = AuraGameMode->GetMapNameFromMapAssetName(DestinationMap);
				AuraGameInstance->PlayerStartTag = DestinationPlayerStartTag; // 右边这个是指CheckPoint的StartTag标签
			}
		}

		IPlayerInterface::Execute_SaveProgress(OtherActor);

		UGameplayStatics::OpenLevelBySoftObjectPtr(this, DestinationMap);
	}
}
