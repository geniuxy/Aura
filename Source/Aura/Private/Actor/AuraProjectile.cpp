// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/Aura.h"
#include "Components/AudioComponent.h"

AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickInterval(0.5f);
	bReplicates = true;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 500.f;
	ProjectileMovement->MaxSpeed = 500.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);
	SetReplicateMovement(true);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);

	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());

	LastFrameLocation = FVector();
	ThisFrameLocation = FVector();
}

void AAuraProjectile::Destroyed()
{
	if (!bHit && !HasAuthority())
	{
		OnHit();
	}
	Super::Destroyed();
}

void AAuraProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	LastFrameLocation = ThisFrameLocation;
	ThisFrameLocation = GetActorLocation();
	const double DistanceBetweenFrame = (ThisFrameLocation - LastFrameLocation).Length();
	if (DistanceBetweenFrame < 5.f)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, SelfImpactEffect, GetActorLocation());
		if (LoopingSoundComponent && LoopingSoundComponent->IsPlaying())
		{
			LoopingSoundComponent->Stop();
			LoopingSoundComponent->DestroyComponent();
		}
		Destroy();
	}
}

void AAuraProjectile::OnHit()
{
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	if (LoopingSoundComponent && LoopingSoundComponent->IsPlaying())
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	bHit = true;
}

void AAuraProjectile::OnSphereOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!IsValidOverlap(OtherActor)) return;
	// bHit不会在不同端之间复制，每个段的bHit值独立
	// 第一次命中时，无论服务器还是客户端，先把声光表现一次性播完
	if (!bHit) OnHit();

	if (HasAuthority()) // 只有服务器能“真”结算
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			DamageEffectParams.DeathImpulse = DeathImpulse;

			const bool bKnockBack = FMath::RandRange(1, 100) < DamageEffectParams.KnockBackChance;
			if (bKnockBack)
			{
				FRotator Rotation = GetActorRotation();
				Rotation.Pitch = 10.f;

				const FVector KnockBackDirection = Rotation.Vector();
				const FVector KnockBackForce = KnockBackDirection * DamageEffectParams.KnockBackForceMagnitude;
				DamageEffectParams.KnockBackForce = KnockBackForce;
			}

			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}

		Destroy(); // 权威端销毁，所有客户端同步销毁
	}
	else // 客户端
	{
		bHit = true; // 告诉本地：特效已播，后面 Destroyed() 不再补播
	}
}

bool AAuraProjectile::IsValidOverlap(AActor* OtherActor)
{
	// 子弹打到自己发射者直接return
	// DamageEffectSpecHandle.Data.IsValid()是为了保证DamageEffectSpecHandle.Data.Get()不是空指针
	// Client会打到自己..
	// if (DamageEffectSpecHandle.Data.IsValid() &&
	// 	DamageEffectSpecHandle.Data.Get()->GetContext().GetEffectCauser() == OtherActor)
	if (!IsValid(DamageEffectParams.SourceAbilitySystemComponent)) return false;
	AActor* SourceAvatarActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	if (SourceAvatarActor == OtherActor) return false;
	// 子弹不会打到友军
	if (!UAuraAbilitySystemLibrary::IsNotFriend(SourceAvatarActor, OtherActor)) return false;

	return true;
}
