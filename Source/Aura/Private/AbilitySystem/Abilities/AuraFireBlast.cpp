// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraFireBlast.h"

FString UAuraFireBlast::GetCurrentLevelDescription(int32 Level)
{
	if (Level == 0)
	{
		return BuildDescription(Level + 1, TEXT("火焰爆破(可学习)"));
	}
	return BuildDescription(Level, TEXT("火焰爆破"));
}

FString UAuraFireBlast::GetNextLevelDescription(int32 Level)
{
	if (Level == 1)
	{
		return Super::GetNextLevelDescription(Level);
	}
	return BuildDescription(Level, TEXT("下一等级:"));
}

FString UAuraFireBlast::BuildDescription(int32 Level, const FString& TitleTag)
{
	const int32 ScalableDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);

	return FString::Printf(TEXT(
		// Title
		"<Title>%s</>\n\n"

		// Level
		"<Small>技能等级: </><Level>%d</>\n"
		// ManaCost
		"<Small>魔法消耗: </><ManaCost>%.1f</>\n"
		// Cooldown
		"<Small>冷却时间: </><Cooldown>%.1f</>\n\n"

		// Number of FireBalls
		"<Default>向四周发射%d枚火球,每个火球都会返回时爆炸并造成</>"

		// Damage
		"<Damage>%d</><Default>火焰伤害，并有几率点燃敌人</>"
	), *TitleTag, Level, ManaCost, Cooldown, NumFireBalls, ScalableDamage);
}
