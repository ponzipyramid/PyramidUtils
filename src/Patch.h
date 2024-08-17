#pragma once
#include "Trampoline.h"
#include "MarkerManager.h"

using namespace PyramidUtils;

namespace Patch
{
	void UpdateQuests(void* a_1, void* a_2, RE::NiPoint3* a_pos, const RE::RefHandle& a_refHandle, std::uint32_t a_markerGotoFrame)
	{
		const auto marker = RE::TESObjectREFR::LookupByHandle(a_refHandle).get();
		MarkerManager::SetMarker(marker);
	}

	std::uintptr_t getJmpTarget(std::uintptr_t a_addr)
	{
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

				call(ptr[rip + hookLabel]);

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

	void Install()
	{
		const auto address = REL::RelocationID{ 50826, 51691 }.address() + REL::VariantOffset{ 0x114, 0x180, 0x114 }.offset();

		const auto target = getJmpTarget(address);

		UpdateQuestsHook updateQuestsHook{ address, target };

		static hooks::DefaultTrampoline tramp1{ updateQuestsHook.getSize() };
		tramp1.write_branch(updateQuestsHook);
	}
}