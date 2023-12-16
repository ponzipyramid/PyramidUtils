#pragma once

namespace PyramidUtils {
    class Hooks {
        public:
            static void Install();
        private:
            static void UpdateCombat(RE::Character* a_this);
            static uint8_t* DoDetect(RE::Actor* viewer, RE::Actor* target, int32_t& detectval, uint8_t& unk04, uint8_t& unk05, uint32_t& unk06, RE::NiPoint3& pos, float& unk08, float& unk09, float& unk10);
            
            static inline REL::Relocation<decltype(UpdateCombat)> _UpdateCombat;
            static inline REL::Relocation<decltype(DoDetect)> _DoDetect;
    };
}