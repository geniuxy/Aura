// Fill out your copyright notice in the Description page of Project Settings.


#include "AuraGameplayTags.h"


namespace AuraGameplayTags
{
	/** Input Tags **/

	/** Aura Tags**/

	/** Enemy Tags**/

	/** Shared Tags**/
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Vital_Health, "Attributes.Vital.Health",
	                               "角色当前的生命值，表示角色的生存能力，当生命值为0时角色死亡。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Vital_MaxHealth, "Attributes.Vital.MaxHealth",
	                               "角色的最大生命值，决定了角色能够承受的最大伤害量。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Vital_Mana, "Attributes.Vital.Mana",
	                               "角色当前的法力值，用于施展技能和法术，法力值耗尽时无法使用消耗法力的技能。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Vital_MaxMana, "Attributes.Vital.MaxMana",
	                               "角色的最大法力值，决定了角色能够存储的最大法力量。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Strength, "Attributes.Primary.Strength",
	                               "角色的力量属性，影响物理攻击的伤害和角色的负重能力。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Intelligence, "Attributes.Primary.Intelligence",
	                               "角色的智力属性，影响魔法技能的效果和法力值的恢复速度。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Vigor, "Attributes.Primary.Vigor",
	                               "角色的活力属性，影响生命值和法力值的上限，以及角色的耐力。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Resilience, "Attributes.Primary.Resilience",
	                               "角色的韧性属性，影响角色的防御能力和受到的伤害减免。");

	/** Game Data tags **/
}
