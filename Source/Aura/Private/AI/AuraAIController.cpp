// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AuraAIController.h"

#include "Runtime/AIModule/Classes/BehaviorTree/BehaviorTreeComponent.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h"

AAuraAIController::AAuraAIController()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>("Blackboard");
	checkf(Blackboard, TEXT("Blackboard creation failed in %s"), *GetNameSafe(this))
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTreeComponent");
	checkf(BehaviorTreeComponent, TEXT("BehaviorTreeComponent creation failed in %s"), *GetNameSafe(this));
}
