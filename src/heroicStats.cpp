#include "heroicStats.h"

#include "dao/generated/ScriptApi.h"
#include "dao/String.h"
#include "dao/Tlk.h"

// Retrieve stats, truncate based on if we need decimal points or not
std::wstring GetHeroicStatValue(int iValue)
{
	auto oHero = dao::script::api::GetHero();
	float fResult = dao::script::api::GetCreatureProperty(oHero, iValue);
	const bool bKeepDecimals =	iValue == hero_stat::WALKED_DISTANCE_METERS ||
								iValue == hero_stat::WALKED_DISTANCE_MILES ||
								iValue == hero_stat::TOTAL_DAMAGE_PERCENT ||
								iValue == hero_stat::HIT_RATE;

	return dao::script::api::FloatToString(fResult, 18, bKeepDecimals ? 2 : 0);
}

std::string STAT_Kills()
{
	return "Kills: " + dao::Utf8(GetHeroicStatValue(hero_stat::KILLS));
}

std::string STAT_TotalDamageDealt()
{
	return "Damage dealt: " + dao::Utf8(GetHeroicStatValue(hero_stat::TOTAL_DAMAGE_DEALT));
}

std::string STAT_FriendlyFireDealt()
{
	return "Friendly-fire damage dealt: " + dao::Utf8(GetHeroicStatValue(hero_stat::FRIENDLY_FIRE_DEALT));
}

std::string STAT_HighestDamageDealt()
{
	return "Greatest damage dealt: " + dao::Utf8(GetHeroicStatValue(hero_stat::HIGHEST_DAMAGE_DEALT));
}

std::string STAT_TotalDamagePercent()
{
	return "Contribution to party damage: " + dao::Utf8(GetHeroicStatValue(hero_stat::TOTAL_DAMAGE_PERCENT));
}

std::string STAT_HitRate()
{
	return "Hit rate: " + dao::Utf8(GetHeroicStatValue(hero_stat::HIT_RATE)) + "%";
}

std::string STAT_MostPowerfulSlain()
{
	auto oHero = dao::script::api::GetHero();
	auto fResult = dao::script::api::GetCreatureProperty(oHero, hero_stat::MOST_POWERFUL_SLAIN);
	auto sResult = dao::tlk::String(dao::script::api::FloatToInt(fResult));
	return "Most powerful foe slain: " + dao::Utf8(sResult);
}

std::string STAT_Injuries()
{
	return "Injuries: " + dao::Utf8(GetHeroicStatValue(hero_stat::INJURIES));
}

std::string STAT_PartyKills()
{
	return "Party kills: " + dao::Utf8(GetHeroicStatValue(hero_stat::PARTY_KILLS));
}

std::string STAT_PartyDamageDealt()
{
	return "Damage dealt by party: " + dao::Utf8(GetHeroicStatValue(hero_stat::PARTY_DAMAGE_DEALT));
}

std::string STAT_MostPowerfulSlainByParty()
{
	auto oHero = dao::script::api::GetHero();
	auto fResult = dao::script::api::GetCreatureProperty(oHero, hero_stat::MOST_POWERFUL_SLAIN_BY_PARTY);
	auto sResult = dao::tlk::String(dao::script::api::FloatToInt(fResult));
	return "Most powerful foe slain by party: " + dao::Utf8(sResult);
}

std::string STAT_GrouptypeDemons()
{
	return "Demons and undead killed: " + dao::Utf8(GetHeroicStatValue(hero_stat::GROUPTYPE_DEMONS));
}

std::string STAT_GrouptypeFantastical()
{
	return "Fantastic creatures killed: " + dao::Utf8(GetHeroicStatValue(hero_stat::GROUPTYPE_FANTASTICAL));
}
