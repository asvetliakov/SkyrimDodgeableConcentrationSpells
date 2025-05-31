#include "config.h"

#include <SimpleIni.h>

void
Config::ReadConfig()
{
    constexpr auto path = L"Data/SKSE/Plugins/DodgeableConcentrationSpells.ini";

    CSimpleIniA ini;
    ini.SetUnicode();
    ini.LoadFile(path);

    playerRestrictMovement = ini.GetBoolValue("Player", "restrictMovement", true);
    playerMovementModifier =
        static_cast<float>(ini.GetDoubleValue("Player", "movementModifier", 0.4));

    npcRestrictMovement = ini.GetBoolValue("NPC", "restrictMovement", true);
    npcRestrictRotation = ini.GetBoolValue("NPC", "restrictRotation", true);
    npcMovementModifier = static_cast<float>(ini.GetDoubleValue("NPC", "movementModifier", 0.1));
}
