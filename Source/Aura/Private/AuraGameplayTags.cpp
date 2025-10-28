// Fill out your copyright notice in the Description page of Project Settings.


#include "AuraGameplayTags.h"

namespace AuraGameplayTags
{
	/** Input Tags **/
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_None, "InputTag.None", "不具备任何Input Tag");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_LMB, "InputTag.LMB", "鼠标左键");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_RMB, "InputTag.RMB", "鼠标右键");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Passive_1, "InputTag.Passive.1", "被动1");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Passive_2, "InputTag.Passive.2", "被动2");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_1, "InputTag.1", "数字键 1");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_2, "InputTag.2", "数字键 2");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_3, "InputTag.3", "数字键 3");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_4, "InputTag.4", "数字键 4");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Player_Block_CursorTrace, "Player.Block.CursorTrace", "Tag用于表明禁止鼠标轨迹移动操作");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Player_Block_InputPressed, "Player.Block.InputPressed", "Tag用于表明禁止输入按下");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Player_Block_InputHeld, "Player.Block.InputHeld", "Tag用于表明禁止持续按下输入");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Player_Block_InputReleased, "Player.Block.InputReleased", "Tag用于表明禁止执行释放输入键的行为");

	/** Game Data Tags**/
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
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Secondary_CriticalHitResistance,
	                               "Attributes.Secondary.CriticalHitResistance",
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

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Resistance_Fire, "Attributes.Resistance.Fire", "火焰抗性");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Resistance_Lightning, "Attributes.Resistance.Lightning", "闪电抗性");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Resistance_Arcane, "Attributes.Resistance.Arcane", "奥术抗性");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Resistance_Physical, "Attributes.Resistance.Physical", "物理抗性");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_Meta_IncomingXP, "Attributes.Meta.IncomingXP", "元数据经验值");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage, "Damage", "伤害");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Fire, "Damage.Fire", "火焰伤害");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Lightning, "Damage.Lightning", "闪电伤害");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Arcane, "Damage.Arcane", "奥术伤害");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Physical, "Damage.Physical", "物理伤害");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Fire, "Debuff.Fire", "火焰伤害buff");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Lightning, "Debuff.Lightning", "闪电伤害buff");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Arcane, "Debuff.Arcane", "奥术伤害buff");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Physical, "Debuff.Physical", "物理伤害buff");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Chance, "Debuff.Chance", "debuff几率");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Damage, "Debuff.Damage", "debuff造成伤害");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Duration, "Debuff.Duration", "debuff持续时间");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Debuff_Frequency, "Debuff.Frequency", "debuff作用效果的频率");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Montage_FireBolt, "Event.Montage.FireBolt",
	                               "火球术技能的动画事件标签，用于触发火球术的施放动画。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Montage_Electrocute, "Event.Montage.Electrocute",
	                               "闪电术技能的动画事件标签，用于触发闪电术的施放动画。");

	/** Enemy Tags**/
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Montage_Attack_1, "Event.Montage.Attack.1", "用于代表敌人的1号Montage。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Montage_Attack_2, "Event.Montage.Attack.2", "用于代表敌人的2号Montage。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Montage_Attack_3, "Event.Montage.Attack.3", "用于代表敌人的3号Montage。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Montage_Attack_4, "Event.Montage.Attack.4", "用于代表敌人的4号Montage。");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(CombatSocket_Weapon, "CombatSocket.Weapon", "指代敌人的进攻方式属于武器槽位。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(CombatSocket_LeftHand, "CombatSocket.LeftHand", "指代敌人的进攻方式属于左手槽位。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(CombatSocket_RightHand, "CombatSocket.RightHand", "指代敌人的进攻方式属于右手槽位。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(CombatSocket_Tail, "CombatSocket.Tail", "指代敌人的进攻方式属于尾巴槽位。");

	/** Shared Tags**/

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_HealthPotion, "Message.HealthPotion",
	                               "一次性恢复一定量的生命值。使用后立即增加生命值，效果为一次性。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_ManaPotion, "Message.ManaPotion",
	                               "一次性恢复一定量的法力值。使用后立即增加法力值，效果为一次性。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_HealthCrystal, "Message.HealthCrystal",
	                               "持续恢复生命值。使用后将在一段时间内持续恢复生命值，效果为持续性。");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_ManaCrystal, "Message.ManaCrystal",
	                               "持续恢复法力值。使用后将在一段时间内持续恢复法力值，效果为持续性。");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_None, "Ability.None", "不具备任何Ability");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_HitReact, "Ability.HitReact", "用于Activate对应的受击反应的GameAbility");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Attack_Melee, "Ability.Attack.Melee", "用于Activate对应的近距离攻击的GameAbility");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Attack_Ranged, "Ability.Attack.Ranged", "用于Activate对应的远距离攻击的GameAbility");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Summon, "Ability.Summon", "用于Activate对应的召唤流的GameAbility");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Skill_Fire_FireBolt, "Ability.Skill.Fire.FireBolt", "主角的火球技能");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Skill_Lightning_Electrocute, "Ability.Skill.Lightning.Electrocute",
	                               "主角的闪电技能");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Type_None, "Ability.Type.None", "能力类型：无");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Type_Offensive, "Ability.Type.Offensive", "能力类型：主动");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Type_Passive, "Ability.Type.Passive", "能力类型：被动");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Status_Locked, "Ability.Status.Locked", "能力状态：锁定中");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Status_Eligible, "Ability.Status.Eligible", "能力状态：可解锁");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Status_Unlocked, "Ability.Status.Unlocked", "能力状态：已解锁");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Status_Equipped, "Ability.Status.Equipped", "能力状态：已装备");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Cooldown_Skill_FireBolt, "Cooldown.Skill.FireBolt", "主角的火球技能的冷却时间");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Effect_HitReact, "Effect.HitReact", "用于对应的GameEffect来向Actor添加HitReact的Tag标签");

	/** GameplayCue Tags **/
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Enemy_MeleeAttack_Impact, "GameplayCue.Enemy.MeleeAttack.Impact",
	                               "敌人近战攻击相关的特效");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Aura_Lightning_ShockBurst, "GameplayCue.Aura.Lightning.ShockBurst",
	                               "Aura闪电术的施法音效");
}
