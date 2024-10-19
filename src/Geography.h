#pragma once

#include "Util.h"

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

	struct CellEdge
	{
		const RE::TESObjectREFR* interior;
		const RE::TESObjectREFR* exterior;
		float dist;
	};
	using CellPath = std::vector<CellEdge>;
	using CellTraversal = std::unordered_map<RE::TESObjectCELL*, CellPath>;

	inline CellTraversal TraverseInteriorCellStartingAt(RE::TESObjectCELL* a_cell, const int a_depth, const RE::TESObjectREFR* a_ref = nullptr)
	{
		CellTraversal seen;

		if (!a_cell->IsInteriorCell())
			return seen;

		seen.insert({ a_cell, CellPath{ CellEdge{ nullptr, a_ref, 0.f } } });

		std::queue<RE::TESObjectCELL*> traverse;
		traverse.push(a_cell);

		logger::info("beginning traversal");

		while (!traverse.empty()) {
			const auto cell = traverse.front();
			traverse.pop();

			const auto& path = seen[cell];
			const auto& [_, entry, dist] = path.back();
			const auto& entryPos = entry ? entry->GetPosition() : RE::NiPoint3{};

			if (!cell->IsInteriorCell() || path.size() + 1 > a_depth) {
				continue;
			}

			// list of all adjacent cells and the closest door
			std::unordered_map<RE::TESObjectCELL*, CellEdge> adjacent;

			cell->ForEachReference([&adjacent, &entryPos, &seen, dist](RE::TESObjectREFR* a_ref) {
				const auto xTeleport = a_ref->extraList.GetByType<RE::ExtraTeleport>();

				if (const auto teleportData = xTeleport ? xTeleport->teleportData : nullptr) {
					logger::info("found teleport data on {}", Util::HexString(a_ref->GetFormID()));

					if (const auto linkedDoorRef = teleportData->linkedDoor) {
						if (const auto linkedDoorPtr = linkedDoorRef.get()) {
							if (const auto linkedDoor = linkedDoorPtr.get()) {
								auto parentCell = linkedDoor->GetParentCell();

								if (!parentCell) {
									parentCell = linkedDoor->GetSaveParentCell();
								}

								if (parentCell) {
									if (!seen.contains(parentCell)) {
										const auto& currDist = a_ref->GetPosition().GetDistance(entryPos);
										if (!adjacent.contains(parentCell) || adjacent[parentCell].interior->GetPosition().GetDistance(entryPos) > currDist) {
											adjacent[parentCell] = CellEdge{ a_ref, linkedDoor, dist + currDist };
										}
									}
								}
							}
						}
					}
				}

				return RE::BSContainer::ForEachResult::kContinue;
			});

			for (const auto& [curr, door] : adjacent) {
				seen[curr].push_back(door);
				traverse.push(curr);
				logger::info("found adjacent cell: {}", curr->GetFormEditorID());
			}
		}

		seen.erase(a_cell);

		std::string contents;
		for (const auto& [cell, path] : seen) {
			contents += "\n";
			contents += cell->GetFormEditorID();
			contents += " = ";

			for (const auto& edge : path) {
				contents += std::format("{}:{}:{}",
					edge.interior ? Util::HexString(edge.interior->GetFormID()) : "none",
					edge.exterior ? Util::HexString(edge.exterior->GetFormID()) : "none",
					edge.dist);
			}
		}

		logger::info("total adjacent cells: {}{}", seen.size(), contents);

		return seen;
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

	inline float GetDistance(const RE::TESObjectREFR* a_ref1, const RE::TESObjectREFR* a_ref2)
	{
		return GetRealPosition(a_ref1).GetDistance(GetRealPosition(a_ref2));
	}

	const CellEdge* GetClosestExteriorCellEdge(const CellTraversal& a_paths)
	{
		float minDist = -1;
		const CellEdge* closestEdge = nullptr;
		for (const auto& [cell, path] : a_paths) {
			if (cell->IsExteriorCell()) {
				const auto& currDist = path.back().dist;
				if (minDist < 0.f || currDist < minDist) {
					minDist = currDist;
					closestEdge = &path.back();
				}
			}
		}

		return closestEdge;
	}

	const CellEdge* GetClosestExteriorCellEdge(const RE::TESObjectREFR* a_ref)
	{
		const auto& paths = TraverseInteriorCellStartingAt(a_ref->GetParentCell(), 10, a_ref);
		return GetClosestExteriorCellEdge(paths);
	}

	inline float GetIntExtDistance(const RE::TESObjectREFR* a_interior, const RE::TESObjectREFR* a_exterior)
	{
		logger::info("GetIntExtDistance: {} - {}", Util::HexString(a_interior->GetFormID()), Util::HexString(a_exterior->GetFormID()));

		if (const auto& closestEdge = GetClosestExteriorCellEdge(a_interior)) {
			const auto& ref1ToDoor = closestEdge->dist;
			const auto& doorToRef2 = GetDistance(a_exterior, closestEdge->exterior);

			logger::info("found exterior cell: {} + {} = {}", ref1ToDoor, doorToRef2, ref1ToDoor + doorToRef2);
			return ref1ToDoor + doorToRef2;
		}

		return -1;
	}

	inline float GetIntIntDistance(const RE::TESObjectREFR* a_interior1, const RE::TESObjectREFR* a_interior2)
	{
		logger::info("GetIntIntDistance: {} - {}", Util::HexString(a_interior1->GetFormID()), Util::HexString(a_interior2->GetFormID()));

		const auto& paths = TraverseInteriorCellStartingAt(a_interior1->GetParentCell(), 10, a_interior1);

		const auto& found = paths.find(a_interior2->GetParentCell());
		if (found != paths.end()) {
			const auto& door = found->second.back();
			const auto& ref1ToDoor = door.dist;
			const auto& doorToRef2 = door.exterior->GetPosition().GetDistance(a_interior2->GetPosition());

			logger::info("interior cells are linked: {} + {} = {}", ref1ToDoor, doorToRef2, ref1ToDoor + doorToRef2);
			return ref1ToDoor + doorToRef2;
		}

		if (const auto& ext1 = GetClosestExteriorCellEdge(paths)) {
			logger::info("found closest edge for ref 1");
			if (const auto& ext2 = GetClosestExteriorCellEdge(a_interior2)) {
				logger::info("found closest edge for ref 2");
				const auto& ref1ToDoor = ext1->dist;
				const auto& door1ToDoor2 = GetDistance(ext1->exterior, ext2->exterior);
				const auto& door2ToRef = ext2->dist;

				logger::info("unlinked interior cells: {} + {} + {} = {}", ref1ToDoor, door1ToDoor2, door2ToRef, ref1ToDoor + door1ToDoor2 + door2ToRef);
				return ref1ToDoor + door1ToDoor2 + door2ToRef;
			}
		}

		return -1;
	}

	inline float GetDistanceBetween(const RE::TESObjectREFR* a_ref1, const RE::TESObjectREFR* a_ref2)
	{
		auto ref1 = a_ref1;
		auto ref2 = a_ref2;

		const auto cell1 = a_ref1->GetParentCell();
		const auto cell2 = a_ref2->GetParentCell();

		if (cell1 == cell2) {
			logger::info("in same cell: {}", GetRealPosition(ref1).GetDistance(GetRealPosition(ref2)));
			return GetRealPosition(ref1).GetDistance(GetRealPosition(ref2));
		}

		if (cell1->IsInteriorCell() && !cell2->IsInteriorCell()) {
			return GetIntExtDistance(a_ref1, a_ref2);
		} else if (!cell1->IsInteriorCell() && cell2->IsInteriorCell()) {
			return GetIntExtDistance(a_ref2, a_ref1);
		} else if (cell1->IsInteriorCell() && cell2->IsInteriorCell()) {
			return GetIntIntDistance(a_ref1, a_ref2);
		}

		return GetDistance(a_ref1, a_ref2);
	}
}