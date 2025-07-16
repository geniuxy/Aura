// Fill out your copyright notice in the Description page of Project Settings.


#include "AuraGameplayTags.h"


namespace AuraGameplayTags
{
	/** Input Tags **/
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_LMB, "InputTag.LMB", "鼠标左键");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_RMB, "InputTag.RMB", "鼠标右键");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_1, "InputTag.1", "数字键 1");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_2, "InputTag.2", "数字键 2");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_3, "InputTag.3", "数字键 3");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_4, "InputTag.4", "数字键 4");

	/** Aura Tags**/
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Strength, "Attributes.Primary.Strength",
	                               "角色的力量属性，影响物理攻击的伤害和角色的负重能力。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Intelligence, "Attributes.Primary.Intelligence",
	                               "角色的智力属性，影响魔法技能的效果和法力值的恢复速度。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Vigor, "Attributes.Primary.Vigor",
	                               "角色的活力属性，影响生命值上限和生命值的恢复速度。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Primary_Resilience, "Attributes.Primary.Resilience",
	                               "角色的韧性属性，影响角色的防御能力和受到的伤害减免。");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_Armor, "Attributes.Secondary.Armor",
	                               "角色的护甲属性");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_ArmorPenetration, "Attributes.Secondary.ArmorPenetration",
	                               "造成伤害时，无视目标护甲的数值。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_BlockChance, "Attributes.Secondary.BlockChance",
	                               "格挡几率：触发后按格挡值减少所受伤害。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_CriticalHitChance, "Attributes.Secondary.CriticalHitChance",
	                               "暴击几率：造成伤害时触发暴击的概率。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_CriticalHitDamage, "Attributes.Secondary.CriticalHitDamage",
	                               "暴击伤害：触发暴击时，额外造成的伤害倍率。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_CriticalHitResistance, "Attributes.Secondary.CriticalHitResistance",
	                               "暴击抗性：降低被暴击的概率及暴击伤害倍率。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_HealthRegeneration, "Attributes.Secondary.HealthRegeneration",
	                               "生命恢复：单位时间自动回复的生命值。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_ManaRegeneration, "Attributes.Secondary.ManaRegeneration",
	                               "法力恢复：单位时间自动回复的法力值。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_MaxHealth, "Attributes.Secondary.MaxHealth",
	                               "角色的最大生命值，决定了角色能够承受的最大伤害量。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_MaxMana, "Attributes.Secondary.MaxMana",
	                               "角色的最大法力值，决定了角色能够存储的最大法力量。");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Vital_Health, "Attributes.Vital.Health",
	                               "角色当前的生命值，表示角色的生存能力，当生命值为0时角色死亡。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Vital_Mana, "Attributes.Vital.Mana",
	                               "角色当前的法力值，用于施展技能和法术，法力值耗尽时无法使用消耗法力的技能。");

	/** Enemy Tags**/

	/** Shared Tags**/

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_HealthPotion, "Message.HealthPotion",
	                               "一次性恢复一定量的生命值。使用后立即增加生命值，效果为一次性。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_ManaPotion, "Message.ManaPotion",
	                               "一次性恢复一定量的法力值。使用后立即增加法力值，效果为一次性。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_HealthCrystal, "Message.HealthCrystal",
	                               "持续恢复生命值。使用后将在一段时间内持续恢复生命值，效果为持续性。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_ManaCrystal, "Message.ManaCrystal",
	                               "持续恢复法力值。使用后将在一段时间内持续恢复法力值，效果为持续性。");

	/** Game Data tags **/
}
