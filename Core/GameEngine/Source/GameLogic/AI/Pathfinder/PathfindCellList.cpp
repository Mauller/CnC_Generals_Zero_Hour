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

#include "PreRTS.h"
#include "GameLogic/Pathfinder/PathfindCell.h"
#include "GameLogic/Pathfinder/PathfindCellList.h"

Bool PathfindOpenHeap::precedes(const PathfindCell* a, const PathfindCell* b) const
{
	if (a->hasHigherPriorityThan(*b)) return true;
	if (b->hasHigherPriorityThan(*a)) return false;
	return a->m_openSequence < b->m_openSequence;
}

void PathfindOpenHeap::siftUp(UnsignedInt index)
{
	PathfindCell* cell = m_cells[index];
	while (index > 0) {
		const UnsignedInt parent = (index - 1) / 4;
		if (!precedes(cell, m_cells[parent])) break;
		m_cells[index] = m_cells[parent];
		m_cells[index]->m_heapIndex = index;
		index = parent;
	}
	m_cells[index] = cell;
	cell->m_heapIndex = index;
}

void PathfindOpenHeap::siftDown(UnsignedInt index)
{
	PathfindCell* cell = m_cells[index];
	const UnsignedInt count = size();
	// This form also avoids overflowing 4 * index + 1 for leaf nodes.
	while (count > 1 && index <= (count - 2) / 4) {
		const UnsignedInt firstChild = 4 * index + 1;
		UnsignedInt best = firstChild;
		const UnsignedInt childCount = count - firstChild < 4 ? count - firstChild : 4;
		for (UnsignedInt offset = 1; offset < childCount; ++offset) {
			const UnsignedInt child = firstChild + offset;
			if (precedes(m_cells[child], m_cells[best])) best = child;
		}
		if (!precedes(m_cells[best], cell)) break;
		m_cells[index] = m_cells[best];
		m_cells[index]->m_heapIndex = index;
		index = best;
	}
	m_cells[index] = cell;
	cell->m_heapIndex = index;
}

void PathfindOpenHeap::insert(PathfindCell& cell)
{
	DEBUG_ASSERTCRASH(!cell.m_open && !cell.m_closed, ("Cell is already on a list."));
	DEBUG_ASSERTCRASH(cell.m_heapIndex == UINT_MAX, ("Cell already has a heap index."));
	const UnsignedInt index = size();
	DEBUG_ASSERTCRASH(index < UINT_MAX, ("Heap index overflow."));
	m_cells.push_back(&cell);
	cell.m_initialized = true;
	cell.m_open = true;
	cell.m_openSequence = m_nextSequence++;
	siftUp(index);
}

void PathfindOpenHeap::remove(PathfindCell& cell)
{
	const UnsignedInt index = cell.m_heapIndex;
	DEBUG_ASSERTCRASH(cell.m_open && !cell.m_closed && index < size() && m_cells[index] == &cell,
		("Cell does not belong to this heap."));
	PathfindCell* last = m_cells.back();
	m_cells.pop_back();
	cell.m_open = false;
	cell.m_heapIndex = UINT_MAX;
	if (index == size()) return;
	m_cells[index] = last;
	last->m_heapIndex = index;
	if (index > 0 && precedes(last, m_cells[(index - 1) / 4])) siftUp(index);
	else siftDown(index);
}

Int PathfindOpenHeap::clear()
{
	const Int count = (Int)size();
	for (UnsignedInt i = 0; i < size(); ++i) {
		PathfindCell* cell = m_cells[i];
		cell->m_open = false;
		cell->m_heapIndex = UINT_MAX;
		cell->clearSearchData();
	}
	m_cells.clear(); // Retain capacity for the next search.
	m_nextSequence = 0;
	return count;
}

void PathfindOpenHeap::reset()
{
	clear();
}
