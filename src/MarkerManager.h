#pragma once

namespace PyramidUtils
{
	class MarkerManager
	{
	public:
		static inline void SetQuest(RE::TESQuest* a_quest) {
			_quest = a_quest;
		}
		static inline void SetMarker(RE::TESObjectREFR* a_ref) {
			_markers[_quest] = a_ref;
		}
		static inline RE::TESObjectREFR* GetLastMarker()
		{
			return _markers.count(_quest) ? _markers[_quest] : nullptr;
		}
		static inline RE::TESObjectREFR* GetQuestMarker(RE::TESQuest* a_quest) {
			return _markers.count(a_quest) ? _markers[a_quest] : nullptr;
		}
	private:
		static inline std::unordered_map<RE::TESQuest*, RE::TESObjectREFR*> _markers;
		static inline RE::TESQuest* _quest = nullptr;
	};
}