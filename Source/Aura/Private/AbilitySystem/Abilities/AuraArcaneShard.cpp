// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraArcaneShard.h"

FString UAuraArcaneShard::GetCurrentLevelDescription(int32 Level)
{
	return BuildDescription(Level, TEXT("奥术碎片"));
}

FString UAuraArcaneShard::GetNextLevelDescription(int32 Level)
{
	return BuildDescription(Level, TEXT("下一等级:"));
}

FString UAuraArcaneShard::BuildDescription(int32 Level, const FString& TitleTag)
{
	const int32 ScalableDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	NumShards = FMath::Min(Level, MaxNumShards);

	return FString::Printf(TEXT(
		// Title
		"<Title>%s</>\n\n"

		// Level
		"<Small>技能等级: </><Level>%d</>\n"
		// ManaCost
		"<Small>魔法消耗: </><ManaCost>%.1f</>\n"
		// Cooldown
		"<Small>冷却时间: </><Cooldown>%.1f</>\n\n"

		// Number of Arcane Shards
		"<Default>释放%d个奥术碎片，并造成</>"

		// Damage
		"<Damage>%d</><Default>范围伤害</>"
	), *TitleTag, Level, ManaCost, Cooldown, NumShards, ScalableDamage);
}
