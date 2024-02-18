#pragma once

namespace PyramidUtils {
    class ActorManager {
        public:
            inline static void SetActorCalmed(RE::Actor* a_actor, bool a_calmed) {
                if (a_calmed) {
                    calmed.insert(a_actor->GetFormID());
                } else {
                    calmed.erase(a_actor->GetFormID());
                }
            }

            inline static void SetActorFrozen(RE::Actor* a_actor, bool a_enabled) {
                a_actor->SetCollision(a_enabled);

                if (a_enabled) {
                    disabledCollision.insert(a_actor->GetFormID());
                }
                else {
                    disabledCollision.erase(a_actor->GetFormID());
                }
            }

            inline static bool IsCalmed(RE::Actor* a_actor) { return a_actor && calmed.contains(a_actor->GetFormID()); }

            inline static bool IsCollisionFrozen(RE::Actor* a_actor) { return a_actor && disabledCollision.contains(a_actor->GetFormID()); }

            inline static void Serialize(SKSE::SerializationInterface* a_intfc) {
                SKSE::log::info("serializing actor manager - START");
                SerializeActorList(a_intfc, calmed);
                SKSE::log::info("serialized {} calmed actors(s)", calmed.size());
                SerializeActorList(a_intfc, disabledCollision);
                SKSE::log::info("serialized {} frozen actor(s)", disabledCollision.size());
                SKSE::log::info("serializing actor manager - END");
            }

            inline static void Deserialize(SKSE::SerializationInterface* a_intfc) {
                SKSE::log::info("deserializing actor manager - START");
                DeserializeActorList(a_intfc, calmed);
                SKSE::log::info("deserialized {} calmed actors(s)", calmed.size());
                DeserializeActorList(a_intfc, disabledCollision);
                SKSE::log::info("deserialized {} frozen actors(s)", disabledCollision.size());
                SKSE::log::info("deserializing actor manager - END");
            }

            inline static void Revert(SKSE::SerializationInterface*) {
                SKSE::log::info("reverting actor manager - START");
                calmed.clear();
                disabledCollision.clear();
                SKSE::log::info("reverting actor manager - END");
            }
        private:
            static inline void SerializeActorList(SKSE::SerializationInterface* a_intfc, std::unordered_set<RE::FormID>& a_list) {
                auto num_actors = a_list.size();
                a_intfc->WriteRecordData(&num_actors, sizeof(num_actors));
                for (auto& form_id : a_list) {
                    a_intfc->WriteRecordData(&form_id, sizeof(form_id));
                }
            }

            static inline void DeserializeActorList(SKSE::SerializationInterface* a_intfc, std::unordered_set<RE::FormID>& a_list) {
                std::size_t num_actors;
                a_intfc->ReadRecordData(&num_actors, sizeof(num_actors));

                for (; num_actors > 0; --num_actors) {
                    RE::FormID form_id;
                    a_intfc->ReadRecordData(&form_id, sizeof(form_id));
                    if (auto victim = RE::TESForm::LookupByID<RE::Actor>(form_id)) {
						if (auto ref = victim->GetObjectReference()) {
                            a_list.insert(form_id);
                        }
                    }
                }
            }

            static inline std::unordered_set<RE::FormID> calmed;
            static inline std::unordered_set<RE::FormID> disabledCollision;
    };
}