/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2026 TheSuperHackers
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <gtest/gtest.h>
#include <vector>

#include "GameLogic/AIPathfind.h"
#include "GameLogic/Pathfinder/PathfindCellList.h"

TEST(PathfindCell, ResetPreservesCoordinatesAndClearsState)
{
	PathfindCell cell, parent;
	cell.setPosition(123, 456);
	cell.initializeSearchData();
	cell.setParentCellHierarchical(&parent);
	cell.setCostSoFar(20);
	cell.setTotalCost(40);
	cell.reset();
	EXPECT_EQ(cell.getXIndex(), 123);
	EXPECT_EQ(cell.getYIndex(), 456);
	EXPECT_FALSE(cell.isInitialized());
	EXPECT_FALSE(cell.getOpen());
	EXPECT_FALSE(cell.getClosed());
	EXPECT_EQ(cell.getParentCell(), nullptr);
	EXPECT_EQ(cell.getCostSoFar(), 0u);
	EXPECT_EQ(cell.getTotalCost(), 0u);
	EXPECT_EQ(cell.getGoalUnit(), INVALID_ID);
	EXPECT_EQ(cell.getPosUnit(), INVALID_ID);
	EXPECT_EQ(cell.getGoalAircraft(), INVALID_ID);
}

TEST(PathfindCell, InitializeResetsScratchOnceAndPreservesActiveState)
{
	PathfindCell cell, parent;
	PathfindOpenHeap open;
	cell.setPosition(2, 3);
	cell.setParentCellHierarchical(&parent);
	cell.setCostSoFar(20);
	cell.setTotalCost(40);
	cell.setBlockedByAlly(true);
	cell.initializeSearchData();
	EXPECT_EQ(cell.getParentCell(), nullptr);
	EXPECT_EQ(cell.getCostSoFar(), 0u);
	EXPECT_EQ(cell.getTotalCost(), 0u);
	EXPECT_FALSE(cell.isBlockedByAlly());
	EXPECT_EQ(cell.getXIndex(), 2);
	EXPECT_EQ(cell.getYIndex(), 3);
	cell.setParentCellHierarchical(&parent);
	cell.setTotalCost(42);
	cell.putOnOpenList(open);
	cell.initializeSearchData();
	EXPECT_EQ(open.getHead(), &cell);
	EXPECT_TRUE(cell.getOpen());
	EXPECT_EQ(cell.getParentCell(), &parent);
	EXPECT_EQ(cell.getTotalCost(), 42u);
	PathfindCell::releaseOpenList(open);
}

TEST(PathfindCell, HeapPreservesPriorityAndStableTies)
{
	PathfindCell cells[5];
	PathfindOpenHeap open;
	const unsigned int total[] = {100, 100, 100, 90, 110};
	const unsigned int progress[] = {20, 50, 50, 10, 80};
	for (int i = 0; i != 5; ++i) {
		cells[i].setTotalCost(total[i]);
		cells[i].setCostSoFar(progress[i]);
		cells[i].putOnOpenList(open);
	}
	const int expected[] = {3, 1, 2, 0, 4};
	for (int i = 0; i != 5; ++i) {
		ASSERT_EQ(open.getHead(), &cells[expected[i]]);
		open.getHead()->removeFromOpenList(open);
		cells[expected[i]].clearSearchData();
		EXPECT_FALSE(cells[expected[i]].isInitialized());
	}
	EXPECT_TRUE(open.empty());
}

TEST(PathfindCell, ClosedCellCanReopenAndCleanupClearsParents)
{
	PathfindCell parent, child;
	PathfindOpenHeap open;
	PathfindCellList closed;
	parent.putOnOpenList(open);
	parent.removeFromOpenList(open);
	parent.putOnClosedList(closed);
	child.setParentCellHierarchical(&parent);
	child.putOnClosedList(closed);
	child.removeFromClosedList(closed);
	child.putOnOpenList(open);
	EXPECT_EQ(PathfindCell::releaseOpenList(open), 1);
	EXPECT_EQ(PathfindCell::releaseClosedList(closed), 1);
	EXPECT_TRUE(open.empty());
	EXPECT_TRUE(closed.empty());
	EXPECT_FALSE(parent.isInitialized());
	EXPECT_FALSE(child.isInitialized());
	EXPECT_EQ(child.getParentCell(), nullptr);
	EXPECT_EQ(child.getNextClosed(), nullptr);
	EXPECT_EQ(child.getPrevClosed(), nullptr);
}

TEST(PathfindCell, OccupancySurvivesSearchCleanupAndThenClears)
{
	PathfindCell cell, parent;
	PathfindCellList closed;
	const ICoord2D pos = {2, 3};
	const ObjectID unit = static_cast<ObjectID>(1);
	const ObjectID aircraft = static_cast<ObjectID>(2);
	cell.setPosition(pos.x, pos.y);
	cell.setGoalUnit(unit, pos);
	cell.setPosUnit(unit, pos);
	cell.setGoalAircraft(aircraft, pos);
	cell.setParentCellHierarchical(&parent);
	cell.putOnClosedList(closed);
	EXPECT_EQ(PathfindCell::releaseClosedList(closed), 1);
	EXPECT_EQ(cell.getGoalUnit(), unit);
	EXPECT_EQ(cell.getPosUnit(), unit);
	EXPECT_EQ(cell.getGoalAircraft(), aircraft);
	EXPECT_EQ(cell.getParentCell(), nullptr);
	EXPECT_TRUE(cell.isInitialized());
	cell.setGoalUnit(INVALID_ID, pos);
	cell.setPosUnit(INVALID_ID, pos);
	cell.setGoalAircraft(INVALID_ID, pos);
	EXPECT_FALSE(cell.isInitialized());
}

