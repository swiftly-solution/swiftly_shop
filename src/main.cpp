#include <swiftly/swiftly.h>
#include <swiftly/server.h>
#include <swiftly/database.h>
#include <swiftly/commands.h>
#include <swiftly/configuration.h>
#include <swiftly/logger.h>
#include <swiftly/timers.h>
#include <swiftly/gameevents.h>
#include <swiftly/menus.h>

Server *server = nullptr;
PlayerManager *g_playerManager = nullptr;
Database *db = nullptr;
Commands *commands = nullptr;
Configuration *config = nullptr;
Logger *logger = nullptr;
Timers *timers = nullptr;
Menus *menus = nullptr;

void OnProgramLoad(const char *pluginName, const char *mainFilePath)
{
    Swiftly_Setup(pluginName, mainFilePath);

    server = new Server();
    g_playerManager = new PlayerManager();
    commands = new Commands(pluginName);
    config = new Configuration();
    logger = new Logger(mainFilePath, pluginName);
    timers = new Timers();
    menus = new Menus(pluginName);
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
    player->HideMenu();
}

void Command_Shop(int playerID, const char **args, uint32_t argsCount, bool silent)
{
     if (playerID == -1)
        return;
        
    if (!db->IsConnected())
        return;

        Player *player = g_playerManager->GetPlayer(playerID);

        if (player == nullptr)
        return;

    player->ShowMenu("shop");
}

void Command_ShopBuyHP(int playerID, const char **args, uint32_t argsCount, bool silent)
{
     if (playerID == -1)
        return;
        
    if (!db->IsConnected())
        return;

        Player *player = g_playerManager->GetPlayer(playerID);

        if (player == nullptr)
        return;

    DB_Result result = db->Query("select * from `players_credits` where steamid = '%llu' limit 1", player->GetSteamID());
    int currentCredits = 0;
    if(result.size() > 0) {
        currentCredits = db->fetchValue<int>(result, 0, "credits");
    }

    int CreditsToBuyHP = config->Fetch<int>("swiftly_shop.CreditsToBuyHP");

    if (currentCredits < CreditsToBuyHP) {
        player->SendMsg(HUD_PRINTTALK, "%s %s.", config->Fetch<const char*>("swiftly_shop.prefix"), FetchTranslation("swiftly_shop.NotEnoughCredits"));
        player->HideMenu();
        return;
    } else {
        db->Query("UPDATE %s SET credits = credits - %d WHERE steamid = '%llu'", "players_credits", CreditsToBuyHP, player->GetSteamID());
        db->Query("UPDATE %s SET HasHPAccess = 1, HasHPActivated = 1 WHERE steamid = '%llu'", "players_features", player->GetSteamID());
        player->SendMsg(HUD_PRINTTALK, "%s %s.", config->Fetch<const char*>("swiftly_shop.prefix"), FetchTranslation("swiftly_shop.BoughtFeature"));
        player->SendMsg(HUD_PRINTTALK, "%s The feature was activated by default.", config->Fetch<const char*>("swiftly_shop.prefix"));
        player->HideMenu();
    }
}

void Command_ShopActivateHP(int playerID, const char **args, uint32_t argsCount, bool silent)
{
     if (playerID == -1)
        return;
        
    if (!db->IsConnected())
        return;

        Player *player = g_playerManager->GetPlayer(playerID);

        if (player == nullptr)
        return;

    DB_Result result = db->Query("select * from `players_features` where steamid = '%llu' limit 1", player->GetSteamID());
    int HasHPActivated = 0;
    int HasHPAccess = 0;
    if(result.size() > 0) {
        HasHPActivated = db->fetchValue<int>(result, 0, "HasHPActivated");
        HasHPAccess = db->fetchValue<int>(result, 0, "HasHPAccess");
    }

    if (HasHPAccess == 0) {
        player->SendMsg(HUD_PRINTTALK, "%s %s.", config->Fetch<const char*>("swiftly_shop.prefix"), FetchTranslation("swiftly_shop.NoAccess"));
        player->HideMenu();
        return;
    } else if (HasHPActivated == 1) {
        player->SendMsg(HUD_PRINTTALK, "%s %s.", config->Fetch<const char*>("swiftly_shop.prefix"), FetchTranslation("swiftly_shop.AlreadyActivated"));
        player->HideMenu();
        return;
    } else {
        db->Query("UPDATE %s SET HasHPActivated = 1 WHERE steamid = '%llu'", "players_features", player->GetSteamID());
        player->SendMsg(HUD_PRINTTALK, "%s %s.", config->Fetch<const char*>("swiftly_shop.prefix"), FetchTranslation("swiftly_shop.ActivatedFeature"));
        player->HideMenu();
    }
}

void Command_ShopDeactivateHP(int playerID, const char **args, uint32_t argsCount, bool silent)
{
     if (playerID == -1)
        return;
        
    if (!db->IsConnected())
        return;

        Player *player = g_playerManager->GetPlayer(playerID);

        if (player == nullptr)
        return;

    DB_Result result = db->Query("select * from `players_features` where steamid = '%llu' limit 1", player->GetSteamID());
    int HasHPActivated = 0;
    int HasHPAccess = 0;
    if(result.size() > 0) {
        HasHPActivated = db->fetchValue<int>(result, 0, "HasHPActivated");
        HasHPAccess = db->fetchValue<int>(result, 0, "HasHPAccess");
    }

    if (HasHPAccess == 0) {
        player->SendMsg(HUD_PRINTTALK, "%s %s.", config->Fetch<const char*>("swiftly_shop.prefix"), FetchTranslation("swiftly_shop.NoAccess"));
        player->HideMenu();
        return;
    } else if (HasHPActivated == 0) {
        player->SendMsg(HUD_PRINTTALK, "%s %s.", config->Fetch<const char*>("swiftly_shop.prefix"), FetchTranslation("swiftly_shop.AlreadyDeactivated"));
        player->HideMenu();
        return;
    } else {
        db->Query("UPDATE %s SET HasHPActivated = 0 WHERE steamid = '%llu'", "players_features", player->GetSteamID());
        player->SendMsg(HUD_PRINTTALK, "%s %s.", config->Fetch<const char*>("swiftly_shop.prefix"), FetchTranslation("swiftly_shop.DeactivatedFeature"));
        player->HideMenu();
    }
}


