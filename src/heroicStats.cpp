#include "heroicStats.h"

#include "dao/generated/ScriptApi.h"
#include "dao/String.h"
#include "dao/Tlk.h"

#include <iomanip>
#include <sstream>

// Retrieve stats, truncate based on if we need decimal points or not
std::wstring GetHeroicStatValue(int iValue)
{
	auto oHero = dao::script::api::GetHero();
	float fResult = dao::script::api::GetCreatureProperty(oHero, iValue);
	const bool bKeepDecimals =	iValue == hero_stat::WALKED_DISTANCE_METERS ||
								iValue == hero_stat::WALKED_DISTANCE_MILES ||
								iValue == hero_stat::TOTAL_DAMAGE_PERCENT ||
								iValue == hero_stat::HIT_RATE ||
								iValue == hero_stat::AREAS_EXPLORED;

	return dao::script::api::FloatToString(fResult, 18, bKeepDecimals ? 2 : 0);
}

//A helper to convert playtime into sensible hh:mm:ss display, as playtime is recorded in seconds
std::string FormatDuration(float fSeconds)
{
	auto totalSeconds = static_cast<unsigned long long>(fSeconds);

	auto hours		= totalSeconds / 3600;
	auto minutes	= (totalSeconds % 3600) / 60;
	auto seconds	= totalSeconds & 60;

	std::ostringstream stream;
	stream	<< std::setfill('0')
			<< std::setw(2) << hours << ':'
			<< std::setw(2) << minutes << ':'
			<< std::setw(2) << seconds;

	return stream.str();
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

std::string STAT_GrouptypeConstruct()
{
	return "Golems killed: " + dao::Utf8(GetHeroicStatValue(hero_stat::GROUPTYPE_CONSTRUCT));
}

std::string STAT_GrouptypeHumanoids()
{
	return "People killed: " + dao::Utf8(GetHeroicStatValue(hero_stat::GROUPTYPE_HUMANOIDS));
}

std::string STAT_GrouptypeDarkspawn()
{
	return "Darkspawn killed: " + dao::Utf8(GetHeroicStatValue(hero_stat::GROUPTYPE_DARKSPAWNS));
}

std::string STAT_GrouptypeArchEnemies()
{
	return "Bosses killed: " + dao::Utf8(GetHeroicStatValue(hero_stat::GROUPTYPE_ARCH_ENEMIES));
}

std::string STAT_GrouptypeDraconics()
{
	return "Dragonkin killed: " + dao::Utf8(GetHeroicStatValue(hero_stat::GROUPTYPE_DRACONICS));
}

std::string STAT_GrouptypeCorruptedAnimals()
{
	return "Corrupted animals killed: " + dao::Utf8(GetHeroicStatValue(hero_stat::GROUPTYPE_CORRUPTED_ANIMALS));
}

std::string STAT_GrouptypeVermin()
{
	return "Vermin killed: " + dao::Utf8(GetHeroicStatValue(hero_stat::GROUPTYPE_VERMIN));
}

std::string STAT_GrouptypeAnimals()
{
	return "Dangerous animals killed: " + dao::Utf8(GetHeroicStatValue(hero_stat::GROUPTYPE_ANIMALS));
}

std::string STAT_StealingSuccess()
{
	return "Stealing successes: " + dao::Utf8(GetHeroicStatValue(hero_stat::STEALING_SUCCESS));
}

std::string STAT_StealingFailure()
{
	return "Stealing failures: " + dao::Utf8(GetHeroicStatValue(hero_stat::STEALING_FAILURE));
}

std::string STAT_MostMoneyEver()
{
	return "Most money at once: " + dao::Utf8(GetHeroicStatValue(hero_stat::MOST_MONEY_EVER));
}

std::string STAT_MoneySpent()
{
	return "Money spent: " + dao::Utf8(GetHeroicStatValue(hero_stat::MONEY_SPENT));
}

std::string STAT_Dialogues()
{
	return "Conversations initiated: " + dao::Utf8(GetHeroicStatValue(hero_stat::DIALOGUES));
}

std::string STAT_CodexEntries()
{
	return "Codex entries discovered: " + dao::Utf8(GetHeroicStatValue(hero_stat::CODEX_ENTRIES));
}

std::string STAT_WalkedDistanceMiles()
{
	return "Miles traveled: " + dao::Utf8(GetHeroicStatValue(hero_stat::WALKED_DISTANCE_MILES));
}

std::string STAT_AreasExplored()
{
	return "World explored: " + dao::Utf8(GetHeroicStatValue(hero_stat::AREAS_EXPLORED)) + "%";
}

std::string STAT_TotalTimePlayed()
{
	auto oHero = dao::script::api::GetHero();
	auto fResult = dao::script::api::GetCreatureProperty(oHero, hero_stat::TOTAL_TIME_PLAYED);

	return "Time played: " + FormatDuration(fResult);
}
