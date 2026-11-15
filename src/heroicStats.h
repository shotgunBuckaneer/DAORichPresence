#pragma once

#include <string>

//DAO consts
// Stat constants - Individual
namespace hero_stat
{
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

std::string STAT_Kills();
