#include "Hooks.h"
#include "ActorManager.h"
#include "Trampoline.h"

using namespace PyramidUtils;
namespace Patches
{
	inline uint64_t test;
	void UpdateQuests(void* a_1, void* a_2, RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::uint32_t a_markerGotoFrame,
		RE::TESQuestTarget* a_questTarget)
	{
		test++;
		if (test % 10000000) {
			const auto marker = RE::TESObjectREFR::LookupByHandle(a_refHandle).get();

			logger::info("UpdateQuests {}", marker != nullptr);
		}
	}

	std::uintptr_t getJmpTarget(std::uintptr_t a_addr) {
		uint8_t opcode = *(uint8_t*)a_addr;

		if (opcode != 0xE9)
			return (uintptr_t)-1;

		int32_t offset = *(int32_t*)(a_addr + 1);

		uintptr_t target = a_addr + 5 + offset;

		return target;
	}

	struct UpdateQuestsHook : hooks::Hook<5>
	{
		struct HookCodeGenerator : Xbyak::CodeGenerator
		{
			HookCodeGenerator(std::uintptr_t a_cnoAddr)
			{
				Xbyak::Label hookLabel;
				Xbyak::Label cnoLabel;

				push(rcx);
				push(rdx);
				push(r8);
				push(r9);

				//sub(rsp, 0x28);
				mov(ptr[rsp + 0x28], rbx);  // rbx = TESQuestTarget*
				call(ptr[rip + hookLabel]);
				//add(rsp, 0x28);

				pop(r9);
				pop(r8);
				pop(rdx);
				pop(rcx);

				jmp(ptr[rip + cnoLabel]);

				L(hookLabel), dq(reinterpret_cast<std::uintptr_t>(&UpdateQuests));
				L(cnoLabel), dq(a_cnoAddr);

				ready();
			}
		};

		UpdateQuestsHook(std::uintptr_t a_hookedAddress, uintptr_t a_cnoAddr) :
			Hook{ a_hookedAddress, HookCodeGenerator{ a_cnoAddr } }
		{}
	};

	void Install() {
		const auto address = REL::RelocationID{ 50826, 51691 }.address() + REL::VariantOffset{ 0x114, 0x180, 0x114 }.offset();

		const auto target = getJmpTarget(address);

		UpdateQuestsHook updateQuestsHook{ address, target };

		static hooks::DefaultTrampoline tramp1{ updateQuestsHook.getSize() };
		tramp1.write_branch(updateQuestsHook);
	}
}

void Hooks::Install() {
	logger::info("installing hooks");
	
    SKSE::AllocTrampoline(28);
    auto& trampoline = SKSE::GetTrampoline();

    REL::Relocation<std::uintptr_t> det{ REL::RelocationID(41659, 42742), REL::VariantOffset(0x526, 0x67B, 0x67B) };
    _DoDetect = trampoline.write_call<5>(det.address(), DoDetect);

    REL::Relocation<std::uintptr_t> upc{ RE::Character::VTABLE[0] };
    _UpdateCombat = upc.write_vfunc(0x0E4, UpdateCombat);

    REL::Relocation<std::uintptr_t> amd{ REL::RelocationID(36359, 37350), REL::VariantOffset(0xF0, 0xFB, 0xFB) };
    _ApplyMovementDelta = trampoline.write_call<5>(amd.address(), ApplyMovementDelta);

	Patches::Install();
}

void Hooks::UpdateCombat(RE::Character* a_this) {
    if (!ActorManager::IsCalmed(a_this)) {
        _UpdateCombat(a_this);
    } else if (a_this->IsInCombat()) {
        a_this->StopCombat();
    }
}

void Hooks::ApplyMovementDelta(RE::Actor* a_actor, float a_delta) {
    if (!ActorManager::IsCollisionFrozen(a_actor)) {
        _ApplyMovementDelta(a_actor, a_delta);
    }
}

uint8_t* Hooks::DoDetect(RE::Actor* viewer, RE::Actor* target, int32_t& detectval, uint8_t& unk04, uint8_t& unk05, uint32_t& unk06, RE::NiPoint3& pos, float& unk08, float& unk09, float& unk10) {
    if (viewer && ActorManager::IsCalmed(viewer) || target && ActorManager::IsCalmed(target)) {
        detectval = -1000;
        return nullptr;
    }
    
    return _DoDetect(viewer, target, detectval, unk04, unk05, unk06, pos, unk08, unk09, unk10);
}