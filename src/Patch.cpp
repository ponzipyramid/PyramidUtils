#include "Patch.h"

using namespace PyramidUtils;

namespace
{
	std::uintptr_t getJmpTarget(std::uintptr_t a_addr)
	{
		uint8_t opcode = *(uint8_t*)a_addr;

		if (opcode != 0xE9)
			return 0;

		int32_t offset = *(int32_t*)(a_addr + 1);

		uintptr_t target = a_addr + 5 + offset;

		return target;
	}

	std::uintptr_t getCallTarget(std::uintptr_t a_addr)
	{
		uint8_t opcode = *(uint8_t*)a_addr;

		if (opcode != 0xFF)
			return 0;

		uintptr_t nextInstr = a_addr + 5;
		int32_t offset = *(int32_t*)(a_addr + 1);

		return nextInstr + offset;
	}
}

bool Patch::UpdateQuests(void* a_1, void* a_2, void* a_pos, void* a_refHandle, void* a_markerGotoFrame, void* a_questTarget)
{
	return _UpdateQuests(a_1, a_2, a_pos, a_refHandle, a_markerGotoFrame, a_questTarget);
}

void Patch::Install()
{
	const auto address = REL::RelocationID{ 50826, 51691 }.address() + REL::VariantOffset{ 0x114, 0x180, 0x114 }.offset();

	const auto jmpTarget = getJmpTarget(address);
	const auto callTarget = getCallTarget(jmpTarget + 5);

	logger::info("Targets = {}, {}", jmpTarget, callTarget);

	UpdateQuestsHook updateQuestsHook{ address };

	_UpdateQuests = callTarget;

	static hooks::DefaultTrampoline tramp1{ updateQuestsHook.getSize() };
	tramp1.write_branch(updateQuestsHook);
}