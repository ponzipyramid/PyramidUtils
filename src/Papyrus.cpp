#include "Papyrus.h"
#include "ActorManager.h"
#include "Input.h"

using namespace PyramidUtils;

namespace {
    constexpr std::string_view PapyrusClass = "PyramidUtils";

    void SetActorCalmed(RE::StaticFunctionTag*, RE::Actor* a_actor, bool a_calmed) {
        SKSE::log::info("SetActorCalmed");
        ActorManager::SetActorCalmed(a_actor, a_calmed);
    }

    void SetActorFrozen(RE::StaticFunctionTag*, RE::Actor* a_target, bool a_frozen) {
        SKSE::log::info("SetActorFrozen");
        ActorManager::SetActorFrozen(a_target, a_frozen);
    }

    std::vector<RE::Actor*> GetDetectedBy(RE::StaticFunctionTag*, RE::Actor* a_target) {
        SKSE::log::info("SetActorCalmed");
        std::vector<RE::Actor*> detected_by;

        const auto processLists = RE::ProcessLists::GetSingleton();
        for (const auto& actorHandle : processLists->highActorHandles) {
            const auto actorPtr = actorHandle.get();
            if (const auto actor = actorPtr.get()) {
                if (actor->RequestDetectionLevel(a_target) > 0) {
                    detected_by.push_back(actor);
                }
            }
           
        }

        return detected_by;
    }

    std::vector<RE::TESForm*> GetItemsByKeyword(RE::StaticFunctionTag*, RE::TESObjectREFR* a_container, std::vector<RE::BGSKeyword*> a_keywords, bool a_matchall) {
        SKSE::log::info("GetItemsByKeyword");
        std::vector<RE::TESForm*> forms;

        if (!a_container) {
            return forms;
        }

        auto inventory = a_container->GetInventory();
        for (const auto& [form, data] : inventory) {
            if (!form->GetPlayable() || form->GetName()[0] == '\0') continue;
            
            if (a_keywords.empty() || form->HasKeywordInArray(a_keywords, a_matchall)) {
                forms.push_back(form);
            }
        }

        return forms; 
    }

    std::vector<RE::TESForm*> FilterFormsByKeyword(RE::StaticFunctionTag*, std::vector<RE::TESForm*> a_forms, std::vector<RE::BGSKeyword*> a_keywords, bool a_matchall, bool a_invert) {
        SKSE::log::info("FilterFormsByKeyword");
        std::vector<RE::TESForm*> forms;

        for (const auto& form : a_forms) {            
            if (a_keywords.empty() || form->HasKeywordInArray(a_keywords, a_matchall) != a_invert) {
                forms.push_back(form);
            }
        }

        return forms;
    }

    std::vector<RE::TESForm*> FilterByEnchanted(RE::StaticFunctionTag*, RE::TESObjectREFR* a_container, std::vector<RE::TESForm*> a_forms, bool a_ench) {
        SKSE::log::info("FilterByEnchanted");
        std::vector<RE::TESForm*> forms;

        if (!a_container) {
            return forms;
        }

        auto inventory = a_container->GetInventory();
        for (const auto& [form, data] : inventory) {
            if (!form->GetPlayable() || form->GetName()[0] == '\0') continue;

            if ((data.second->GetEnchantment() != nullptr) == a_ench) {
                forms.push_back(form);
            }
        }

        return forms; 
    }

    long RemoveForms(RE::StaticFunctionTag*, RE::TESObjectREFR* a_fromContainer, std::vector<RE::TESForm*> a_forms, RE::TESObjectREFR* a_toContainer) {
        SKSE::log::info("RemoveForms");
        long totalRemoved = 0;
        
        if (!a_fromContainer) {
            return totalRemoved;
        }

        std::unordered_set<RE::TESForm*> formsToRemove(a_forms.begin(), a_forms.end());

        auto removeReason = a_toContainer ? RE::ITEM_REMOVE_REASON::kStoreInContainer : RE::ITEM_REMOVE_REASON::kRemove;

        auto inventory = a_fromContainer->GetInventory();
        auto counts = a_fromContainer->GetInventoryCounts();

        for (const auto& [form, data] : inventory) {
            if (!form->GetPlayable() || form->GetName()[0] == '\0') continue;
            if (data.second->IsQuestObject()) continue;

            if (formsToRemove.contains(form)) {
                auto count = counts[form];
                a_fromContainer->RemoveItem(form, count, removeReason, nullptr, a_toContainer);
                totalRemoved += count;
            }
        }

        return totalRemoved;
    }

    

    RE::Actor* GetPlayerSpeechTarget(RE::StaticFunctionTag*) {
        SKSE::log::info("GetPlayerSpeechTarget");
        const auto processLists = RE::ProcessLists::GetSingleton();
        for (const auto& actorHandle : processLists->highActorHandles) {
            const auto actorPtr = actorHandle.get();
            if (const auto actor = actorPtr.get()) {
                if (auto targetHandle = actor->GetActorRuntimeData().dialogueItemTarget) {
                    if (auto targetPtr = targetHandle.get()) {
                        if (targetPtr.get() == RE::PlayerCharacter::GetSingleton()) {
                            return actor;
                        }
                    }
                }
            }
           
        }

        return nullptr;
    }
    
    std::string GetButtonForDXScanCode(RE::StaticFunctionTag*, int a_keyCode) {
        SKSE::log::info("GetButtonForDXScanCode");
        return Input::GetButtonForDXScanCode(a_keyCode);
    }

    std::string ReplaceAt(RE::StaticFunctionTag*, std::string a_str, int a_index, std::string a_replacement) {
        SKSE::log::info("ReplaceAt");
        a_str.replace(a_index, a_replacement.size(), a_replacement);
        return a_str;
    }

}

bool Papyrus::RegisterFunctions(RE::BSScript::IVirtualMachine* vm) {
    vm->RegisterFunction("SetActorCalmed", PapyrusClass, SetActorCalmed);
    vm->RegisterFunction("SetActorFrozen", PapyrusClass, SetActorFrozen);
    vm->RegisterFunction("GetDetectedBy", PapyrusClass, GetDetectedBy);

    vm->RegisterFunction("GetItemsByKeyword", PapyrusClass, GetItemsByKeyword);
    vm->RegisterFunction("FilterFormsByKeyword", PapyrusClass, FilterFormsByKeyword);
    vm->RegisterFunction("FilterByEnchanted", PapyrusClass, FilterByEnchanted);
    vm->RegisterFunction("RemoveForms", PapyrusClass, RemoveForms);

    vm->RegisterFunction("GetPlayerSpeechTarget", PapyrusClass, GetPlayerSpeechTarget);
    vm->RegisterFunction("GetButtonForDXScanCode", PapyrusClass, GetButtonForDXScanCode);
    vm->RegisterFunction("ReplaceAt", PapyrusClass, ReplaceAt);

    return true;
}