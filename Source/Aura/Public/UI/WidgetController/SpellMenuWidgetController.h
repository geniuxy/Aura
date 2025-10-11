#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

class UAuraUserWidget;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpellGlobeSelectedSignature, UAuraUserWidget*, SpellGlobe);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category="GAS|Spells")
	FOnPlayerStatChangedSignature SpellPointsChangeDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|Spells")
	FSpellGlobeSelectedSignature SpellGlobeSelectedDelegate;

	UFUNCTION(BlueprintCallable, Category = "GAS|Spells")
	void SelectAbility(UAuraUserWidget* SpellGlobe);
};