void OnPlayerSpawn(Player *player)
{
    if (!db->IsConnected())
        return;

    if (player->IsFirstSpawn() && !player->IsFakeClient()) {
        db->Query("insert ignore into `players_credits` (steamid) values ('%llu')", player->GetSteamID());
        db->Query("insert ignore into `players_features` (steamid) values ('%llu')", player->GetSteamID());
    }
}



void OnPlayerDeath(Player *player, Player *attacker, Player *assister, bool assistedflash, const char *weapon, bool headshot, short dominated, short revenge, short wipe, short penetrated, bool noreplay, bool noscope, bool thrusmoke, bool attackerblind, float distance, short dmg_health, short dmg_armor, short hitgroup)
{

    DB_Result result3;

    DB_Result result = db->Query("SELECT credits FROM %s WHERE steamid = '%llu' LIMIT 1", "players_credits", player->GetSteamID());
    int currentCredits = 0;
    if(result.size() > 0) {
        currentCredits = db->fetchValue<int>(result, 0, "credits");
    }

    result3 = db->Query("SELECT HasHPActivated FROM %s WHERE steamid = '%llu' LIMIT 1", "players_features", attacker->GetSteamID());
    int HasHPActivated = 0;
    if(result3.size() > 0) {
        HasHPActivated = db->fetchValue<int>(result3, 0, "HasHPActivated");
    }

    int healthtogive = config->Fetch<int>("swiftly_shop.HealthToGive");
    int maxhealth = config->Fetch<int>("swiftly_shop.MaxHealth");
    int playerhealth = attacker->health->Get();

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
        if (HasHPActivated == 1 && playerhealth <= maxhealth){
            char buffer[256];
            sprintf(buffer, FetchTranslation("swiftly_shop.HealthGiven"), healthtogive);
            attacker->health->SetMax(maxhealth);
            attacker->health->Set(attacker->health->Get() + healthtogive);
            attacker->SendMsg(HUD_PRINTTALK, "%s %s.", config->Fetch<const char*>("swiftly_shop.prefix"), buffer);
        }
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
        if (HasHPActivated == 1 && playerhealth <= maxhealth){
            char buffer[256];
            sprintf(buffer, FetchTranslation("swiftly_shop.HealthGiven"), healthtogive);
            attacker->health->SetMax(maxhealth);
            attacker->health->Set(attacker->health->Get() + healthtogive);
            attacker->SendMsg(HUD_PRINTTALK, "%s %s.", config->Fetch<const char*>("swiftly_shop.prefix"), buffer);
        }
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
        if (HasHPActivated == 1 && playerhealth <= maxhealth){
            char buffer[256];
            sprintf(buffer, FetchTranslation("swiftly_shop.HealthGiven"), healthtogive);
            attacker->health->SetMax(maxhealth);
            attacker->health->Set(attacker->health->Get() + healthtogive);
            attacker->SendMsg(HUD_PRINTTALK, "%s %s.", config->Fetch<const char*>("swiftly_shop.prefix"), buffer);
        }
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
    commands->Register("shop", reinterpret_cast<void *>(&Command_Shop));
    commands->Register("shopbuy_hp", reinterpret_cast<void *>(&Command_ShopBuyHP));
    commands->Register("shop_activatehp", reinterpret_cast<void *>(&Command_ShopActivateHP));
    commands->Register("shop_deactivatehp", reinterpret_cast<void *>(&Command_ShopDeactivateHP));


    db = new Database("swiftly_shop");
    DB_Result result = db->Query("CREATE TABLE IF NOT EXISTS `players_credits` (`steamid` varchar(128) NOT NULL, `credits` int(11) NOT NULL DEFAULT 0) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;");
    DB_Result result1 = db->Query("CREATE TABLE IF NOT EXISTS `players_features` (`steamid` varchar(128) NOT NULL, `HasHPAccess` int(11) NOT NULL, `HasHPActivated` int(11) NOT NULL DEFAULT 0) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;");
    if (result.size() > 0)
            db->Query("ALTER TABLE `players_credits` ADD UNIQUE KEY `steamid` (`steamid`);");
    if (result1.size() > 0)
            db->Query("ALTER TABLE `players_features` ADD UNIQUE KEY `steamid` (`steamid`);");


    menus->RegisterMenu("shop", "Shop Menu", config->Fetch<const char*>("swiftly_shop.MenuColor"), {
        {"HP / Kill", "hpkill"},
        {"See your credits", "sw_credits"}
    });

    menus->RegisterMenu("hpkill", "HP / Kill Menu", config->Fetch<const char*>("swiftly_shop.MenuColor"), {
        {"Buy", "sw_shopbuy_hp"},
        {"Toggle", "togglehp"}
    });

    menus->RegisterMenu("togglehp", "HP / Kill Menu", config->Fetch<const char*>("swiftly_shop.MenuColor"), {
        {"Activate HP / Kill", "sw_shop_activatehp"},
        {"Deactivate HP / Kill", "sw_shop_deactivatehp"}
    });

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
    return "1.0.1";
}

const char *GetPluginName()
{
    return "swiftly_shop";
}

const char *GetPluginWebsite()
{
    return "https://github.com/swiftly-solution/swiftly_shop";
}