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

Bool PathfindCellList::canReverseSort(PathfindCell& currentCell) const
{
	if (m_head[0] && m_tail[0])
		return m_head[0]->getTotalCostDifference(currentCell) > m_tail[0]->getTotalCostDifference(currentCell);

	return false;
}

void PathfindCellList::init()
{
	for (Int i = 0; i < SKIP_LEVELS; ++i)
	{
		m_head[i] = nullptr;
		m_tail[i] = nullptr;
	}
}
