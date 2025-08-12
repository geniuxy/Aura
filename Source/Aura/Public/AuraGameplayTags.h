#pragma once
#include "NativeGameplayTags.h"

namespace AuraGameplayTags
{
	/** Input Tags **/
	AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_LMB);
	AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_RMB);
	AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_1);
	AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_2);
	AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_3);
	AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_4);

	/** Aura Tags**/
	AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary_Strength);
	AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary_Intelligence);
	AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary_Vigor);
	AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Primary_Resilience);
	AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_Armor);
	AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_ArmorPenetration);
	AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_BlockChance);
	AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_CriticalHitChance);
	AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_CriticalHitDamage);
	AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_CriticalHitResistance);
	AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_HealthRegeneration);
	AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_ManaRegeneration);
	AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_MaxHealth);
	AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Secondary_MaxMana);
	AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Vital_Health);
	AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attributes_Vital_Mana);
	
	AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Montage_FireBolt);
	
	/** Enemy Tags**/

	/** Shared Tags**/
	AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_HealthPotion);
	AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_ManaPotion);
	AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_HealthCrystal);
	AURA_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_ManaCrystal);

	
	
	/** Game Data tags **/
}