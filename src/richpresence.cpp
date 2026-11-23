/*The goal of this DAOLib plugin is to collect and transmit data to Discord's Rich Presence feature.
It can very reliably capture Player Name, Class, Level, location, and activity based on Game- and GUI
modes.*/

#include <string>
#include <algorithm>
#include <array>

#include "dao/Log.h"
#include "dao/Plugin.h"
#include "dao/Events.h"
#include "dao/GameModes.h"
#include "dao/Objects.h"
#include "dao/World.h"
#include "dao/generated/ScriptApi.h"
#include "dao/String.h"
#include "dao/Tlk.h"

#include "discord_rpc.h"

#include "heroicStats.h"

//DAO consts
//GUI
constexpr int GUI_CHARACTER_RECORD = 0;
constexpr int GUI_CONTAINER_INVENTORY = 2;
constexpr int GUI_INVENTORY = 8;
constexpr int GUI_JOURNAL = 9;
constexpr int GUI_SPELLS_TALENTS = 10;
constexpr int GUI_SKILLS = 13;
constexpr int GUI_MAP = 15;
constexpr int GUI_WORLD_MAP = 16;
constexpr int GUI_MAIN_MENU = 20;
constexpr int GUI_CHANTERS = 22;
constexpr int GUI_TACTICS = 23;
constexpr int GUI_FADE_MAP = 31;
constexpr int GUI_STORE = 32;
constexpr int GUI_ITEM_UPGRADE = 37;
constexpr int GUI_CRAFTING = 38;
constexpr int GUI_SLIDESHOW = 48;

/*
* Shelved. See ANNOTATION1 below.
//terrains
namespace terrain
{
    constexpr int INVALID = 0;
    constexpr int Mountains = 1;
    constexpr int Forest = 2;
    constexpr int Underground = 3;
    constexpr int Plains = 4;
    constexpr int Highway = 5;
    constexpr int DenerimClimaxNormal = 6;
    constexpr int DenerimClimaxPalace = 7;
    constexpr int Denerim = 8;
}
*/

//strings as global variable for transport from dllexport to GameMode switch
std::string DialogueInitiator{};
std::string LevelUpTargetName{};

//integer as global variable for transport from dllexport to guiType switch; initialized as -1 because
// 0 is a perfectly valid result; GUI_CHARACTER_RECORD.
int guiMode{-1};
//ditto for Chargen mode
int ChargenMode{-1};

//saving Class as a variable for the Small Icon update
int iPCClass{};

//GUI array for checking against in the GUI mode determination
static constexpr std::array<int, 16> s_variants{
    GUI_CHARACTER_RECORD,
    GUI_CONTAINER_INVENTORY,
    GUI_INVENTORY,
    GUI_JOURNAL,
    GUI_SPELLS_TALENTS,
    GUI_SKILLS,
    GUI_MAP,
    GUI_WORLD_MAP,
    GUI_MAIN_MENU,
    GUI_CHANTERS,
    GUI_TACTICS,
    GUI_FADE_MAP,
    GUI_STORE,
    GUI_ITEM_UPGRADE,
    GUI_CRAFTING,
    GUI_SLIDESHOW
};

bool IsRelevantGUI(int mode)
{
    return std::find(s_variants.begin(), s_variants.end(), mode) != s_variants.end();
}

// retrieve player character name from Token
std::string PCName()
{
    auto sPCName = dao::tlk::Resolve(L"<FullName/>");
    return dao::Utf8(sPCName);
}

// Identify PC class via NWScript, return class string
std::string PCClass()
{
    auto oHero  = dao::script::api::GetHero();
    iPCClass    = dao::script::api::FloatToInt(dao::script::api::GetCreatureProperty(oHero, 27));

    switch (iPCClass)
    {
    case 1:
        return "Warrior";

    case 2:
        return "Mage";

    case 3:
        return "Rogue";

    default:
        return "Unknown class";

    }

}

// Retrieve player level in NWScript
std::string PCLevel()
{
    auto oHero      = dao::script::api::GetHero();
    auto fLevel     = dao::script::api::FloatToString(dao::script::api::GetCreatureProperty(oHero, 15, 1), 2, 0);
    return dao::Utf8(fLevel);
}