TEST(PathfindCell, OccupancyChangesDoNotDamageActiveSearch)
{
	PathfindCell cell, parent;
	PathfindOpenHeap open;
	const ICoord2D pos = {0, 0};
	cell.setGoalUnit(static_cast<ObjectID>(1), pos);
	cell.setParentCellHierarchical(&parent);
	cell.setTotalCost(42);
	cell.putOnOpenList(open);
	cell.setGoalUnit(INVALID_ID, pos);
	EXPECT_EQ(open.getHead(), &cell);
	EXPECT_TRUE(cell.getOpen());
	EXPECT_EQ(cell.getParentCell(), &parent);
	EXPECT_EQ(cell.getTotalCost(), 42u);
	PathfindCell::releaseOpenList(open);
	EXPECT_FALSE(cell.isInitialized());
	EXPECT_EQ(cell.getParentCell(), nullptr);
}

TEST(PathfindCell, MoreThanThirtyThousandCellsCanRemainActiveAndBeReused)
{
	std::vector<PathfindCell> cells(30001);
	PathfindCellList closed;
	for (int iteration = 0; iteration != 2; ++iteration) {
		for (size_t i = 0; i != cells.size(); ++i) {
			EXPECT_FALSE(cells[i].isInitialized());
			cells[i].putOnClosedList(closed);
		}
		EXPECT_EQ(PathfindCell::releaseClosedList(closed), 30001);
		EXPECT_TRUE(closed.empty());
	}
}

TEST(PathfindCell, HeapMatchesReferenceAcrossMixedOperationsAndDebugTraversal)
{
	const int count = 257;
	PathfindCell cells[count];
	bool active[count] = {};
	unsigned int sequence[count] = {};
	unsigned int random = 12345;
	PathfindOpenHeap open;
	for (unsigned int step = 1; step <= 10000; ++step) {
		random = random * 1664525u + 1013904223u;
		const int index = (random >> 8) % count;
		random = random * 1664525u + 1013904223u;
		// The search currently changes costs before removing and reinserting an open cell.
		cells[index].setTotalCost((random >> 8) % 100);
		cells[index].setCostSoFar((random >> 16) % 50);
		if (active[index]) cells[index].removeFromOpenList(open);
		if ((random >> 24) & 1) {
			cells[index].putOnOpenList(open);
			sequence[index] = step;
			active[index] = true;
		} else {
			cells[index].clearSearchData();
			active[index] = false;
		}
		int best = -1;
		unsigned int activeCount = 0;
		for (int i = 0; i != count; ++i) {
			EXPECT_EQ(cells[i].getOpen(), active[i]);
			if (!active[i]) continue;
			++activeCount;
			if (best == -1 || cells[i].getTotalCost() < cells[best].getTotalCost() ||
				(cells[i].getTotalCost() == cells[best].getTotalCost() &&
				(cells[i].getCostSoFar() > cells[best].getCostSoFar() ||
				(cells[i].getCostSoFar() == cells[best].getCostSoFar() && sequence[i] < sequence[best])))) best = i;
		}
		ASSERT_EQ(open.size(), activeCount);
		ASSERT_EQ(open.getHead(), best == -1 ? nullptr : &cells[best]);
		// This is the same non-destructive traversal used to draw the debug grid.
		bool seen[count] = {};
		for (UnsignedInt i = 0; i < open.size(); ++i) {
			const int cellIndex = (int)(open.getCell(i) - cells);
			EXPECT_TRUE(active[cellIndex]);
			EXPECT_FALSE(seen[cellIndex]);
			seen[cellIndex] = true;
		}
	}
	while (!open.empty()) {
		int best = -1;
		for (int i = 0; i != count; ++i) {
			if (!active[i]) continue;
			if (best == -1 || cells[i].getTotalCost() < cells[best].getTotalCost() ||
				(cells[i].getTotalCost() == cells[best].getTotalCost() &&
				(cells[i].getCostSoFar() > cells[best].getCostSoFar() ||
				(cells[i].getCostSoFar() == cells[best].getCostSoFar() && sequence[i] < sequence[best])))) best = i;
		}
		ASSERT_EQ(open.getHead(), &cells[best]);
		cells[best].removeFromOpenList(open);
		cells[best].clearSearchData();
		active[best] = false;
	}
	cells[0].putOnOpenList(open);
	open.reset();
	EXPECT_TRUE(open.empty());
	for (int i = 0; i != count; ++i) EXPECT_FALSE(cells[i].getOpen());
}

TEST(PathfindCell, HeapClearAndReuseBeyondFormerPoolLimit)
{
	std::vector<PathfindCell> cells(30001);
	PathfindOpenHeap open;
	for (int iteration = 0; iteration != 2; ++iteration) {
		for (size_t i = 0; i != cells.size(); ++i) {
			cells[i].setTotalCost((UnsignedInt)(i % 100));
			cells[i].putOnOpenList(open);
		}
		EXPECT_EQ(PathfindCell::releaseOpenList(open), 30001);
		EXPECT_TRUE(open.empty());
		for (size_t i = 0; i != cells.size(); ++i) EXPECT_FALSE(cells[i].isInitialized());
	}
}
