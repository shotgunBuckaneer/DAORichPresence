# DAORichPresence
A DAOLib Plugin to display the player's status in Dragon Age: Origins to Discord Rich Presence.

By default, the plugin keeps track of who the player character is (their name, class and level), where they are and what they are currently doing in the game, displaying it to Discord Rich Presence; changes are updated in near-real-time by listening to in-game events.

Requires [DAOLib](https://github.com/Whocares66-Dev/DAOLib/tags) by Whocares66.
Do install, place the richpresence.dll file into the installation folder […]Dragon Age\bin_ship\daolib\plugins\richpresence\.

Should support any of the official game versions from EA, Gog or Steam that are also supported by DAOLib itself.

# Known limitations
* Event timing might make it so the Game Mode "GUI", with GUI type STORE, is not successfully recognized at all times. In such a case, it will default back to "In a menu".
* Once in a menu, further GUI changes are not registered; this is owing to the way in which GUI and Game Mode changes are independently listened to.
