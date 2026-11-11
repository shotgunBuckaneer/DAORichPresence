/*The goal of this DAOLib plugin is to collect and transmit data to Discord's Rich Presence feature.
It can very reliably capture Player Name, Class, Level, location, and activity based on Game- and GUI
modes.*/

#include <string>

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

//strings as global variable for transport from dllexport to GameMode switch
std::string DialogueInitiator{};
std::string LevelUpTargetName{};

//integer as global variable for transport from dllexport to guiType switch; initialized as -1 because
// 0 is a perfectly valid result; GUI_CHARACTER_RECORD.
int guiMode{-1};
//ditto for Chargen mode
int ChargenMode{-1};

// retrieve player character name from Token
std::string PCName()
{
    auto sPCName = dao::tlk::Resolve(L"<FullName/>");
    return dao::Utf8(sPCName);
}

// Identify PC class via NWScript, return class string
std::string PCClass()
{
    auto oHero      = dao::script::api::GetHero();
    auto iPCClass   = dao::script::api::FloatToInt(dao::script::api::GetCreatureProperty(oHero, 27));

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
    auto iLevel     = dao::script::api::FloatToInt(dao::script::api::GetCreatureProperty(oHero, 15, 1));
    //roundabout conversion from Float, the type all properties are, to int so the value becomes truncated;
    //then to wstring, and finally to UTF8 for use in the "real world".
    auto sLevel     = dao::script::api::IntToString(iLevel);
    return dao::Utf8(sLevel);
}

// Logging action for both kinds of PC detection
void PCAction()
{
    spdlog::info("Hero: {}, Class: {}, Level {}", PCName(), PCClass(), PCLevel());
    spdlog::info("Area change registered");
}

// Identify area in NWScript
std::string AreaName()
{
    auto oHero      = dao::script::api::GetHero();
    auto oArea      = dao::script::api::GetArea(oHero);
    auto sName      = dao::script::api::GetName(oArea);
    return dao::Utf8(sName);
}

//identifying only relevant GUI types as defined in NWScript's script.ldf
std::string guiType()
{
    switch (guiMode)
    {
    case 0:
        return "Examining a character record";

    case 2:
        return "Rummaging through a container";

    case 8:
        return "In the Inventory";

    case 9:
        return "Checking the Journal";

    case 10:
        return "Checking Abilities";

    case 13:
        return "Checking Skills";

    case 15:
        return "Examining the local Map";

    case 16:
        return "Examining the Wide Open World Map";

    case 20:
        return "In the Main Menu";
        
    case 22:
        return "Checking a Quest Board in " + AreaName();

    case 23:
        return "Checking on Party Tactics";

    case 31:
        return "Charting the Fade";

    // at present, it seems that there's a race going on when a merchant window is launched from within
    // a dialogue, and the GameMode GUI event wins before the GUI change is registered. Don't really
    // have a good workaround for that just yet, so often, it will run to the default case instead.
    case 32:
        return "Trading in " + AreaName();

    case 37:
        return "Enchantment!";

    case 38:
        return "Crafting Items";

    case 48:
        return "The end...?";

    default:
        return "In a menu";
    }
}

//as GUI Chargen can be either actual creation or level up, we need to determine which
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
    
    //GUI is very broad, needs narrowing down
    //known and accepted caveat: This will not update the information if changing from one menu to another;
    //on the plus side, the Discord API will be unlikely to then reject too many information bursts
    //as spam or malicious if all a player does is quickly go through menus.
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
        
        
    dao::OpenLog("richpresence"); // Documents\BioWare\Dragon Age\daolib\richpresence.log
    spdlog::info("Plugin loaded successfully");

    //Handlers aren't really needed at this point but the Initialization requires them as an argument
    //empty handler struct might let us bypass
    DiscordEventHandlers handlers{};

    //Initialization of Discord RPC
    Discord_Initialize("1570102592795840763", &handlers, 1, nullptr);

    //Purely a test case: Try and send something at all
    //initializing the struct to fill it as needed
    DiscordRichPresence presence{};

    presence.details    = "Testing DAORichPresence plugin";
    presence.state      = "Exploring Thedas";

    Discord_UpdatePresence(&presence);

    //Transition from load into area, logging thereof
    dao::events::OnScriptEvent(dao::script::EventType::ArealoadsavePreloadexit,
                              [](dao::ObjectId, const dao::CScriptEvent&) 
                              {     
                                    PCAction();
                              });
    
    //Area (transition) identification, logging thereof
    dao::events::OnScriptEvent(dao::script::EventType::ArealoadPreloadexit, 
                              [](dao::ObjectId, const dao::CScriptEvent&) 
                              { 
                                    PCAction();
                              });

    //Safety check on level
    dao::events::OnScriptEvent(dao::script::EventType::Playerlevelup, 
                              [](dao::ObjectId, const dao::CScriptEvent&) 
                              {
                                   spdlog::info("Extra check on level after level up event: Level {}", PCLevel());
                              });

    // Identify dialogue event firing creature/placeable, including name as reported by NWScript

    dao::events::OnScriptEvent(dao::script::EventType::Dialogue,
                              [](dao::ObjectId target, const dao::CScriptEvent&)
                              {
                                    auto sName          = dao::script::api::GetName(target);
                                    DialogueInitiator   = dao::Utf8(sName);
                                    spdlog::info("Dialogue event registered");
                              });

    //detail GUI changes
    dao::events::OnScriptEvent(dao::script::EventType::GuiOpened, 
                              [](dao::ObjectId, const dao::CScriptEvent &event) 
                              {
                                    guiMode = event.Integer(0);
                                    //commented logging out, we don't need to log every one of these,
                                    //the log would very quickly swell.
                                    //spdlog::info("GUI change detected: {}", guiMode);
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

    //register and log game mode change
    dao::events::OnScriptEvent(dao::script::EventType::GamemodeChange,
                               [](dao::ObjectId, const dao::CScriptEvent &event) 
                               { 
                                   const auto newMode = static_cast<dao::GameMode>(event.Integer(0));

                                   spdlog::info("Game mode changed: {}", 
                                                GameModeName(newMode));
        
                               });
    return true;
}