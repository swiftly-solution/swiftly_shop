#include <swiftly/swiftly.h>
#include <swiftly/server.h>
#include <swiftly/database.h>
#include <swiftly/commands.h>
#include <swiftly/configuration.h>
#include <swiftly/logger.h>
#include <swiftly/timers.h>
#include <swiftly/gameevents.h>

Server *server = nullptr;
PlayerManager *g_playerManager = nullptr;
Database *db = nullptr;
Commands *commands = nullptr;
Configuration *config = nullptr;
Logger *logger = nullptr;
Timers *timers = nullptr;

void OnProgramLoad(const char *pluginName, const char *mainFilePath)
{
    Swiftly_Setup(pluginName, mainFilePath);

    server = new Server();
    g_playerManager = new PlayerManager();
    commands = new Commands(pluginName);
    config = new Configuration();
    logger = new Logger(mainFilePath, pluginName);
    timers = new Timers();
}

void Command_Credits(int playerID, const char **args, uint32_t argsCount, bool silent)
{
     if (playerID == -1)
        return;
        
    if (!db->IsConnected())
        return;

        Player *player = g_playerManager->GetPlayer(playerID);

        if (player == nullptr)
        return;

    DB_Result result = db->Query("select * from `players_credits` where steamid = '%llu' limit 1", player->GetSteamID());

    player->SendMsg(HUD_PRINTTALK, "%s You have %d credits.", config->Fetch<const char*>("swiftly_shop.prefix"), (db->fetchValue<int>(result, 0, "credits")));
}

void OnPlayerSpawn(Player *player)
{
        if (!db->IsConnected())
            return;

        if (player->IsFirstSpawn() && !player->IsFakeClient())
            db->Query("insert ignore into `players_credits` (steamid) values ('%llu')", player->GetSteamID());
}

void OnPlayerDeath(Player *player, Player *attacker, Player *assister, bool assistedflash, const char *weapon, bool headshot, short dominated, short revenge, short wipe, short penetrated, bool noreplay, bool noscope, bool thrusmoke, bool attackerblind, float distance, short dmg_health, short dmg_armor, short hitgroup)
{
    DB_Result result = db->Query("SELECT credits FROM %s WHERE steamid = '%llu' LIMIT 1", "players_credits", player->GetSteamID());
    int currentCredits = 0;
    if(result.size() > 0) {
        currentCredits = db->fetchValue<int>(result, 0, "credits");
    }

    if(player == attacker) {
        if (currentCredits > 0) {
            char buffer[256];
            int deathCredits = config->Fetch<int>("swiftly_shop.Death");
            sprintf(buffer, FetchTranslation("swiftly_shop.Death"), deathCredits);
            player->SendMsg(HUD_PRINTTALK, "%s %s.", config->Fetch<const char*>("swiftly_shop.prefix"), buffer);
            db->Query("UPDATE %s SET credits = credits - %d WHERE steamid = '%llu'", "players_credits", config->Fetch<int>("swiftly_shop.Death"), player->GetSteamID());
        }
    } else if (headshot && attacker) {
        char buffer[256];
        int headshotCredits = config->Fetch<int>("swiftly_shop.Headshot");
        sprintf(buffer, FetchTranslation("swiftly_shop.Headshot"), headshotCredits);
        attacker->SendMsg(HUD_PRINTTALK, "%s %s.", config->Fetch<const char*>("swiftly_shop.prefix"), buffer);
        db->Query("UPDATE %s SET credits = credits + %d WHERE steamid = '%llu'", "players_credits", headshotCredits, attacker->GetSteamID());
        if (currentCredits > 0) {
            char buffer[256];
            int deathCredits = config->Fetch<int>("swiftly_shop.Death");
            sprintf(buffer, FetchTranslation("swiftly_shop.Death"), deathCredits);
            player->SendMsg(HUD_PRINTTALK, "%s %s.", config->Fetch<const char*>("swiftly_shop.prefix"), buffer);
            db->Query("UPDATE %s SET credits = credits - %d WHERE steamid = '%llu'", "players_credits", config->Fetch<int>("swiftly_shop.Death"), player->GetSteamID());
        }
    } else if (noscope && attacker) {
        char buffer[256];
        int noscopeCredits = config->Fetch<int>("swiftly_shop.Noscope");
        sprintf(buffer, FetchTranslation("swiftly_shop.Noscope"), noscopeCredits);
        attacker->SendMsg(HUD_PRINTTALK, "%s %s.", config->Fetch<const char*>("swiftly_shop.prefix"), buffer);
        db->Query("UPDATE %s SET credits = credits + %d WHERE steamid = '%llu'", "players_credits", noscopeCredits, attacker->GetSteamID());
        if (currentCredits > 0) {
            char buffer[256];
            int deathCredits = config->Fetch<int>("swiftly_shop.Death");
            sprintf(buffer, FetchTranslation("swiftly_shop.Death"), deathCredits);
            player->SendMsg(HUD_PRINTTALK, "%s %s.", config->Fetch<const char*>("swiftly_shop.prefix"), buffer);
            db->Query("UPDATE %s SET credits = credits - %d WHERE steamid = '%llu'", "players_credits", config->Fetch<int>("swiftly_shop.Death"), player->GetSteamID());
        }
    } else if (attacker) {
        char buffer[256];
        int NormalKill = config->Fetch<int>("swiftly_shop.NormalKill");
        sprintf(buffer, FetchTranslation("swiftly_shop.NormalKill"), NormalKill);
        attacker->SendMsg(HUD_PRINTTALK, "%s %s.", config->Fetch<const char*>("swiftly_shop.prefix"), buffer);
        db->Query("UPDATE %s SET credits = credits + %d WHERE steamid = '%llu'", "players_credits", NormalKill, attacker->GetSteamID());
        if (currentCredits > 0) {
            char buffer[256];
            int deathCredits = config->Fetch<int>("swiftly_shop.Death");
            sprintf(buffer, FetchTranslation("swiftly_shop.Death"), deathCredits);
            player->SendMsg(HUD_PRINTTALK, "%s %s.", config->Fetch<const char*>("swiftly_shop.prefix"), buffer);
            db->Query("UPDATE %s SET credits = credits - %d WHERE steamid = '%llu'", "players_credits", config->Fetch<int>("swiftly_shop.Death"), player->GetSteamID());
        }
    }
}


void OnPluginStart()
{
    commands->Register("credits", reinterpret_cast<void *>(&Command_Credits));

    db = new Database("swiftly_shop");
    DB_Result result = db->Query("CREATE TABLE IF NOT EXISTS `players_credits` (`steamid` varchar(128) NOT NULL, `credits` int(11) NOT NULL, `medkits` int(11) NOT NULL DEFAULT 0) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;");
    if (result.size() > 0)
            db->Query("ALTER TABLE `players_credits` ADD UNIQUE KEY `steamid` (`steamid`);");


}

void OnPluginStop()
{
}

const char *GetPluginAuthor()
{
    return "Swiftly Solutions";
}

const char *GetPluginVersion()
{
    return "1.0.0";
}

const char *GetPluginName()
{
    return "swiftly_shop";
}

const char *GetPluginWebsite()
{
    return "https://github.com/swiftly-solution/swiftly_shop";
}