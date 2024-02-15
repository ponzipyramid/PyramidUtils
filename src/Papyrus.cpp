#include "Papyrus.h"
#include "ActorManager.h"
#include "Input.h"
#include "Expression.h"

using namespace PyramidUtils;

namespace {
    constexpr std::string_view PapyrusClass = "PyramidUtils";

    bool HasKeywords(RE::TESForm* a_form, std::vector<RE::BGSKeyword*> a_kwds, bool a_matchAll) {
        if (a_matchAll) {
            for (auto kwd : a_kwds) {
                if (!a_form->HasKeywordInArray(std::vector<RE::BGSKeyword*> { kwd }, false)) {
                    return false;
                }   
            }
            return true;
        } else {
            return a_form->HasKeywordInArray(a_kwds, false);
        }

    }

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
        std::vector<RE::TESForm*> filtered;

        if (!a_container) {
            return filtered;
        }

        auto inventory = a_container->GetInventory();
        for (const auto& [form, data] : inventory) {
            if (!form->GetPlayable() || form->GetName()[0] == '\0') continue;
            
            if (a_keywords.empty() || HasKeywords(form, a_keywords, a_matchall)) {
                filtered.push_back(form);
            }
        }

        return filtered; 
    }

    std::vector<RE::TESForm*> FilterFormsByKeyword(RE::StaticFunctionTag*, std::vector<RE::TESForm*> a_forms, std::vector<RE::BGSKeyword*> a_keywords, bool a_matchall, bool a_invert) {
        SKSE::log::info("FilterFormsByKeyword");
        std::vector<RE::TESForm*> filtered;

        for (const auto& form : a_forms) {
            logs::info("Form: {} kwd = {}, invert = {}", form->GetName(), HasKeywords(form, a_keywords, a_matchall), a_invert);
            if (HasKeywords(form, a_keywords, a_matchall) != a_invert) {
                filtered.push_back(form);
            }
        }

        return filtered;
    }

    std::vector<RE::TESForm*> FilterFormsByGoldValue(RE::StaticFunctionTag*, std::vector<RE::TESForm*> a_forms, int a_value, bool a_greater, bool a_equal) {
        SKSE::log::info("FilterFormsByGoldValue");
        std::vector<RE::TESForm*> filtered;

        for (const auto& form : a_forms) {            
            if ((a_greater && form->GetGoldValue() > a_value) || (!a_greater && form->GetGoldValue() < a_value) || (a_equal && form->GetGoldValue() == a_value)) {
                filtered.push_back(form); 
            }
        }

        return filtered;
    }

    std::vector<RE::TESForm*> FilterByEnchanted(RE::StaticFunctionTag*, RE::TESObjectREFR* a_container, std::vector<RE::TESForm*> a_forms, bool a_ench) {
        SKSE::log::info("FilterByEnchanted");
        std::vector<RE::TESForm*> filtered;

        if (!a_container) {
            return filtered;
        }

        std::unordered_set<RE::TESForm*> forms(a_forms.begin(), a_forms.end());

        auto inventory = a_container->GetInventory();
        for (const auto& [form, data] : inventory) {
            if (!form->GetPlayable() || form->GetName()[0] == '\0') continue;

            if ((data.second->GetEnchantment() != nullptr) == a_ench && forms.contains(form)) {
                filtered.push_back(form);
            }
        }

        return filtered; 
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
        
        if (auto speakerObjPtr = RE::MenuTopicManager::GetSingleton()->speaker) {
            if (auto speakerPtr = speakerObjPtr.get()) {
                if (auto speaker = speakerPtr.get()) {
                    return speaker->As<RE::Actor>();
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

    RE::BSScript::LatentStatus SetPhonemeModifierSmooth(RE::BSScript::Internal::VirtualMachine*, RE::VMStackID a_stackId, RE::StaticFunctionTag*, RE::Actor* a_actor, int a_mode, int a_id1, int a_id2, int a_value, float a_speed, int a_delay) {
        return Expression::SetPhonemeModifierSmooth(a_actor, a_mode, a_id1, a_id2, a_value, a_speed, a_delay, a_stackId) ? RE::BSScript::LatentStatus::kStarted : RE::BSScript::LatentStatus::kFailed;
	}

    RE::BSScript::LatentStatus SmoothSetExpression(RE::BSScript::Internal::VirtualMachine*, RE::VMStackID a_stackId, RE::StaticFunctionTag*, RE::Actor* a_actor, int a_mood, int a_strength, int a_currentStrength, float a_modifier, float a_speed, int a_delay) {
        return Expression::SmoothSetExpression(a_actor, a_mood, a_strength, a_currentStrength, a_modifier, a_speed, a_delay, a_stackId) ? RE::BSScript::LatentStatus::kStarted : RE::BSScript::LatentStatus::kFailed;
    }
}

bool Papyrus::RegisterFunctions(RE::BSScript::IVirtualMachine* vm) {
    // actors
    vm->RegisterFunction("SetActorCalmed", PapyrusClass, SetActorCalmed);
    vm->RegisterFunction("SetActorFrozen", PapyrusClass, SetActorFrozen);
    vm->RegisterFunction("GetDetectedBy", PapyrusClass, GetDetectedBy);

    // inv processing
    vm->RegisterFunction("GetItemsByKeyword", PapyrusClass, GetItemsByKeyword);
    vm->RegisterFunction("FilterFormsByKeyword", PapyrusClass, FilterFormsByKeyword);
    vm->RegisterFunction("FilterFormsByGoldValue", PapyrusClass, FilterFormsByGoldValue);
    vm->RegisterFunction("FilterByEnchanted", PapyrusClass, FilterByEnchanted);
    vm->RegisterFunction("RemoveForms", PapyrusClass, RemoveForms);

    // player
    vm->RegisterFunction("GetPlayerSpeechTarget", PapyrusClass, GetPlayerSpeechTarget);
    
    // input
    vm->RegisterFunction("GetButtonForDXScanCode", PapyrusClass, GetButtonForDXScanCode);
    
    // strings
    vm->RegisterFunction("ReplaceAt", PapyrusClass, ReplaceAt);

    // expressions
    vm->RegisterLatentFunction<bool>("SetPhonemeModifierSmooth", PapyrusClass, SetPhonemeModifierSmooth);
    vm->RegisterLatentFunction<bool>("SmoothSetExpression", PapyrusClass, SmoothSetExpression);
    vm->RegisterLatentFunction<bool>("SmoothResetMFG", PapyrusClass, SmoothResetMFG);
    vm->RegisterFunction("GetExpressionId", PapyrusClass, GetExpressionId);
    vm->RegisterFunction("GetExpressionValue", PapyrusClass, GetExpressionValue);
    vm->RegisterFunction("GetPhonemeValue", PapyrusClass, GetPhonemeValue);
    vm->RegisterFunction("GetModifierValue", PapyrusClass, GetModifierValue);
    return true;
}