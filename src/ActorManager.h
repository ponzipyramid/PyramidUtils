#pragma once

namespace PyramidUtils {
    class ActorManager {
        public:
            inline static void CalmActor(RE::Actor* a_actor) {
                calmed_actors.insert(a_actor->GetFormID());
            }

            inline static void ClearActor(RE::Actor* a_actor) {
                calmed_actors.erase(a_actor->GetFormID());
            }

            inline static bool IsCalmed(RE::Actor* a_actor) { return calmed_actors.contains(a_actor->GetFormID()); }

            inline static void Serialize(SKSE::SerializationInterface* a_intfc) {
                SKSE::log::info("serializing");
               
                auto num_actors = calmed_actors.size();
                a_intfc->WriteRecordData(&num_actors, sizeof(num_actors));
                for (auto& form_id : calmed_actors) {
                    a_intfc->WriteRecordData(&form_id, sizeof(form_id));
                } 
            }

            inline static void Deserialize(SKSE::SerializationInterface* a_intfc) {
                SKSE::log::info("deserializing");

                std::size_t num_actors;
                a_intfc->ReadRecordData(&num_actors, sizeof(num_actors));

                for (; num_actors > 0; --num_actors) {
                    RE::FormID form_id;
                    a_intfc->ReadRecordData(&form_id, sizeof(form_id));
                    if (auto victim = RE::TESForm::LookupByID<RE::Actor>(form_id)) {
						if (auto ref = victim->GetObjectReference()) {
                            calmed_actors.insert(form_id);
                        }
                    }
                }
            }

            inline static void Revert(SKSE::SerializationInterface*) {
                calmed_actors.clear();
            }
        private:
            static inline std::unordered_set<RE::FormID> calmed_actors;
    };
}