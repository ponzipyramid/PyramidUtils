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
	inline int randomInt(int min, int max)
	{
		return min + rand() % (max - min + 1);
	}
	inline int getDelayPlusRandom(int a_delay)
	{
		return a_delay + randomInt(0, a_delay);
	}
	inline int getShortenedDelay(int a_delay)
	{
		return a_delay/5 + randomInt(0, a_delay/3);
	}

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
				return static_cast<int>(animData->expressionKeyFrame.values[i] * 100.0f);
			}
		}

		//neutral value - 0
		return 0;
	}

	inline int GetPhonemeValue(RE::Actor* a_actor, int phonemeId)
	{
		SKSE::log::info("GetPhonemeValue({}) called", a_actor ? a_actor->GetName() : "NONE");

		if (a_actor == nullptr) return 0;

		RE::BSFaceGenAnimationData* animData = a_actor->GetFaceGenAnimationData();

		if (animData == nullptr) return 0;
		if (phonemeId < 0 && phonemeId > 15) return 0;

		return static_cast<int>(animData->phenomeKeyFrame.values[phonemeId] * 100.0f);
		
	}

	inline int GetModifierValue(RE::Actor* a_actor, int modifierId)
	{
		SKSE::log::info("GetModifierValue({}) called", a_actor ? a_actor->GetName() : "NONE");

		if (a_actor == nullptr) return 0;

		RE::BSFaceGenAnimationData* animData = a_actor->GetFaceGenAnimationData();

		if (animData == nullptr) return 0;
		if (modifierId < 0 && modifierId > 13) return 0;

		return static_cast<int>(animData->modifierKeyFrame.values[modifierId] * 100.0f);
	}

	inline bool ResetMFG(RE::BSFaceGenAnimationData* animData)
	{
		animData->lock.Lock();
		animData->ClearExpressionOverride();
		animData->Reset(0.0f, true, true, true, false);
		animData->lock.Unlock();
		return true;
	}

    inline void SmoothSetPhoneme(RE::BSFaceGenAnimationData* animData, int a_id, int a_value, float a_speed, int a_delay)
	{
		int t1 = animData->phenomeKeyFrame.count ? static_cast<int>(std::lround(animData->phenomeKeyFrame.values[a_id] * 100.0f)) : 0;
		int t2 = 0;

		if (t1 != a_value) {
			while (t1 != a_value) {
				t2 = (a_value - t1) / std::abs(a_value - t1);
				t1 = t1 + static_cast<int>(t2 * a_speed);

				if ((a_value - t1) / t2 < 0) {
					t1 = a_value;
				}
				animData->lock.Lock();
				animData->phenomeKeyFrame.SetValue(a_id, std::clamp(t1, 0, 100) / 100.0f);
				animData->lock.Unlock();
				std::this_thread::sleep_for(std::chrono::milliseconds{ getDelayPlusRandom(a_delay) });
			}
		}
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

		if (t1 == str_dest) {
			return;
		}

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
				animData->lock.Lock();
				animData->modifierKeyFrame.SetValue(mod1 * t3 + mod2 * (1 - t3), std::clamp(t1, 0, 100) / 100.0f);
				animData->modifierKeyFrame.SetValue(mod2 * t3 + mod1 * (1 - t3), std::clamp(t1, 0, 100) / 100.0f);
				animData->lock.Unlock();
				std::this_thread::sleep_for(std::chrono::milliseconds{ getShortenedDelay(a_delay)});
			}
			else {
				animData->lock.Lock();
				animData->modifierKeyFrame.SetValue(mod1, std::clamp(t1, 0, 100) / 100.0f);
				animData->lock.Unlock();
				std::this_thread::sleep_for(std::chrono::milliseconds{ getDelayPlusRandom(a_delay) });
			}
			
			
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

		std::thread t([=]() {
			bool result = true;

			auto animData = a_actor->GetFaceGenAnimationData();

				std::srand(static_cast<unsigned int>(std::time(nullptr)));

				SKSE::log::info("SetPhonemeModifierSmooth: entering loop");

				switch (a_mode) {
				case Mode::Reset:
					{
						ResetMFG(animData);
					}
				case Mode::Phoneme:
					{
						SmoothSetPhoneme(animData, a_id1, a_value, a_speed, a_time);
					}
				case Mode::Modifier:
					{
						SmoothSetModifier(animData, a_id1, a_id2, a_value, a_speed, a_time);
					}
				default:
					{
						result = false;
						break;
					}
				}

				SKSE::log::info("SetPhonemeModifierSmooth: exiting loop");

			RE::BSScript::Internal::VirtualMachine::GetSingleton()->ReturnLatentResult<bool>(a_stackId, result);
		});
		t.detach();
		
		return true;
	}
	inline bool SmoothSetExpression(RE::BSFaceGenAnimationData* animData, int a_mood, int a_strength, int a_currentStrength, float a_modifier, float a_speed, int a_delay)
	{
		int exp_dest = static_cast<int>(a_strength * a_modifier);
		int exp_value = a_currentStrength;
		
		if (exp_value != exp_dest) {
			while (exp_value != exp_dest) {
				int t2 = (exp_dest - exp_value) / std::abs(exp_dest - exp_value);

				exp_value = exp_value + static_cast<int>(t2 * a_speed);

				if ((exp_dest - exp_value) / t2 < 0) {
					exp_value = exp_dest;
				}
				animData->lock.Lock();
				animData->SetExpressionOverride(a_mood, static_cast<float>(exp_value));
				animData->lock.Unlock();
				std::this_thread::sleep_for(std::chrono::milliseconds{ getDelayPlusRandom(a_delay) });
			}
		}
		return true;
	}

	inline bool SetExpressionSmooth(RE::Actor* a_actor, int a_mood, int a_strength, int a_currentStrength, float a_modifier, float a_speed, int a_delay, RE::VMStackID a_stackId)
	{
		auto animData = a_actor->GetFaceGenAnimationData();

		if (!animData) {
			return false;
		}
		std::thread t([=]() {
			SmoothSetExpression(animData, a_mood, a_strength, a_currentStrength, a_modifier, a_speed, a_delay);
			RE::BSScript::Internal::VirtualMachine::GetSingleton()->ReturnLatentResult<bool>(a_stackId, true);
		});
		t.detach();

		return true;
	}


	inline bool ResetMFGSmooth(RE::Actor* a_actor, float a_speed, int a_delay, int a_stackId) {
		auto animData = a_actor->GetFaceGenAnimationData();

		if (!animData) {
			return false;
		}
		std::thread t([=]() {
			//RE::BSSpinLockGuard locker(animData->lock);

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
			SmoothSetExpression(animData, GetExpressionId(a_actor), 0, GetExpressionValue(a_actor), 1, a_speed, a_delay);

	

			RE::BSScript::Internal::VirtualMachine::GetSingleton()->ReturnLatentResult<bool>(a_stackId, true);
		});
		t.detach();

		return true;
	}

	inline bool ApplyExpressionPreset(RE::Actor* a_actor, std::vector<float> a_expression, bool a_openMouth, int exprPower, float exprStrModifier, float modStrModifier, float phStrModifier, float a_speed, int a_delay, RE::VMStackID a_stackId)
	{
		SKSE::log::info("ApplyExpressionRaw({}) called", a_actor ? a_actor->GetName() : "NONE");

		if (a_actor == nullptr)
		{
			SKSE::log::error("No actor selected");
			return false;
		}
			
		if (a_expression.size() != 32) {
			SKSE::log::error("Expression is of incorrect size - returning");
			return false;
		}
		auto animData = a_actor->GetFaceGenAnimationData();

		if (!animData) {
			SKSE::log::error("No animdata found");
			return false;
		}

		std::thread t([=]() {

			int i = 0;
			int p = 0;
			int m = 0;

			// Set expression
			int exprNum = static_cast<int>(a_expression[30]);
			int exprStrResult = static_cast<int>(a_expression[31] * 100.0 * exprStrModifier);

			// dynamic exprPower for non angry expressions
			if (exprNum > 0) {
				if (exprStrResult == 0) {
					exprStrResult = exprPower;
				}
			}
			if (!a_openMouth) {
				SmoothSetExpression(animData, exprNum, exprStrResult, 0, exprStrModifier, a_speed, a_delay);
			}
			// Set Phoneme
			while (p <= 15) {
				if (!a_openMouth && GetPhonemeValue(a_actor, p) != a_expression[i]) {
					SmoothSetPhoneme(animData, p, static_cast<int>(a_expression[i] * 100.0 * phStrModifier), a_speed, a_delay);
				}
				++i;
				++p;
			}

			while (m <= 13) {
				if (GetModifierValue(a_actor, m) != a_expression[i]) {
					// both eyes involved
					if (m == 0 || m == 2 || m == 4 || m == 6 || m == 12) {
						if (a_expression[i] == a_expression[i + 1]) {
							SmoothSetModifier(animData, m, m + 1, static_cast<int>(a_expression[i] * 100.0 * modStrModifier), a_speed, a_delay);
							++i;
							++m;
						} else {
							SmoothSetModifier(animData, m, -1, static_cast<int>(a_expression[i] * 100.0 * modStrModifier), a_speed, a_delay);
						}
					} else {
						SmoothSetModifier(animData, m, -1, static_cast<int>(a_expression[i] * 100.0 * modStrModifier), a_speed, a_delay);
					}
				}
				++i;
				++m;
			}

			RE::BSScript::Internal::VirtualMachine::GetSingleton()->ReturnLatentResult<bool>(a_stackId, true);
		});
		t.detach();

		return true;
	}
}