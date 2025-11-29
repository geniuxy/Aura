// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckPoint/AuraCheckPoint.h"

#include "Components/SphereComponent.h"
#include "Game/AuraGameInstance.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"

AAuraCheckPoint::AAuraCheckPoint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>("CheckpointMesh");
	CheckpointMesh->SetupAttachment(GetRootComponent());
	CheckpointMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CheckpointMesh->SetCollisionResponseToAllChannels(ECR_Block);

	CheckpointMesh->SetCustomDepthStencilValue(CustomDepthStencil);
	CheckpointMesh->MarkRenderStateDirty();

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(CheckpointMesh);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	MoveToComponent = CreateDefaultSubobject<USceneComponent>("MoveToComponent");
	MoveToComponent->SetupAttachment(GetRootComponent());
}

void AAuraCheckPoint::LoadActor_Implementation()
{
	if (bReached)
	{
		Glow();
	}
}

void AAuraCheckPoint::HighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(true);
}

void AAuraCheckPoint::UnHighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(false);
}

void AAuraCheckPoint::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	OutDestination = MoveToComponent->GetComponentLocation();
}

void AAuraCheckPoint::BeginPlay()
{
	Super::BeginPlay();

	if (bBindOverlapCallback)
	{
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraCheckPoint::OnSphereOverlap);
	}
}

void AAuraCheckPoint::OnSphereOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (bReached)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		return;
	}
	if (OtherActor->Implements<UPlayerInterface>())
	{
		// 更新这个CheckPoint的状态
		bReached = true;
		AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
		if (AuraGameMode)
		{
			AuraGameMode->SaveWorldState(GetWorld());
			// 更新GameInstance中的起始点信息
			if (UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(AuraGameMode->GetGameInstance()))
			{
				AuraGameInstance->CurMapName = AuraGameMode->GetMapNameFromMapAssetName(CurMap);
				AuraGameInstance->PlayerStartTag = PlayerStartTag; // 右边这个是指CheckPoint的StartTag标签
			}
		}

		IPlayerInterface::Execute_SaveProgress(OtherActor);
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Glow();
	}
}

void AAuraCheckPoint::Glow()
{
	UMaterialInstanceDynamic* DynamicMaterialInstace = UMaterialInstanceDynamic::Create(
		CheckpointMesh->GetMaterial(0), this);
	CheckpointMesh->SetMaterial(0, DynamicMaterialInstace);
	StartGlowTimeline(DynamicMaterialInstace);
}
