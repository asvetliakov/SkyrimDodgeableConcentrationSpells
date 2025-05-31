#pragma once

namespace Config
{

    void         ReadConfig();
    inline bool  playerRestrictMovement = true;
    inline float playerMovementModifier = 0.4f;
    inline bool  npcRestrictMovement    = true;
    inline bool  npcRestrictRotation    = true;
    inline float npcMovementModifier    = 0.1f;
} // namespace Config
