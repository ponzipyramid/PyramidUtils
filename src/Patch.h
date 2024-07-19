#pragma once
#include "Trampoline.h"
#include "MarkerManager.h"

using namespace PyramidUtils;

namespace PyramidUtils
{
	class Patch
	{
	public:
		static void Install();
	private:
		static bool UpdateQuests(void* a_1, void* a_2, void* a_pos, void* a_refHandle, void* a_markerGotoFrame, void* a_questTarget);
		static inline REL::Relocation<decltype(UpdateQuests)> _UpdateQuests;

		struct UpdateQuestsHook : hooks::Hook<5>
		{
			struct HookCodeGenerator : Xbyak::CodeGenerator
			{
				HookCodeGenerator(std::uintptr_t a_hookedAddress)
				{
					Xbyak::Label hookLabel;
					Xbyak::Label retnLabel;

					mov(ptr[rsp + 0x28], rbx);  // rbx = TESQuestTarget*
					call(ptr[rip + hookLabel]);

					jmp(ptr[rip + retnLabel]);

					L(hookLabel), dq(reinterpret_cast<std::uintptr_t>(&UpdateQuests));
					L(retnLabel), dq(a_hookedAddress + 5);

					ready();
				}
			};

			UpdateQuestsHook(std::uintptr_t a_hookedAddress) :
				Hook{ a_hookedAddress, HookCodeGenerator{ a_hookedAddress } }
			{}
		};
	};
}