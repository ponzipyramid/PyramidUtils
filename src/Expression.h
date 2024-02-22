#pragma once
#include <chrono>
#include <thread>

namespace PyramidUtils::Expression {
	enum Mode : int
	{
		Reset = -1,
		Phoneme,
		Modifier,
		ExpressionValue,
		ExpressionId
	};

	inline int GetExpressionId(RE::Actor* a_actor)
	{
		SKSE::log::info("GetExpressionId({}) called", a_actor ? a_actor->GetName() : "NONE");

		if (a_actor == nullptr) return 7;

		RE::BSFaceGenAnimationData* animData = a_actor->GetFaceGenAnimationData();

		if (animData == nullptr) return 7;

		const size_t count_exp = animData->expressionKeyFrame.count;
		for (int i = 0; i < count_exp; i++)
		{
			if (animData->expressionKeyFrame.values[i] != 0.0f)
			{
				return i;
			}
		}
		//neutral mood
		return 7;
	
	}

	inline int GetExpressionValue(RE::Actor* a_actor)
	{
		SKSE::log::info("GetExpressionIValue({}) called", a_actor ? a_actor->GetName() : "NONE");
		
		if (a_actor == nullptr) return 0;

		RE::BSFaceGenAnimationData* animData = a_actor->GetFaceGenAnimationData();

		if (animData == nullptr) return 0;

		const size_t count_exp = animData->expressionKeyFrame.count;
		for (int i = 0; i < count_exp; i++)
		{
			if (animData->expressionKeyFrame.values[i] != 0.0f)
			{
				return static_cast<int>(animData->expressionKeyFrame.values[i]);
			}
		}

		//neutral value
		return 0;
	}

	inline int GetPhonemeValue(RE::Actor* a_actor, int phonemeId)
	{
		SKSE::log::info("GetPhonemeValue({}) called", a_actor ? a_actor->GetName() : "NONE");

		if (a_actor == nullptr) return 0;

		RE::BSFaceGenAnimationData* animData = a_actor->GetFaceGenAnimationData();

		if (animData == nullptr) return 0;
		if (phonemeId < 0 && phonemeId > 15) return 0;

		return static_cast<int>(animData->phenomeKeyFrame.values[phonemeId]);
		
	}

	inline int GetModifierValue(RE::Actor* a_actor, int modifierId)
	{
		SKSE::log::info("GetModifierValue({}) called", a_actor ? a_actor->GetName() : "NONE");

		if (a_actor == nullptr) return 0;

		RE::BSFaceGenAnimationData* animData = a_actor->GetFaceGenAnimationData();

		if (animData == nullptr) return 0;
		if (modifierId < 0 && modifierId > 13) return 0;

		return static_cast<int>(animData->modifierKeyFrame.values[modifierId]);
	}

    inline void SmoothSetPhoneme(RE::BSFaceGenAnimationData* animData, int a_id, int a_value, float a_speed, int a_delay)
	{
		int t1 = animData->phenomeKeyFrame.count ? static_cast<int>(std::lround(animData->phenomeKeyFrame.values[a_id] * 100.0f)) : 0;
		int t2 = 0;

		while (t1 != a_value) {
			t2 = (a_value - t1) / std::abs(a_value - t1);
			t1 = t1 + static_cast<int>(t2 * a_speed);

			if ((a_value - t1) / t2 < 0)
			{
				t1 = a_value;
			}

			animData->phenomeKeyFrame.SetValue(a_id, std::clamp(a_value, 0, 100) / 100.0f);

			std::this_thread::sleep_for(std::chrono::milliseconds{a_delay});
		}
	}

	inline int randomInt(int min, int max)
	{
		return min + rand() % (max - min + 1);
	}

	inline void SmoothSetModifier(RE::BSFaceGenAnimationData* animData, int mod1, int mod2, int str_dest, float a_speed, int a_delay)
	{
		int speed_blink_min = 25; 
		int speed_blink_max = 60;
		int speed_eye_move_min = 5;
		int speed_eye_move_max = 15;
		int speed_blink = 0;

		int t1 = animData->modifierKeyFrame.count ? std::lround(animData->modifierKeyFrame.values[mod1] * 100.0f) : 0;

		int t2;
		int t3;
		int speed;

		if (mod1 < 2) {
			if (str_dest > 0) {
				speed_blink = randomInt(speed_blink_min, speed_blink_max);
				speed = speed_blink;
			}
			else {
				if (speed_blink > 0) {
					speed = static_cast<int>(std::round(speed_blink * 0.5));
				}
				else {
					speed = static_cast<int>(std::round(randomInt(speed_blink_min, speed_blink_max) * 0.5));
				}
			}
		}
		else if (mod1 > 7 && mod1 < 12) {
			speed = randomInt(speed_eye_move_min, speed_eye_move_max);
		}
		else {
			speed = static_cast<int>(a_speed);
		}

		while (t1 != str_dest) {
			t2 = (str_dest - t1) / std::abs(str_dest - t1);
			t1 += t2 * speed;

			if ((str_dest - t1) / t2 < 0) {
				t1 = str_dest;
			}
			//simulate identical time for both e.g. brows change
			if (!(mod2 < 0 || mod2 > 13)) {
				t3 = randomInt(0, 1);
				animData->modifierKeyFrame.SetValue(mod1 * t3 + mod2 * (1 - t3), std::clamp(t1, 0, 100) / 100.0f);
				animData->modifierKeyFrame.SetValue(mod2 * t3 + mod1 * (1 - t3), std::clamp(t1, 0, 100) / 100.0f);

			}
			else {
				animData->modifierKeyFrame.SetValue(mod1, std::clamp(t1, 0, 100) / 100.0f);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds{a_delay});
		}
	}

