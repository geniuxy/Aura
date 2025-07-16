// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraInputConfig.h"
#include "EnhancedInputComponent.h"
#include "AuraInputComponent.generated.h"

class UAuraInputConfig;
/**
 * 
 */
UCLASS()
class AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
	void BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc,
	                        ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc);
};

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
void UAuraInputComponent::BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* Object,
                                             PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc,
                                             HeldFuncType HeldFunc)
{
	checkf(InputConfig, TEXT("InputConfig is null!"));

	for (const FAuraInputActionInfo& Info : InputConfig->AbilityInputActionInfos)
	{
		if (Info.InputAction && Info.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(Info.InputAction, ETriggerEvent::Started, Object, PressedFunc, Info.InputTag);
			}

			if (ReleasedFunc)
			{
				BindAction(Info.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Info.InputTag);
			}

			if (HeldFunc)
			{
				BindAction(Info.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, Info.InputTag);
			}
		}
	}
}
