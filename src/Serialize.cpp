#include "Serialize.h"
#include "ActorManager.h"

using namespace PyramidUtils;

void Serialize::Save(SKSE::SerializationInterface* a_intfc) {
    ActorManager::Serialize(a_intfc);
}

void Serialize::Load(SKSE::SerializationInterface* a_intfc) {
    ActorManager::Deserialize(a_intfc);
}

void Serialize::Revert(SKSE::SerializationInterface* a_intfc) {
    ActorManager::Revert(a_intfc);
}