#include "Papyrus.h"
#include "ActorManager.h"
#include "Input.h"
#include "MarkerManager.h"

using namespace PyramidUtils;

namespace {
    constexpr std::string_view PapyrusClass = "PyramidUtils";

    bool HasKeywords(RE::TESForm* a_form, std::vector<RE::BGSKeyword*> a_kwds, bool a_matchAll)
	{
		if (!a_form)
			return false;

		auto keywordForm = a_form->As<RE::BGSKeywordForm>();

		if (const auto arrowForm = a_form->As<RE::TESAmmo>()) {
			keywordForm = keywordForm ? keywordForm : arrowForm->AsKeywordForm();
		}

		if (!keywordForm) {
			return false;
		}

		for (const auto& kwd : a_kwds) {
			if (!kwd) {
				continue;
			}

			if (keywordForm->HasKeyword(kwd)) {
				if (!a_matchAll) {
					return true;
				}
			} else {
				if (a_matchAll) {
					return false;
				}
			}
		}

		return false;
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

    std::vector<RE::BGSKeyword*> WornHasKeywords(RE::StaticFunctionTag*, RE::Actor* a_actor, std::vector<RE::BGSKeyword*> a_kwds) {
		if (!a_actor)
			return std::vector<RE::BGSKeyword*>{};
        
        std::unordered_set<RE::BGSKeyword*> worn;

        const auto inv = a_actor->GetInventory([](RE::TESBoundObject& a_object) {
			return a_object.IsArmor();
		});

		for (const auto& [item, invData] : inv) {
			const auto& [count, entry] = invData;
			if (count > 0 && entry->IsWorn()) {
                for (const auto& kwd : a_kwds) {
                    if (item->HasKeywordInArray(std::vector<RE::BGSKeyword*>{ kwd }, true)) {
						worn.insert(kwd);
                    }
                }
			}
		}

        return std::vector<RE::BGSKeyword*>{ worn.begin(), worn.end() };
    }

	std::vector<RE::BGSKeyword*> WornHasKeywordStrings(RE::StaticFunctionTag*, RE::Actor* a_actor, std::vector<std::string> a_kwds)
	{
		if (!a_actor)
			return std::vector<RE::BGSKeyword*>{};

		std::unordered_set<RE::BGSKeyword*> worn;

		const auto inv = a_actor->GetInventory([](RE::TESBoundObject& a_object) {
			return a_object.IsArmor();
		});

		for (const auto& [item, invData] : inv) {
			const auto& [count, entry] = invData;
			if (count > 0 && entry->IsWorn()) {
				for (const auto& kwd : a_kwds) {
					if (item->HasKeywordByEditorID(kwd)) {
						worn.insert(RE::TESForm::LookupByEditorID<RE::BGSKeyword>(kwd));
					}
				}
			}
		}

		return std::vector<RE::BGSKeyword*>{ worn.begin(), worn.end() };
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

    bool FormHasKeyword(RE::StaticFunctionTag*, RE::TESForm* a_form, std::vector<RE::BGSKeyword*> a_kwds, bool a_all)
	{
		return HasKeywords(a_form, a_kwds, a_all);
    }


    bool FormHasKeywordStrings(RE::StaticFunctionTag*, RE::TESForm* a_form, std::vector<std::string> a_kwds, bool a_all)
	{
		std::vector<RE::BGSKeyword*> kwds;
		kwds.reserve(a_kwds.size());
        for (const auto& kwdStr : a_kwds) {
			if (const auto kwd = RE::TESForm::LookupByEditorID<RE::BGSKeyword>(kwdStr)) {
				kwds.push_back(kwd);
			}
        }

		return HasKeywords(a_form, kwds, a_all);
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

    RE::TESObjectREFR* GetQuestMarker(RE::StaticFunctionTag*, RE::TESQuest* a_quest) {
		return MarkerManager::GetQuestMarker(a_quest);
    }

    std::vector<RE::TESForm*> GetInventoryNamedObjects(RE::StaticFunctionTag*, RE::TESObjectREFR* a_container,
		std::vector<std::string> itemNames)
	{
		std::vector<RE::TESForm*> refrs;

		if (!a_container) {
			return refrs;
		}

		auto inventory = a_container->GetInventory();
		for (const auto& [form, data] : inventory) {
			if (!form->GetPlayable() || form->GetName()[0] == '\0')
				continue;
			if (data.second->IsQuestObject())
				continue;

			std::string formName = form->GetName();
			for (const auto& iName : itemNames) {
				std::string itemName(iName);

				std::transform(itemName.begin(), itemName.end(), itemName.begin(),
					[](unsigned char c) { return (char)std::tolower(c); });

				std::transform(formName.begin(), formName.end(), formName.begin(),
					[](unsigned char c) { return (char)std::tolower(c); });

				SKSE::log::info("Processing item {} with tag {}", formName, itemName);

				if (formName.find(itemName) != std::string::npos) {
					refrs.push_back(form);
				}
			}
		}

		return refrs;
	}

    void Dismount(RE::StaticFunctionTag*, RE::Actor* a_actor)
    {
        // source: BTPS (https://gitlab.com/Shrimperator/skyrim-mod-betterthirdpersonselection)
		typedef __int64 (__fastcall DismountActor_func)(RE::Actor*);
		static auto DismountActor = RELOCATION_ID(36882, 37906);
		static auto dismount = REL::Relocation<DismountActor_func>{ DismountActor };
		dismount(a_actor);
    }

    float GetTemperFactor(RE::StaticFunctionTag*, RE::TESObjectREFR* a_container, RE::TESObjectWEAP* a_form)
    {
		const auto& inventory = a_container->GetInventory();
        for (const auto& [item, meta] : inventory) {
			const auto& [_, data] = meta;

            if (item->GetFormID() == a_form->GetFormID()) {
				if (data->extraLists) {
					for (const auto& list : (*data->extraLists)) {
						if (const auto extraData = list->GetByType<RE::ExtraTextDisplayData>()) {
							return extraData->temperFactor;
                        }
                    }
                }
            }
        }

        return 0.f;
    }
}

bool Papyrus::RegisterFunctions(RE::BSScript::IVirtualMachine* vm) {
#define REGISTERPAPYRUSFUNC(name) vm->RegisterFunction(#name, PapyrusClass, name);

    // actors
	REGISTERPAPYRUSFUNC(SetActorCalmed)
    REGISTERPAPYRUSFUNC(SetActorFrozen);
	REGISTERPAPYRUSFUNC(GetDetectedBy);
	REGISTERPAPYRUSFUNC(WornHasKeywords);
	REGISTERPAPYRUSFUNC(WornHasKeywordStrings);

    // inv processing
    REGISTERPAPYRUSFUNC(GetItemsByKeyword);
    REGISTERPAPYRUSFUNC(FilterFormsByKeyword);
    REGISTERPAPYRUSFUNC(FilterFormsByGoldValue);
    REGISTERPAPYRUSFUNC(FilterByEnchanted);
	REGISTERPAPYRUSFUNC(RemoveForms);
	REGISTERPAPYRUSFUNC(GetInventoryNamedObjects);
	REGISTERPAPYRUSFUNC(GetTemperFactor);

    // forms
	REGISTERPAPYRUSFUNC(FormHasKeyword);
	REGISTERPAPYRUSFUNC(FormHasKeywordStrings);

    // player
	REGISTERPAPYRUSFUNC(GetPlayerSpeechTarget);
	REGISTERPAPYRUSFUNC(Dismount);
    
    // input
    REGISTERPAPYRUSFUNC(GetButtonForDXScanCode);
    
    // strings
    REGISTERPAPYRUSFUNC(ReplaceAt);

    // map
	REGISTERPAPYRUSFUNC(GetQuestMarker);

    return true;
}