	inline bool SetPhonemeModifierSmooth(RE::Actor* a_actor, int a_mode, int a_id1, int a_id2, int a_value, float a_speed, int a_time, RE::VMStackID a_stackId)
	{
		if (!a_actor) {
			return false;
		}

		if (!a_actor->GetFaceGenAnimationData()) {
			return false;
		}

		SKSE::GetTaskInterface()->AddTask([=]{
			bool result = true;

			if (auto animData = a_actor->GetFaceGenAnimationData()) {
				RE::BSSpinLockGuard locker(animData->lock);

				std::srand(static_cast<unsigned int>(std::time(nullptr)));
				
				SKSE::log::info("SetPhonemeModifierSmooth: entering loop");

				switch (a_mode) {
					case Mode::Reset: {
						animData->ClearExpressionOverride();
						animData->Reset(0.0f, true, true, true, false);
					}
					case Mode::Phoneme: {
						SmoothSetPhoneme(animData, a_id1, a_value, a_speed, a_time);
					}
					case Mode::Modifier: {
						SmoothSetModifier(animData, a_id1, a_id2, a_value, a_speed, a_time);
					}
					default: {
						result = false;
						break;
					}
				}

				SKSE::log::info("SetPhonemeModifierSmooth: exiting loop");

			} else {
				result = false;
			}
			
			RE::BSScript::Internal::VirtualMachine::GetSingleton()->ReturnLatentResult<bool>(a_stackId, result);
			
		});
		
		return true;
	}
	inline bool SmoothSetExpressionLocal(RE::BSFaceGenAnimationData* animData, int a_mood, int a_strength, int a_currentStrength, float a_modifier, float a_speed, int a_delay)
	{
		int exp_dest = static_cast<int>(a_strength * a_modifier);
		int exp_value = a_currentStrength;

		while (exp_value != exp_dest) {
			std::this_thread::sleep_for(std::chrono::milliseconds{ a_delay });

			int t2 = (exp_dest - exp_value) / std::abs(exp_dest - exp_value);

			exp_value = exp_value + static_cast<int>(t2 * a_speed);

			if ((exp_dest - exp_value) / t2 < 0) {
				exp_value = exp_dest;
			}
			animData->SetExpressionOverride(a_mood, static_cast<float>(exp_value));
		}
		return true;
	}

	inline bool SmoothSetExpression(RE::Actor* a_actor, int a_mood, int a_strength, int a_currentStrength, float a_modifier, float a_speed, int a_delay, RE::VMStackID a_stackId) {

		auto animData = a_actor->GetFaceGenAnimationData();

		if (!animData) {
			return false;
		}

		SKSE::GetTaskInterface()->AddTask([=] {

			int exp_dest = static_cast<int>(a_strength * a_modifier);
			int exp_value = a_currentStrength;
			SKSE::log::info("SmoothSetExpression: entering loop");
			RE::BSSpinLockGuard locker(animData->lock);
			
			while (exp_value != exp_dest) {
				
				std::this_thread::sleep_for(std::chrono::milliseconds{ a_delay });
			
				int t2 = (exp_dest - exp_value) / std::abs(exp_dest - exp_value);

				exp_value = exp_value + static_cast<int>(t2 * a_speed);

				if ((exp_dest - exp_value) / t2 < 0) {
					exp_value = exp_dest;
				}
				animData->SetExpressionOverride(a_mood, static_cast<float>(exp_value));
			}

			SKSE::log::info("SmoothSetExpression: exiting loop");

			RE::BSScript::Internal::VirtualMachine::GetSingleton()->ReturnLatentResult<bool>(a_stackId, true);
		});

		return true;
	}
	
	inline bool SmoothResetMFG(RE::Actor* a_actor, float a_speed, int a_delay, int a_stackId) {
		auto animData = a_actor->GetFaceGenAnimationData();

		if (!animData) {
			return false;
		}
		SKSE::GetTaskInterface()->AddTask([=] {
			RE::BSSpinLockGuard locker(animData->lock);

			// Blinks
			SmoothSetModifier(animData, 0, 1, 0, a_speed, a_delay);

			// Brows
			SmoothSetModifier(animData, 2, 3, 0, a_speed, a_delay);
			SmoothSetModifier(animData, 4, 5, 0, a_speed, a_delay);
			SmoothSetModifier(animData, 6, 7, 0, a_speed, a_delay);

			// Eyes
			SmoothSetModifier(animData, 8, -1, 0, a_speed, a_delay);
			SmoothSetModifier(animData, 9, -1, 0, a_speed, a_delay);
			SmoothSetModifier(animData, 10, -1, 0, a_speed, a_delay);
			SmoothSetModifier(animData, 11, -1, 0, a_speed, a_delay);

			// Squints
			SmoothSetModifier(animData, 12, 13, 0, a_speed, a_delay);

			// Mouth
			int p = 0;
			while (p <= 15) {
				SmoothSetPhoneme(animData, p, 0, a_speed, a_delay);
				p = p + 1;
			}

			// Expressions
			SmoothSetExpressionLocal(animData, GetExpressionId(a_actor), 0, GetExpressionValue(a_actor), 1, a_speed, a_delay);

			RE::BSScript::Internal::VirtualMachine::GetSingleton()->ReturnLatentResult<bool>(a_stackId, true);
		});
		

		return true;
	}
}