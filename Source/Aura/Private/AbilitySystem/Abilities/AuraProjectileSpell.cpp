// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "DebugHelper.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraProjectileSpell::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;
	// 只在服务器Spawn Projectile，Actor能在所有客户端“自动”出现
	// 原因：该 Actor 被标记为“可复制” => bReplicates = true;

	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(
		GetAvatarActorFromActorInfo(), AuraGameplayTags::Event_Montage_Attack_Weapon);
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());

	// SpawnActorDeferred: 需手动调用 FinishSpawning 以完成 Actor 的最终生成和初始化
	AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	UAbilitySystemComponent* SourceASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	TArray<TWeakObjectPtr<AActor>> Actors;
	Actors.Add(Projectile);
	EffectContextHandle.AddActors(Actors);
	EffectContextHandle.AddSourceObject(Projectile);
	FHitResult HitResult;
	HitResult.Location = ProjectileTargetLocation;
	EffectContextHandle.AddHitResult(HitResult);
	FGameplayEffectSpecHandle SpecHandle =
		SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

	for (auto& Pair : DamageTypes)
	{
		const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
		Debug::Print(
			FString::Printf(TEXT("Gameplay Tag: %s, Fire Bolt Damage: %f"), *Pair.Key.ToString(), ScaledDamage));
	}
	Projectile->DamageEffectSpecHandle = SpecHandle;
	Projectile->Owner = GetAvatarActorFromActorInfo();

	Projectile->FinishSpawning(SpawnTransform);
}
