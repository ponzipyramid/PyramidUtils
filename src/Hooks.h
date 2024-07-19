#pragma once

namespace PyramidUtils {

    constexpr auto noCollisionFlag = static_cast<std::uint32_t>(RE::CFilter::Flag::kNoCollision);

    class Hooks {
        public:
            static void Install();
        private:
            static void UpdateCombat(RE::Character* a_this);
            static uint8_t* DoDetect(RE::Actor* viewer, RE::Actor* target, int32_t& detectval, uint8_t& unk04, uint8_t& unk05, uint32_t& unk06, RE::NiPoint3& pos, float& unk08, float& unk09, float& unk10);
            static void ApplyMovementDelta(RE::Actor* a_actor, float a_delta);
			static RE::RefHandle GetQuestMarkerRef(int64_t, int64_t, RE::TESQuest*);
			static bool UpdateQuests(void*, void*, RE::NiPoint3*, const RE::RefHandle&, std::int32_t);

            static inline REL::Relocation<decltype(UpdateCombat)> _UpdateCombat;
            static inline REL::Relocation<decltype(DoDetect)> _DoDetect;
			static inline REL::Relocation<decltype(ApplyMovementDelta)> _ApplyMovementDelta;
			static inline REL::Relocation<decltype(GetQuestMarkerRef)> _GetQuestMarkerRef;
			static inline REL::Relocation<decltype(UpdateQuests)> _UpdateQuests;

            static inline uint64_t test;

    };
}