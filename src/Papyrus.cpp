#include "Papyrus.h"
#include "ActorManager.h"

using namespace PyramidUtils;

namespace {
    constexpr std::string_view PapyrusClass = "DealManager";

    void CalmActor(RE::StaticFunctionTag*, RE::Actor* a_actor) {
        ActorManager::CalmActor(a_actor);
    }

    void ClearActor(RE::StaticFunctionTag*, RE::Actor* a_actor) {
        ActorManager::ClearActor(a_actor);
    }
}

bool Papyrus::RegisterFunctions(RE::BSScript::IVirtualMachine* vm) {
    vm->RegisterFunction("CalmActor", PapyrusClass, CalmActor);

    return true;
}