#include "Hooks.h"
#include "ActorManager.h"

using namespace PyramidUtils;

void Hooks::Install() {
    SKSE::AllocTrampoline(28);
    auto& trampoline = SKSE::GetTrampoline();

    REL::Relocation<std::uintptr_t> det{ REL::RelocationID(41659, 42742), REL::VariantOffset(0x526, 0x67B, 0x67B) };
    _DoDetect = trampoline.write_call<5>(det.address(), DoDetect);

    REL::Relocation<std::uintptr_t> upc{ RE::Character::VTABLE[0] };
    _UpdateCombat = upc.write_vfunc(0x0E4, UpdateCombat);

    REL::Relocation<std::uintptr_t> amd{ REL::RelocationID(36359, 37350), REL::VariantOffset(0xF0, 0xFB, 0xFB) };
    _ApplyMovementDelta = trampoline.write_call<5>(amd.address(), ApplyMovementDelta);
}

void Hooks::UpdateCombat(RE::Character* a_this) {
    if (!ActorManager::IsCalmed(a_this)) {
        _UpdateCombat(a_this);
    } else if (a_this->IsInCombat()) {
        a_this->StopCombat();
    }
}

void Hooks::ApplyMovementDelta(RE::Actor* a_actor, float a_delta) {
    if (!ActorManager::IsCollisionFrozen(a_actor)) {
        _ApplyMovementDelta(a_actor, a_delta);
    }
}

uint8_t* Hooks::DoDetect(RE::Actor* viewer, RE::Actor* target, int32_t& detectval, uint8_t& unk04, uint8_t& unk05, uint32_t& unk06, RE::NiPoint3& pos, float& unk08, float& unk09, float& unk10) {
    if (viewer && ActorManager::IsCalmed(viewer) || target && ActorManager::IsCalmed(target)) {
        detectval = -1000;
        return nullptr;
    }
    
    return _DoDetect(viewer, target, detectval, unk04, unk05, unk06, pos, unk08, unk09, unk10);
}