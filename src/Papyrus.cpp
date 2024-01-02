#include "Papyrus.h"
#include "ActorManager.h"

using namespace PyramidUtils;

namespace {
    constexpr std::string_view PapyrusClass = "PyramidUtils";

    void CalmActor(RE::StaticFunctionTag*, RE::Actor* a_actor) {
        ActorManager::CalmActor(a_actor);
    }

    void ClearActor(RE::StaticFunctionTag*, RE::Actor* a_actor) {
        ActorManager::ClearActor(a_actor);
    }

    std::vector<RE::Actor*> DetectedBy(RE::StaticFunctionTag*, RE::Actor* a_target, std::vector<RE::Actor*> a_watchers) {
        std::vector<RE::Actor*> detected_by;

        for (auto watcher : a_watchers) {
            if (watcher->RequestDetectionLevel(a_target) > 0) {
                detected_by.push_back(watcher);
            }
        }

        return detected_by;
    }

    std::vector<RE::TESForm*> GetItemsByKeyword(RE::StaticFunctionTag*, RE::TESObjectREFR* a_container, std::vector<RE::BGSKeyword*> a_keywords, bool a_matchall) {
        std::vector<RE::TESForm*> forms;

        if (!a_container) {
            return forms;
        }

        auto inventory = a_container->GetInventory();
        for (const auto& [form, data] : inventory) {
            if (!form->GetPlayable() || form->GetName()[0] == '\0') continue;
            if (data.second->IsQuestObject()) continue;
            
            if (form->HasKeywordInArray(a_keywords, a_matchall)) {
                forms.push_back(form);
            }
        }

        return forms; 
    }

    int RemoveItemsByKeyword(RE::StaticFunctionTag*, RE::TESObjectREFR* a_fromContainer, std::vector<RE::BGSKeyword*> a_keywords, bool a_matchall, RE::TESObjectREFR* a_toContainer) {
        int totalRemoved = 0;
        
        if (!a_fromContainer) {
            return totalRemoved;
        }

        auto removeReason = a_toContainer ? RE::ITEM_REMOVE_REASON::kStoreInContainer : RE::ITEM_REMOVE_REASON::kRemove;

        auto inventory = a_fromContainer->GetInventory();
        auto counts = a_fromContainer->GetInventoryCounts();

        for (const auto& [form, data] : inventory) {
            if (!form->GetPlayable() || form->GetName()[0] == '\0') continue;
            if (data.second->IsQuestObject()) continue;
            
            if (form->HasKeywordInArray(a_keywords, a_matchall)) {
                auto count = counts[form];

                a_fromContainer->RemoveItem(form, count, removeReason, nullptr, a_toContainer);
                totalRemoved += count;
            }
        }

        return totalRemoved;
    }
}

bool Papyrus::RegisterFunctions(RE::BSScript::IVirtualMachine* vm) {
    vm->RegisterFunction("CalmActor", PapyrusClass, CalmActor);
    vm->RegisterFunction("ClearActor", PapyrusClass, ClearActor);
    vm->RegisterFunction("DetectedBy", PapyrusClass, DetectedBy);
    vm->RegisterFunction("GetItemsByKeyword", PapyrusClass, GetItemsByKeyword);
    vm->RegisterFunction("RemoveItemsByKeyword", PapyrusClass, RemoveItemsByKeyword);

    return true;
}