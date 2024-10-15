#pragma once

namespace Geography
{
	// stolen from: https://github.com/alexsylex/CompassNavigationOverhaul/blob/main/include/utils/Geometry.h
	inline RE::NiPoint3 GetRealPosition(const RE::TESObjectREFR* a_objRef)
	{
		RE::NiPoint3 position = a_objRef->GetPosition();

		if (const RE::TESWorldSpace* worldSpace = a_objRef->GetWorldspace()) {
			RE::NiPoint3 worldSpaceOffset{ worldSpace->worldMapOffsetData.mapOffsetX,
				worldSpace->worldMapOffsetData.mapOffsetY,
				worldSpace->worldMapOffsetData.mapOffsetZ };

			position += worldSpaceOffset * worldSpace->worldMapOffsetData.mapScale;
		}

		return position;
	}

	inline float GetDistanceBetween(const RE::TESObjectREFR* a_ref1, const RE::TESObjectREFR* a_ref2)
	{
		RE::NiPoint3 ref1Pos = GetRealPosition(a_ref1);
		RE::NiPoint3 ref2Pos = GetRealPosition(a_ref2);	

		return ref1Pos.GetDistance(ref2Pos);
	}

	inline float GetDistanceBetween(const RE::TESObjectREFR* a_ref, const RE::NiPoint3 a_pos)
	{
		RE::NiPoint3 refPos = GetRealPosition(a_ref);
		return refPos.GetDistance(a_pos);
	}

	inline float GetDistanceBetween(const RE::NiPoint3 a_pos1, const RE::NiPoint3 a_pos2)
	{
		return a_pos1.GetDistance(a_pos2);
	}
}