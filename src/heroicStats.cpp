#include "heroicStats.h"

#include "dao/generated/ScriptApi.h"
#include "dao/String.h"
#include "dao/Tlk.h"

#include <iomanip>
#include <sstream>
#include <array>
#include <random>

//DAO consts
namespace hero_stat
{
	// Stat constants - Individual
	inline constexpr int KILLS = 1001;
	inline constexpr int TOTAL_DAMAGE_DEALT = 1002;
	inline constexpr int FRIENDLY_FIRE_DEALT = 1003;
	inline constexpr int HIGHEST_DAMAGE_DEALT = 1004;
	inline constexpr int TOTAL_DAMAGE_PERCENT = 1005;
	inline constexpr int HIT_RATE = 1006;
	inline constexpr int MOST_POWERFUL_SLAIN = 1007;
	inline constexpr int INJURIES = 1008;

	// Stat constants - Party
	inline constexpr int PARTY_KILLS = 2001;
	inline constexpr int PARTY_DAMAGE_DEALT = 2002;
	inline constexpr int MOST_POWERFUL_SLAIN_BY_PARTY = 2003;
	inline constexpr int GROUPTYPE_DEMONS = 2004;
	inline constexpr int GROUPTYPE_FANTASTICAL = 2005;
	inline constexpr int GROUPTYPE_CONSTRUCT = 2006;
	inline constexpr int GROUPTYPE_HUMANOIDS = 2007;
	inline constexpr int GROUPTYPE_DARKSPAWNS = 2008;
	inline constexpr int GROUPTYPE_ARCH_ENEMIES = 2009;
	inline constexpr int GROUPTYPE_DRACONICS = 2010;
	inline constexpr int GROUPTYPE_CORRUPTED_ANIMALS = 2011;
	inline constexpr int GROUPTYPE_VERMIN = 2012;
	inline constexpr int GROUPTYPE_ANIMALS = 2013;
	inline constexpr int STEALING_SUCCESS = 2015;
	inline constexpr int STEALING_FAILURE = 2016;
	inline constexpr int MOST_MONEY_EVER = 2017;
	inline constexpr int MONEY_SPENT = 2018;
	inline constexpr int DIALOGUES = 2019;
	inline constexpr int CODEX_ENTRIES = 2020;
	inline constexpr int WALKED_DISTANCE_MILES = 2021;
	inline constexpr int AREAS_EXPLORED = 2022;
	inline constexpr int TOTAL_TIME_PLAYED = 2023;
	inline constexpr int WALKED_DISTANCE_METERS = 2028;
	inline constexpr int ITEMS_CRAFTED = 2031;
}

//implementations of helpers that don't need to be global
namespace
{
	// Retrieve stats, round to two digits past decimal based on if we need decimal points or not
	std::wstring GetHeroicStatValue(int iValue)
	{
		auto oHero = dao::script::api::GetHero();
		float fResult = dao::script::api::GetCreatureProperty(oHero, iValue);
		const bool bKeepDecimals =	iValue == hero_stat::WALKED_DISTANCE_METERS ||
									iValue == hero_stat::WALKED_DISTANCE_MILES ||
									iValue == hero_stat::TOTAL_DAMAGE_PERCENT ||
									iValue == hero_stat::HIT_RATE ||
									iValue == hero_stat::AREAS_EXPLORED;

		//preconvert meters to kilometers
		if (iValue == hero_stat::WALKED_DISTANCE_METERS)
		{
			fResult = fResult / 1000.0f;
		}

		return dao::script::api::FloatToString(fResult, 18, bKeepDecimals ? 2 : 0);
	}

	//A helper to convert playtime into sensible hh:mm:ss display, as playtime is recorded in seconds
	std::string FormatDuration(float fSeconds)
	{
		auto totalSeconds = static_cast<unsigned long long>(fSeconds);

		auto hours		= totalSeconds / 3600;
		auto minutes	= (totalSeconds % 3600) / 60;
		auto seconds	= totalSeconds % 60;

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
		return "Contribution to party damage: " + dao::Utf8(GetHeroicStatValue(hero_stat::TOTAL_DAMAGE_PERCENT)) + "%";
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

	std::string STAT_WalkedDistanceKilometers()
	{
		return "Kilometers traveled: " + dao::Utf8(GetHeroicStatValue(hero_stat::WALKED_DISTANCE_METERS));
	}

	std::string STAT_ItemsCrafted()
	{
		return "Items crafted: " + dao::Utf8(GetHeroicStatValue(hero_stat::ITEMS_CRAFTED));
	}

	//array for randomizer as function pointers
	std::array<std::string(*)(), 32> s_stats{
	&STAT_Kills,
	&STAT_TotalDamageDealt,
	&STAT_FriendlyFireDealt,
	&STAT_HighestDamageDealt,
	&STAT_TotalDamagePercent,
	&STAT_HitRate,
	&STAT_MostPowerfulSlain,
	&STAT_Injuries,
	&STAT_PartyKills,
	&STAT_PartyDamageDealt,
	&STAT_MostPowerfulSlainByParty,
	&STAT_GrouptypeDemons,
	&STAT_GrouptypeFantastical,
	&STAT_GrouptypeConstruct,
	&STAT_GrouptypeHumanoids,
	&STAT_GrouptypeDarkspawn,
	&STAT_GrouptypeArchEnemies,
	&STAT_GrouptypeDraconics,
	&STAT_GrouptypeCorruptedAnimals,
	&STAT_GrouptypeVermin,
	&STAT_GrouptypeAnimals,
	&STAT_StealingSuccess,
	&STAT_StealingFailure,
	&STAT_MostMoneyEver,
	&STAT_MoneySpent,
	&STAT_Dialogues,
	&STAT_CodexEntries,
	&STAT_WalkedDistanceMiles,
	&STAT_AreasExplored,
	&STAT_TotalTimePlayed,
	&STAT_WalkedDistanceKilometers,
	&STAT_ItemsCrafted
	};

}

//pulls one random Heroic Stat.
std::string STAT_Random()
{
	static std::mt19937 rng{ std::random_device{}() };

	std::uniform_int_distribution<std::size_t> distribution{
		0, s_stats.size() - 1
	};

	return s_stats[distribution(rng)]();
}
