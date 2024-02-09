#pragma once

namespace PyramidUtils::Expression {
	enum Mode : std::int32_t
	{
		Reset = -1,
		Phoneme,
		Modifier,
		ExpressionValue,
		ExpressionId
	};

    void SmoothSetPhoneme(RE::BSFaceGenAnimationData* animData, int a_id, int a_value)
	{
		int t1 = animData->phenomeKeyFrame.count ? static_cast<int>(std::lround(animData->phenomeKeyFrame.values[a_id] * 100.0f)) : 0;
		int t2 = 0;
		const int speed = 5;

		while (t1 != a_value) {
			t2 = (a_value - t1) / std::abs(a_value - t1);
			t1 += t2 * speed;

			if ((a_value - t1) / t2 < 0)
			{
				t1 = a_value;
			}

			animData->phenomeKeyFrame.SetValue(a_id, std::clamp(a_value, 0, 100) / 100.0f);
		}
	}

	int randomInt(int min, int max)
	{
		return min + rand() % (max - min + 1);
	}

	void SmoothSetModifier(RE::BSFaceGenAnimationData* animData, int mod1, int mod2, int str_dest)
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
			speed = 5;
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
		}
	}

	bool SetPhonemeModifierSmooth(RE::Actor* a_actor, std::int32_t a_mode, std::uint32_t a_id1, std::uint32_t a_id2, std::int32_t a_value)
	{
		if (!a_actor) {
			return false;
		}

		auto animData = a_actor->GetFaceGenAnimationData();

		if (!animData) {
			return false;
		}

		RE::BSSpinLockGuard locker(animData->lock);

		std::srand(static_cast<unsigned int>(std::time(nullptr)));

		switch (a_mode) {
            case Expression::Mode::Reset: {
                animData->ClearExpressionOverride();
                animData->Reset(0.0f, true, true, true, false);
                return true;
            }
            case Expression::Mode::Phoneme: {
                Expression::SmoothSetPhoneme(animData, a_id1, a_value);
                return true;
            }
            case Expression::Mode::Modifier: {
                Expression::SmoothSetModifier(animData, a_id1, a_id2, a_value);
                return true;
            }
		}

		return false;
	}

	int SmoothSetExpression(RE::Actor* a_actor, int a_mood, int a_strength, int a_currentStrength = 0, float a_modifier = 1.0, float a_speed = 2.0) {
        int t2;
        a_strength = static_cast<int>(a_strength * a_modifier);

		auto animData = a_actor->GetFaceGenAnimationData();

		if (!animData) {
			return false;
		}

		RE::BSSpinLockGuard locker(animData->lock);

        while (a_currentStrength != a_strength) {
            t2 = (a_strength - a_currentStrength) / std::abs(a_strength - a_currentStrength);
            a_currentStrength = a_currentStrength + t2 * a_speed;

            if ((a_strength - a_currentStrength) / t2 < 0) {
                a_currentStrength = a_strength;
            }

            animData->SetExpressionOverride(a_mood, a_currentStrength);
        }

        return a_currentStrength;
    }
}