namespace
{
    // Action for both kinds of PC detection on AreaLoad events
    void PCAction(DiscordRichPresence& presence, std::string& sName, std::string& sClassAndLevel, std::string& sHeroicStat)
    {
        //to avoid call redundancy
        const auto sPCName = PCName();
        const auto sPCClass = PCClass();
        const auto sPCLevel = PCLevel();

        spdlog::debug("Hero: {}, Class: {}, Level {}", sPCName, sPCClass, sPCLevel);
        spdlog::debug("Area change registered");

        sName = sPCName;
        sClassAndLevel = sPCClass + ", Level " + sPCLevel;
        sHeroicStat = STAT_Random();
        presence.details = sName.c_str();
        presence.smallImageText = sClassAndLevel.c_str();
        presence.largeImageText = sHeroicStat.c_str();
        presence.largeImageKey = STAT_ImageKey;
        switch (iPCClass)
        {
        case 1:
            presence.smallImageKey = "classico_warrior";
            break;
        case 2:
            presence.smallImageKey = "classico_mage";
            break;
        case 3:
            presence.smallImageKey = "classico_rogue";
            break;
        default:
            presence.smallImageKey = "secret";
        }
        Discord_UpdatePresence(&presence);
    }

}

// Identify current area in NWScript
std::string AreaName()
{
    auto oHero      = dao::script::api::GetHero();
    auto oArea      = dao::script::api::GetArea(oHero);
    auto sName      = dao::script::api::GetName(oArea);
    return dao::Utf8(sName);
}

/* 
* Shelved. See ANNOTATION1 below.
// returns Terrain-based string for Worldmap travel information
std::string TerrainAttachment(int nTargetTerrain)
{
    switch (nTargetTerrain)
    {
    case terrain::Mountains:
        return ", traversing the mountains";

    case terrain::Forest:
        return ", going through the thick forest";

    case terrain::Underground:
        return ", braving the Deep Roads";

    case terrain::Plains:
        return ", crossing over the plains";

    case terrain::Highway:
        return ", using the Imperial Highway";

    case terrain::DenerimClimaxNormal:
        return ", fighting through the streets of Denerim";

    case terrain::DenerimClimaxPalace:
        return ", the fighting is getting more intense";

    case terrain::Denerim:
        return ", winding through the many alleys of Denerim";

    default:
        return {};
    }
}
*/

//identifying only relevant GUI types as defined in NWScript's script.ldf
std::string guiType()
{
    switch (guiMode)
    {
    case GUI_CHARACTER_RECORD:
        return "Examining a character record";

    case GUI_CONTAINER_INVENTORY:
        return "Rummaging through a container";

    case GUI_INVENTORY:
        return "In the Inventory";

    case GUI_JOURNAL:
        return "Checking the Journal";

    case GUI_SPELLS_TALENTS:
        return "Checking Abilities";

    case GUI_SKILLS:
        return "Checking Skills";

    case GUI_MAP:
        return "Examining the local Map of " + AreaName();

    case GUI_WORLD_MAP:
        return "Examining the Wide Open World Map";

    case GUI_MAIN_MENU:
        return "In the Main Menu";
        
    case GUI_CHANTERS:
        return "Checking a Quest Board in " + AreaName();

    case GUI_TACTICS:
        return "Checking on Party Tactics";

    case GUI_FADE_MAP:
        return "Charting the Fade";

    // at present, it seems that there's a race going on when a merchant window is launched from within
    // a dialogue, and the GameMode GUI event wins before the GUI change is registered. Don't really
    // have a good workaround for that just yet, so often, it will run to the default case instead.
    case GUI_STORE:
        return "Trading in " + AreaName();

    case GUI_ITEM_UPGRADE:
        return "Enchantment!";

    case GUI_CRAFTING:
        return "Crafting Items";

    case GUI_SLIDESHOW:
        return "The end...?";

    default:
        return "In a menu";
    }
}

//as GM Chargen can be either actual creation or level up, we need to determine which
std::string ChargenType()
{
    switch (ChargenMode)
    {
    case 0:
        return "In Character Creation";

    //just in case we somehow end up with an empty name string...
    case 1:
        return LevelUpTargetName.empty() 
        ? "Leveling up" 
        : "Leveling " + LevelUpTargetName + " up";

    default:
        //we don't know which state we are in if this happens, best to keep the description vague
        return "Shaping characters...";
    }
}

