#pragma once

namespace Geography
{
	inline std::vector<RE::TESWorldSpace*> GetWorldSpaces(RE::TESObjectCELL* a_cell)
	{
		if (a_cell->IsExteriorCell()) {
			return std::vector<RE::TESWorldSpace*>{ a_cell->GetRuntimeData().worldSpace };
		}

		std::unordered_set<RE::TESWorldSpace*> result;

		std::unordered_set<RE::TESObjectCELL*> seen;
		std::queue<RE::TESObjectCELL*> q;

		seen.insert(a_cell);
		q.push(a_cell);

		while (!q.empty()) {
			const auto& cell = q.back();
			q.pop();

			cell->ForEachReference([&result, &q, &seen](RE::TESObjectREFR* a_ref) {
				const auto xTeleport = a_ref->extraList.GetByType<RE::ExtraTeleport>();

				if (const auto teleportData = xTeleport ? xTeleport->teleportData : nullptr) {
					if (const auto linkedDoorRef = teleportData->linkedDoor) {
						if (const auto linkedDoorPtr = linkedDoorRef.get()) {
							if (const auto linkedDoor = linkedDoorPtr.get()) {
								auto parentCell = linkedDoor->GetParentCell();

								if (!parentCell) {
									parentCell = linkedDoor->GetSaveParentCell();
								}

								if (parentCell) {
									if (parentCell->IsExteriorCell()) {
										if (const auto& world = parentCell->GetRuntimeData().worldSpace) {
											logger::info("world {} from doors: {}-{}", world->GetFormEditorID(), a_ref->GetFormID(), linkedDoor->GetFormID());
											result.insert(world);
										}
									} else if (!seen.contains(parentCell)) {
										q.push(parentCell);
										seen.insert(parentCell);
									}
								} else {
									logger::info("parent nor save parent cell found");

									RE::NiPoint3 pos;
									RE::NiPoint3 rot;
									RE::TESForm* loc = nullptr;

									linkedDoor->GetEditorLocation2(pos, rot, loc, nullptr);

									if (loc) {
										if (const auto& world = loc->As<RE::TESWorldSpace>()) {
											result.insert(world);
										} else if (const auto& nextCell = loc->As<RE::TESObjectCELL>()) {
											if (!seen.contains(nextCell)) {
												q.push(nextCell);
												seen.insert(nextCell);
											}
										}
									}
								}
							}
						}
					}
				}

				return RE::BSContainer::ForEachResult::kContinue;
			});
		}

		return std::vector<RE::TESWorldSpace*>{ result.begin(), result.end() };
	}

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
}