#include "Hooks.h"

#include "Config.h"

using namespace RE;
using namespace RE::BSScript;
using namespace REL;
using namespace SKSE;

namespace
{
    float Hook_GetMaximumMovementSpeed(RE::Actor* actor);
    void  Hook_AIProcess_SetRotationSpeedZ1(RE::AIProcess* aiProcess, float origRotationSpeed);

    REL::Relocation<decltype(Hook_GetMaximumMovementSpeed)>      _GetMaximumMovementSpeed;
    REL::Relocation<decltype(Hook_AIProcess_SetRotationSpeedZ1)> _AIProcess_SetRotationSpeedZ1;

    bool IsDragonActor(RE::Actor* actor)
    {
        if (!actor)
        {
            return false;
        }
        if (actor->HasKeywordString("ActorTypeDragon"))
        {
            return true;
        }
        // Some dragons may not have keyword
        auto race = actor->GetRace();
        if (race && race->HasKeywordString("ActorTypeDragon"))
        {
            return true;
        }

        return false;
    }

    bool IsAimedConcentrationSpell(RE::MagicItem* spell)
    {
        if (spell == nullptr)
        {
            return false;
        }
        if (spell->GetCastingType() != RE::MagicSystem::CastingType::kConcentration)
        {
            return false;
        }
        if (spell->GetDelivery() != RE::MagicSystem::Delivery::kAimed)
        {
            return false;
        }
        return true;
    }

    bool ShouldRestrictMovement(RE::Actor* actor)
    {
        // Don't mess with dragons :(
        if (!actor || IsDragonActor(actor))
        {
            return false;
        }
        auto leftHandCaster  = actor->GetMagicCaster(RE::MagicSystem::CastingSource::kLeftHand);
        auto rightHandCaster = actor->GetMagicCaster(RE::MagicSystem::CastingSource::kRightHand);
        auto otherCaster     = actor->GetMagicCaster(RE::MagicSystem::CastingSource::kOther);
        if (rightHandCaster != nullptr && rightHandCaster->currentSpell != nullptr &&
            actor->IsCasting(rightHandCaster->currentSpell) &&
            IsAimedConcentrationSpell(rightHandCaster->currentSpell))
        {
            return true;
        }
        if (leftHandCaster != nullptr && leftHandCaster->currentSpell != nullptr &&
            actor->IsCasting(leftHandCaster->currentSpell) &&
            IsAimedConcentrationSpell(leftHandCaster->currentSpell))
        {
            return true;
        }
        if (otherCaster != nullptr && otherCaster->currentSpell != nullptr &&
            actor->IsCasting(otherCaster->currentSpell) &&
            IsAimedConcentrationSpell(otherCaster->currentSpell))
        {
            return true;
        }
        return false;
    }

    bool HasActiveCastingTimer(RE::Actor* actor)
    {
        auto leftHandCaster  = actor->GetMagicCaster(RE::MagicSystem::CastingSource::kLeftHand);
        auto rightHandCaster = actor->GetMagicCaster(RE::MagicSystem::CastingSource::kRightHand);
        auto otherCaster     = actor->GetMagicCaster(RE::MagicSystem::CastingSource::kOther);

        return leftHandCaster->castingTimer > 0 || rightHandCaster->castingTimer > 0 ||
               otherCaster->castingTimer > 0;
    }

    float Hook_GetMaximumMovementSpeed(RE::Actor* actor)
    {
        float originalSpeed = _GetMaximumMovementSpeed(actor);

        if (!ShouldRestrictMovement(actor))
        {
            return originalSpeed;
        }
        float newSpeed = originalSpeed;
        bool  isPlayer =
            actor->GetActorBase() == RE::PlayerCharacter::GetSingleton()->GetActorBase();
        if (isPlayer && Config::playerRestrictMovement)
        {
            newSpeed = newSpeed * Config::playerMovementModifier;
        }
        else if (!isPlayer && Config::npcRestrictMovement)
        {
            newSpeed = newSpeed * Config::npcMovementModifier;
        }
        return newSpeed;
    }

    void Hook_AIProcess_SetRotationSpeedZ1(RE::AIProcess* aiProcess, float origRotationSpeed)
    {
        // Exclude player
        if (aiProcess == RE::PlayerCharacter::GetSingleton()->GetActorRuntimeData().currentProcess)
        {
            return _AIProcess_SetRotationSpeedZ1(aiProcess, origRotationSpeed);
        }
        auto actor = aiProcess->GetUserData();
        if (actor != nullptr && ShouldRestrictMovement(actor) && Config::npcRestrictRotation)
        {
            float newRotationSpeed                         = origRotationSpeed < 0 ? -0.01f : 0.01f;
            aiProcess->high->pathingCurrentRotationSpeed.z = newRotationSpeed;
            aiProcess->middleHigh->rotationSpeed.z         = newRotationSpeed;

            return _AIProcess_SetRotationSpeedZ1(aiProcess, newRotationSpeed);
        }
        _AIProcess_SetRotationSpeedZ1(aiProcess, origRotationSpeed);
    }
} // namespace

void
Hooks::Initialize(Trampoline& trampoline)
{
    REL::Relocation<uintptr_t> setSpeedHook{RELOCATION_ID(37013, 37943)};
    _GetMaximumMovementSpeed = trampoline.write_call<5>(
        setSpeedHook.address() + REL::Relocate(0x1A, 0x51), Hook_GetMaximumMovementSpeed);

    REL::Relocation<uintptr_t> setRotationSpeedZHook{RELOCATION_ID(36365, 37356)}; // 5D87F0, 5FD7E0
    _AIProcess_SetRotationSpeedZ1 = trampoline.write_call<5>(
        setRotationSpeedZHook.address() + REL::VariantOffset(0x356, 0x3EF, 0).offset(),
        Hook_AIProcess_SetRotationSpeedZ1);
    log::info("Finished hooks initialization");
}