// grab enum results of GameMode, return legible text explaining the status
std::string GameModeName(dao::GameMode mode)
{

    static std::string RetainedGM{"Unknown"};

    switch (mode)
    {
    case dao::GameMode::Explore:
        //we just-in-case clear the DialogueInitiator here to avoid any rare carry-over cases when the
        //next dialogue comes around out of order
        DialogueInitiator.clear();
        RetainedGM = "Exploring " + AreaName();
        break;

    case dao::GameMode::Combat:
        //ditto
        DialogueInitiator.clear();
        RetainedGM = "Fighting in " + AreaName();
        break;

    case dao::GameMode::Dead:
        RetainedGM = "Defeated";
        break;
    
    //GUI should now be handled by its own function, but I am keeping this intact just in case something
    //manages to slip through and creates a false default state.
    case dao::GameMode::Gui:
        RetainedGM = guiType();
        break;

    case dao::GameMode::Dialog:
        //ensure returned owner string isn't empty, include name if not
        RetainedGM = DialogueInitiator.empty()
            ? "In a conversation" 
            : "Talking with " + DialogueInitiator;
        break;

    case dao::GameMode::Cutscene:
        RetainedGM = "In a cutscene";
        break;

    case dao::GameMode::FlyCam:
        RetainedGM = "Using the Flycam";
        break;

    //No one seems to know what this IS. Logging it as a warn in the unlikely event it is ever stumbled
    //upon.
    case dao::GameMode::Fixed:
        spdlog::warn("Encountered state Fixed");
        RetainedGM = "Fixed(?)";
        break;

    case dao::GameMode::Pregame:
        RetainedGM = "On the Title Screen";
        break;

    //Don't want Loading to return anything, as the number of load-screens would just flood feeds.
    case dao::GameMode::Loading:
        spdlog::debug("Loading, not for productive logging");
        return RetainedGM;

    case dao::GameMode::Movie:
        RetainedGM = "In a cutscene";
        break;

    case dao::GameMode::CharGen:
        RetainedGM = ChargenType();
        break;

    case dao::GameMode::PartyPicker:
        RetainedGM = "Selecting Party members";
        break;

    default:
        RetainedGM = "Unknown";
        spdlog::info("Unidentified GameMode encountered.");
    }

    return RetainedGM;
}

