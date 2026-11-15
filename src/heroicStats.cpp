#include "heroicStats.h"

#include "dao/generated/ScriptApi.h"
#include "dao/String.h"

std::string STAT_Kills()
{
	auto oHero = dao::script::api::GetHero();
	float fResult = dao::script::api::GetCreatureProperty(oHero, hero_stat::KILLS);
	int iResult = dao::script::api::FloatToInt(fResult);
	auto sResult = dao::script::api::IntToString(iResult);
	return "Kills: " + dao::Utf8(sResult);
}
