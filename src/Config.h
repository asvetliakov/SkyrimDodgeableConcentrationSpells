#pragma once

#include <SKSE/SKSE.h>
#include <articuno/articuno.h>

class Debug {
public:
    [[nodiscard]] inline spdlog::level::level_enum GetLogLevel() const noexcept { return _logLevel; }

    [[nodiscard]] inline spdlog::level::level_enum GetFlushLevel() const noexcept { return _flushLevel; }

private:
    articuno_serialize(ar) {
        auto logLevel = spdlog::level::to_string_view(_logLevel);
        auto flushLevel = spdlog::level::to_string_view(_flushLevel);
        ar <=> articuno::kv(logLevel, "logLevel");
        ar <=> articuno::kv(flushLevel, "flushLevel");
    }

    articuno_deserialize(ar) {
        *this = Debug();
        std::string logLevel;
        std::string flushLevel;
        if (ar <=> articuno::kv(logLevel, "logLevel")) {
            _logLevel = spdlog::level::from_str(logLevel);
        }
        if (ar <=> articuno::kv(flushLevel, "flushLevel")) {
            _flushLevel = spdlog::level::from_str(flushLevel);
        }
    }

    spdlog::level::level_enum _logLevel{spdlog::level::level_enum::info};
    spdlog::level::level_enum _flushLevel{spdlog::level::level_enum::trace};

    friend class articuno::access;
};

class NPCConfig {
public:
    bool restrictMovement = true;
    bool restrictRotation = true;
    bool lockDragonShouts = true;
    float movementModifier = 0.1f;

private:
    articuno_serialize(ar) {
        ar <=> articuno::kv(restrictMovement, "restrictMovement");
        ar <=> articuno::kv(restrictRotation, "restrictRotation");
        ar <=> articuno::kv(lockDragonShouts, "lockDragonShouts");
        ar <=> articuno::kv(movementModifier, "movementModifier");
    }

    articuno_deserialize(ar) {
        *this = NPCConfig();
        std::string _restrictMovement;
        std::string _restrictRotation;
        std::string _lockDragonShouts;
        std::string _movementModifier;
        if (ar <=> articuno::kv(_restrictMovement, "restrictMovement")) {
            restrictMovement = _restrictMovement == "true" || _restrictMovement == "1";
        }
        if (ar <=> articuno::kv(_restrictRotation, "restrictRotation")) {
            restrictRotation = _restrictRotation == "true" || _restrictRotation == "1";
        }
        if (ar <=> articuno::kv(_lockDragonShouts, "lockDragonShouts")) {
            lockDragonShouts = _lockDragonShouts == "true" || _lockDragonShouts == "1";
        }
        if (ar <=> articuno::kv(_movementModifier, "movementModifier")) {
            movementModifier = std::stof(_movementModifier);
        }
    }

    friend class articuno::access;
};

class PlayerConfig {
public:
    bool restrictMovement = true;
    float movementModifier = 0.1f;

private:
    articuno_serialize(ar) {
        ar <=> articuno::kv(restrictMovement, "restrictMovement");
        ar <=> articuno::kv(movementModifier, "movementModifier");
    }

    articuno_deserialize(ar) {
        *this = PlayerConfig();
        std::string _restrictMovement;
        std::string _movementModifier;
        if (ar <=> articuno::kv(_restrictMovement, "restrictMovement")) {
            restrictMovement = _restrictMovement == "true" || _restrictMovement == "1";
        }
        if (ar <=> articuno::kv(_movementModifier, "movementModifier")) {
            movementModifier = std::stof(_movementModifier);
        }
    }

    friend class articuno::access;
};

class Config {
public:
    [[nodiscard]] inline const Debug& GetDebug() const noexcept { return _debug; }
    [[nodiscard]] inline const NPCConfig& GetNPCConfig() const noexcept { return _npc_config; }
    [[nodiscard]] inline const PlayerConfig& GetPlayerConfig() const noexcept { return _player_config; }

    [[nodiscard]] static const Config& GetSingleton() noexcept;

private:
    articuno_serde(ar) {
        ar <=> articuno::kv(_debug, "debug");
        ar <=> articuno::kv(_player_config, "player");
        ar <=> articuno::kv(_npc_config, "npc");
    }

    Debug _debug;
    NPCConfig _npc_config;
    PlayerConfig _player_config;

    friend class articuno::access;
};