extern "C" __declspec(dllexport) bool __cdecl DAOLib_Load(const dao::LoaderInfo *info)
{
    //plugin initialization
    if (info == nullptr || info->apiVersion != dao::kPluginApiVersion || !info->knownExecutable)
    {
        spdlog::error("Mismatch between plugin build, loader and/or game");
        return false; // not the loader or the game this was built for
    }

    #ifdef _DEBUG
        spdlog::set_level(spdlog::level::debug);
    #endif // _DEBUG

        
        
    dao::OpenLog("richpresence"); // Documents\BioWare\Dragon Age\daolib\richpresence.log
    spdlog::info("Plugin loaded successfully");

    //Handlers aren't really needed at this point but the Initialization requires them as an argument
    //empty handler struct might let us bypass
    DiscordEventHandlers handlers{};

    //Initialization of Discord RPC
    Discord_Initialize("1570102592795840763", &handlers, 1, nullptr);

    //initializing the struct to fill it as needed
    static DiscordRichPresence presence{};

    //establishing string variables
    static std::string sName{};
    static std::string sClassAndLevel{};
    static std::string sHeroicStat{};
    static std::string sGameMode{};
    static std::string sGuiMode{};
    //static std::string sAreaName{};

    //Initializing first status message
    presence.state = "Starting the game...";
    Discord_UpdatePresence(&presence);


    //Transition from load into area, logging thereof, registration of Hero to RPC, updating of class
    //icon
    dao::events::OnScriptEvent(dao::script::EventType::ArealoadsavePreloadexit,
                              [](dao::ObjectId, const dao::CScriptEvent&) 
                              {     
                                    PCAction(presence, sName, sClassAndLevel, sHeroicStat);
                              });
    
    //Area (transition) identification, logging thereof, registration of Hero to RPC
    dao::events::OnScriptEvent(dao::script::EventType::ArealoadPreloadexit, 
                              [](dao::ObjectId, const dao::CScriptEvent&) 
                              { 
                                    PCAction(presence, sName, sClassAndLevel, sHeroicStat);
                              });

    //Safety check on level
    dao::events::OnScriptEvent(dao::script::EventType::Playerlevelup, 
                              [](dao::ObjectId, const dao::CScriptEvent&) 
                              {
                                   spdlog::debug("Extra check on level after level up event: Level {}", PCLevel());
                                   sClassAndLevel = PCClass() + ", Level " + PCLevel();
                                   presence.smallImageText = sClassAndLevel.c_str();
                                   Discord_UpdatePresence(&presence);
                              });

    // Identify dialogue event firing creature/placeable, including name as reported by NWScript
    dao::events::OnScriptEvent(dao::script::EventType::Dialogue,
                              [](dao::ObjectId target, const dao::CScriptEvent&)
                              {
                                    auto sName          = dao::script::api::GetName(target);
                                    DialogueInitiator   = dao::Utf8(sName);
                                    spdlog::debug("Dialogue event registered");
                              });

    //detail GUI changes, update if needed
    dao::events::OnScriptEvent(dao::script::EventType::GuiOpened, 
                              [](dao::ObjectId, const dao::CScriptEvent &event) 
                              {
                                    guiMode = event.Integer(0);
                                    auto iGM = dao::script::api::GetGameMode();
                                    //Double check that our  GUI switch is relevant AND that we're in GameMode GUI)
                                    if (IsRelevantGUI(guiMode) && iGM == 3)
                                    {
                                        sGuiMode = guiType();
                                        presence.state = sGuiMode.c_str();
                                        Discord_UpdatePresence(&presence);
                                    }
                                    //commented logging out, we don't need to log every one of these,
                                    //the log would very quickly swell.
                                    //spdlog::debug("GUI change detected: {}", guiMode);
                              });

    //capture Chargen mode and target
    dao::events::OnScriptEvent(dao::script::EventType::ChargenStart, 
                              [](dao::ObjectId, const dao::CScriptEvent &event) 
                              { 
                                    //just in case, re-clear the LevelUpTargetName variable to avoid
                                    //issues with false carry-overs and/or failed lookups
                                    if (!LevelUpTargetName.empty())
                                    {
                                        LevelUpTargetName.clear();
                                    }
                                    ChargenMode         = event.Integer(0);

                                    if (dao::script::api::IsObjectValid(event.Object(0)))
                                    {
                                        LevelUpTargetName = dao::Utf8(dao::script::api::GetName(event.Object(0)));
                                    }
                                                                        
                              });

    /*
    * ANNOTATION1:
    * Shelving this for now. The Destination is, regardless of approach, not accessible as it is not 
    * loaded yet.

    //Capturing World Map transitions for display on state.
    dao::events::OnScriptEvent(dao::script::EventType::BeginTravel,
                              [](dao::ObjectId, const dao::CScriptEvent &event)
                              {
                                    auto sSourceTag = event.String(0);
                                    auto sAreaTag   = event.String(1);

                                    spdlog::info("BeginTravel: {} -> {}",
                                    dao::Utf8(sSourceTag), dao::Utf8(sAreaTag));

                                    //Sanity check: are we trying to travel to where we already are?
                                    if (sSourceTag != sAreaTag)
                                    {
                                        auto oArea = dao::script::api::GetObjectByTag(sAreaTag);

                                        spdlog::info("Destination object valid: {}",
                                        dao::script::api::IsObjectValid(oArea));

                                        //Sanity check: Does the destination actually exist?
                                        if (dao::script::api::IsObjectValid(oArea))
                                        {
                                            sAreaName = "Travelling to " + dao::Utf8(dao::script::api::GetName(oArea)) + TerrainAttachment(event.Integer(1)) + "...";

                                            spdlog::info("Prepared travel string: {}", sAreaName);
                                        }

                                    }

                              });
    //split the actual string execution and passing off the buck off to RetainedGM on the Worldmap Pretransition
    //event due to timing concerns.
    //In testing, just using RetainedGM didn't work, so we'll try updating the state from right in here,
    //instead.
    dao::events::OnScriptEvent(dao::script::EventType::WorldmapPretransition,
                              [](dao::ObjectId, const dao::CScriptEvent&){
                                    RetainedGM = sAreaName;
                                    presence.state = RetainedGM.c_str();
                                    Discord_UpdatePresence(&presence);

                                    spdlog::info("Pretransition received. Stored area name: '{}'", sAreaName);

                                    spdlog::debug("Travel: {}", sAreaName);
                              });

*/

    //register and log game mode change
    dao::events::OnScriptEvent(dao::script::EventType::GamemodeChange,
                               [](dao::ObjectId, const dao::CScriptEvent &event) 
                               {
                                   //GUI is handled by its own event
                                   if (event.Integer(0) != 3)
                                   {
                                       const auto newMode = static_cast<dao::GameMode>(event.Integer(0));
                                       sGameMode = GameModeName(newMode);
                                       presence.state = sGameMode.c_str();
                                       Discord_UpdatePresence(&presence);

                                       spdlog::debug("Game mode changed: {}", sGameMode);
                                   }

                               });
    return true;
}
