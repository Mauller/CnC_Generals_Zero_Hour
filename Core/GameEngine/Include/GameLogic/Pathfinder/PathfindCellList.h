/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2025 Electronic Arts Inc.
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

#pragma once

#include <vector>

class PathfindCell;

// Intrusive closed list: constant-time removal when a cell is reopened.
class PathfindCellList
{
	friend class PathfindCell;
public:
	PathfindCellList() : m_head(nullptr) {}
	void reset() { m_head = nullptr; }
	PathfindCell* getHead() const { return m_head; }
	Bool empty() const { return m_head == nullptr; }
private:
	PathfindCell* m_head;
};

// Indexed 4-ary min heap. Array order is suitable for debug traversal, not sorted traversal.
class PathfindOpenHeap
{
public:
	PathfindOpenHeap() : m_nextSequence(0) {}
	Bool empty() const { return m_cells.empty(); }
	UnsignedInt size() const { return (UnsignedInt)m_cells.size(); }
	void reserve(UnsignedInt cellCount) { m_cells.reserve(cellCount); }
	PathfindCell* getHead() const { return empty() ? nullptr : m_cells[0]; }
	PathfindCell* getCell(UnsignedInt index) const { return m_cells[index]; }
	void insert(PathfindCell& cell);
	void remove(PathfindCell& cell);
	Int clear();
	void reset();

private:
	PathfindOpenHeap(const PathfindOpenHeap&);
	PathfindOpenHeap& operator=(const PathfindOpenHeap&);
	Bool precedes(const PathfindCell* a, const PathfindCell* b) const;
	void siftUp(UnsignedInt index);
	void siftDown(UnsignedInt index);
	std::vector<PathfindCell*> m_cells;
	UnsignedInt64 m_nextSequence;
};
