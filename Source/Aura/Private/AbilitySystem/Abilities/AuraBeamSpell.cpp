// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

FString UAuraBeamSpell::GetCurrentLevelDescription(int32 Level)
{
	return BuildDescription(Level, TEXT("闪电术"));
}

FString UAuraBeamSpell::GetNextLevelDescription(int32 Level)
{
	return BuildDescription(Level, TEXT("下一等级:"));
}

void UAuraBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		MouseHitLocation = HitResult.ImpactPoint;
		MouseHitActor = HitResult.GetActor();
	}
	else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UAuraBeamSpell::StoreOwnerVariables()
{
	if (CurrentActorInfo)
	{
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	}
}

void UAuraBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	check(OwnerCharacter);
	if (OwnerCharacter->Implements<UCombatInterface>())
	{
		if (USkeletalMeshComponent* Weapon = ICombatInterface::Execute_GetWeapon(OwnerCharacter))
		{
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(OwnerCharacter);
			FHitResult HitResult;
			const FVector SocketLocation = Weapon->GetSocketLocation(FName("TipSocket"));
			UKismetSystemLibrary::SphereTraceSingle(
				OwnerCharacter,
				SocketLocation,
				BeamTargetLocation,
				3.f,
				TraceTypeQuery1,
				false,
				ActorsToIgnore,
				EDrawDebugTrace::None,
				HitResult,
				true
			);

			if (HitResult.bBlockingHit)
			{
				MouseHitLocation = HitResult.ImpactPoint;
				MouseHitActor = HitResult.GetActor();
			}
		}
	}

	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(MouseHitActor))
	{
		if (!CombatInterface->GetOnDeathDelegate()->IsAlreadyBound(this, &UAuraBeamSpell::PrimaryTargetDied))
		{
			CombatInterface->GetOnDeathDelegate()->AddDynamic(this, &UAuraBeamSpell::PrimaryTargetDied);
		}
	}
}

void UAuraBeamSpell::StoreAdditionTargets(TArray<AActor*>& OutAdditionalTargets)
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	ActorsToIgnore.Add(MouseHitActor);

	TArray<AActor*> OverlappingActors;
	UAuraAbilitySystemLibrary::GetLivePlayerWithinRadius(
		GetAvatarActorFromActorInfo(),
		OverlappingActors,
		ActorsToIgnore,
		450.f,
		MouseHitActor->GetActorLocation(),
		false
	);

	int32 NumAdditionalTargets = FMath::Min(GetAbilityLevel() - 1, MaxNumShockTargets);

	UAuraAbilitySystemLibrary::GetClosestTargets(
		NumAdditionalTargets,
		OverlappingActors,
		OutAdditionalTargets,
		MouseHitActor->GetActorLocation()
	);

	for (AActor* TargetActor : OutAdditionalTargets)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(TargetActor))
		{
			if (!CombatInterface->GetOnDeathDelegate()->IsAlreadyBound(this, &UAuraBeamSpell::AdditionalTargetDied))
			{
				CombatInterface->GetOnDeathDelegate()->AddDynamic(this, &UAuraBeamSpell::AdditionalTargetDied);
			}
		}
	}
}

FString UAuraBeamSpell::BuildDescription(int32 Level, const FString& TitleTag)
{
	const int32 ScalableDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	const int32 ShockTargets = FMath::Min(Level - 1, MaxNumShockTargets);

	return FString::Printf(TEXT(
		// Title
		"<Title>%s</>\n\n"

		// Level
		"<Small>技能等级: </><Level>%d</>\n"
		// ManaCost
		"<Small>魔法消耗: </><ManaCost>%.1f</>\n"
		// Cooldown
		"<Small>冷却时间: </><Cooldown>%.1f</>\n\n"

		// Number of Lightning Hurt Enemies
		"<Default>发射1束闪电，并弹射至周围%d位敌人并造成</>"

		// Damage
		"<Damage>%d</><Default>闪电伤害</>"
	), *TitleTag, Level, ManaCost, Cooldown, ShockTargets, ScalableDamage);
}
