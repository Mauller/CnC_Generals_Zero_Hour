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

#include "GameLogic/Object.h"
#include "GameLogic/Pathfinder/PathfindCell.h"
#include "GameLogic/Pathfinder/PathfindCellList.h"
#include "GameLogic/Pathfinder/PathfindConstants.h"
#include "GameLogic/Module/BodyModule.h"

/**
 * Constructor
 */
PathfindCell::PathfindCell()
{
	m_pos.x = m_pos.y = 0;
	reset();
}

/**
 * Reset the cell to default values
 */
void PathfindCell::reset()
{
	m_type = CELL_CLEAR;
	m_flags = NO_UNITS;
	m_zone = 0;
	m_aircraftGoal = false;
	m_pinched = false;
	m_nextClosed = m_prevClosed = m_pathParent = nullptr;
	m_initialized = m_open = m_closed = false;
	m_heapIndex = UINT_MAX;
	m_openSequence = 0;
	m_totalCost = m_costSoFar = 0;
	m_goalUnitID = m_posUnitID = m_goalAircraftID = INVALID_ID;
	m_obstacleID = INVALID_ID;
	m_blockedByAlly = false;
	m_obstacleIsFence = false;
	m_obstacleIsTransparent = false;

	m_connectsToLayer = LAYER_INVALID;
	m_layer = LAYER_GROUND;

}

/**
 * Reset the pathfinding values in the cell.
 */
Bool PathfindCell::startPathfind( PathfindCell *goalCell  )
{
	m_nextClosed = nullptr;
	m_prevClosed = nullptr;
	m_pathParent = nullptr;
	m_costSoFar = 0;		// start node, no cost to get here
	m_totalCost = 0;
	if (goalCell) {
		m_totalCost = costToGoal( goalCell );
	}
	m_initialized = true;
	m_open = FALSE;
	m_closed = FALSE;
	return true;
}

/**
 * Set the blocked by ally flag on the cell.
 */
Bool PathfindCell::isBlockedByAlly() const
{
	return m_blockedByAlly;
}

void PathfindCell::setBlockedByAlly(Bool blocked)
{
	m_blockedByAlly = (blocked != 0);
}

/**
 * Set the parent pointer.
 */
void PathfindCell::setParentCell( PathfindCell* parent  )
{
	m_pathParent = parent;
	Int dx = m_pos.x - parent->m_pos.x;
	Int dy = m_pos.y - parent->m_pos.y;
	if (dx<-1 || dx>1 || dy<-1 || dy>1) {
		DEBUG_CRASH(("Invalid parent index."));
	}
}

/**
 * Set the parent pointer.
 */
void PathfindCell::setParentCellHierarchical( PathfindCell* parent  )
{
	m_pathParent = parent;
}

/**
 * Reset the parent cell.
 */
void PathfindCell::clearParentCell(  )
{
	m_pathParent = nullptr;
}

// Initialize scratch data on first use; repeated visits must retain active search state.
void PathfindCell::initializeSearchData()
{
	if (m_initialized) return;
	DEBUG_ASSERTCRASH(!m_open && !m_closed && !m_nextClosed && !m_prevClosed, ("Shouldn't be linked."));
	m_nextClosed = m_prevClosed = m_pathParent = nullptr;
	m_open = m_closed = false;
	m_totalCost = m_costSoFar = 0;
	m_blockedByAlly = false;
	m_heapIndex = UINT_MAX;
	m_openSequence = 0;
	m_initialized = true;
}

// Clear search scratch data without affecting terrain, coordinates, or occupancy.
void PathfindCell::clearSearchData()
{
	// Occupancy updates can occur while a search is active. Leave linked cells intact.
	if (m_open || m_closed) return;
	DEBUG_ASSERTCRASH(!m_nextClosed && !m_prevClosed, ("Shouldn't be linked."));
	m_pathParent = nullptr;
	if (m_flags != NO_UNITS || m_aircraftGoal) return;
	m_initialized = false;
	m_totalCost = m_costSoFar = 0;
}

/**
 * Sets the goal unit for a cell.
 */
void PathfindCell::setGoalUnit(ObjectID unitID, const ICoord2D &pos )
{
	if (unitID != INVALID_ID) initializeSearchData();
	m_goalUnitID = unitID;
	if (unitID == INVALID_ID) {
		m_flags = m_posUnitID == INVALID_ID ? NO_UNITS : UNIT_PRESENT_MOVING;
		clearSearchData();
	} else if (unitID == m_posUnitID) {
		m_flags = UNIT_PRESENT_FIXED;
	} else {
		m_flags = m_posUnitID == INVALID_ID ? UNIT_GOAL : UNIT_GOAL_OTHER_MOVING;
	}
}

/**
 * Sets the goal aircraft for a cell.
 */
void PathfindCell::setGoalAircraft(ObjectID unitID, const ICoord2D &pos )
{
	if (unitID != INVALID_ID) initializeSearchData();
	m_goalAircraftID = unitID;
	m_aircraftGoal = unitID != INVALID_ID;
	if (unitID == INVALID_ID) clearSearchData();
}

/**
 * Sets the position unit for a cell.
 */
void PathfindCell::setPosUnit(ObjectID unitID, const ICoord2D &pos )
{
	if (unitID != INVALID_ID && m_goalUnitID != INVALID_ID && m_goalUnitID == m_posUnitID) {
		return; // A unit is already occupying this cell.
	}
	if (unitID != INVALID_ID) initializeSearchData();
	m_posUnitID = unitID;
	if (unitID == INVALID_ID) {
		m_flags = m_goalUnitID == INVALID_ID ? NO_UNITS : UNIT_GOAL;
		clearSearchData();
	} else if (unitID == m_goalUnitID) {
		m_flags = UNIT_PRESENT_FIXED;
	} else {
		m_flags = m_goalUnitID == INVALID_ID ? UNIT_PRESENT_MOVING : UNIT_GOAL_OTHER_MOVING;
	}
}

/**
 * Return the relevant obstacle ID.
 */
ObjectID PathfindCell::getObstacleID() const
{
	return m_obstacleID;
}

/**
 * Flag this cell as an obstacle, from the given one.
 * Return true if cell was flagged.
 */
Bool PathfindCell::setTypeAsObstacle( Object *obstacle, Bool isFence, const ICoord2D &pos )
{
	if (m_type!=CELL_CLEAR && m_type != CELL_IMPASSABLE) {
		return false;
	}

	Bool isRubble = false;
	if (obstacle->getBodyModule() && obstacle->getBodyModule()->getDamageState() == BODY_RUBBLE)
	{
		isRubble = true;
	}

	if (isRubble) {
		m_type = CELL_RUBBLE;
		m_obstacleID = INVALID_ID;
		m_obstacleIsFence = false;
		m_obstacleIsTransparent = false;
		return true;
	}

	m_type = CELL_OBSTACLE;
	m_obstacleID = obstacle->getID();
	m_obstacleIsFence = isFence;
	m_obstacleIsTransparent = obstacle->isKindOf(KINDOF_CAN_SEE_THROUGH_STRUCTURE);
	return true;
}

/**
 * Flag this cell as given type.
 */
void PathfindCell::setType( CellType type )
{
	if (m_obstacleID != INVALID_ID) {
		DEBUG_ASSERTCRASH(type == CELL_OBSTACLE, ("Wrong type."));
		m_type = CELL_OBSTACLE;
		return;
	}
	m_type = type;
}

/**
 * Unflag this cell as an obstacle, from the given one.
 * Return true if this cell was previously flagged as an obstacle by this object.
 */
Bool PathfindCell::removeObstacle( Object *obstacle )
{
	if (m_type == CELL_RUBBLE) {
		m_type = CELL_CLEAR;
	}
	if (m_obstacleID != obstacle->getID()) return false;
	m_type = CELL_CLEAR;
	m_obstacleID = INVALID_ID;
	m_obstacleIsFence = false;
	m_obstacleIsTransparent = false;
	return true;
}

void PathfindCell::putOnOpenList(PathfindOpenHeap &list)
{
	list.insert(*this);
}

void PathfindCell::removeFromOpenList(PathfindOpenHeap &list)
{
	list.remove(*this);
}

Int PathfindCell::releaseOpenList(PathfindOpenHeap &list)
{
	return list.clear();
}

/// remove all cells from "closed" list
Int PathfindCell::releaseClosedList( PathfindCellList &list )
{
	Int count = 0;
	while (list.m_head) {
		count++;
		DEBUG_ASSERTCRASH(list.m_head->m_closed==TRUE && list.m_head->m_open==FALSE, ("Serious error - Invalid flags. jba"));
		PathfindCell *cur = list.m_head;

		if (cur->m_nextClosed) {
			list.m_head = cur->m_nextClosed;
		} else {
			list.reset();
		}
		cur->m_nextClosed = nullptr;
		cur->m_prevClosed = nullptr;
		cur->m_closed = FALSE;
		cur->clearSearchData();
	}
	return count;
}

/// put self on "closed" list, return new list
void PathfindCell::putOnClosedList( PathfindCellList &list )
{
	DEBUG_ASSERTCRASH(m_closed==FALSE && m_open==FALSE, ("Serious error - Invalid flags. jba"));
	// only put on list if not already on it
	if (m_closed == FALSE)
	{
		m_initialized = true;
		m_closed = TRUE;

		m_prevClosed = nullptr;
		m_nextClosed = list.m_head;
		if (list.m_head)
			list.m_head->m_prevClosed = this;

		list.m_head = this;
	}

}

/// remove self from "closed" list
void PathfindCell::removeFromClosedList( PathfindCellList &list )
{
	DEBUG_ASSERTCRASH(m_closed==TRUE && m_open==FALSE, ("Serious error - Invalid flags. jba"));
	if (m_nextClosed)
		m_nextClosed->m_prevClosed = m_prevClosed;

	if (m_prevClosed)
		m_prevClosed->m_nextClosed = m_nextClosed;
	else
		list.m_head = getNextClosed();

	m_closed = false;
	m_nextClosed = nullptr;
	m_prevClosed = nullptr;

}

/**
 * Return true if the given object ID is registered as an obstacle in this cell
 */
Bool PathfindCell::isObstaclePresent(ObjectID objID) const
{
	if (objID != INVALID_ID && (getType() == CELL_OBSTACLE))
	{
		return m_obstacleID == objID;
	}

	return false;
}

/**
 * return true if the obstacle in the cell is KINDOF_CAN_SEE_THROUGHT_STRUCTURE
 */
Bool PathfindCell::isObstacleTransparent() const
{
	return m_obstacleIsTransparent;
}

/**
 * return true if the given obstacle in the cell is a fence.
 */
Bool PathfindCell::isObstacleFence() const
{
	return m_obstacleIsFence;
}

UnsignedInt PathfindCell::costToGoal( PathfindCell *goal )
{
	Int dx = m_pos.x - goal->getXIndex();
	Int dy = m_pos.y - goal->getYIndex();
	if (dx<0) dx = -dx;
	if (dy<0) dy = -dy;

	// Use diagonal steps for the shared distance, then orthogonal steps for the remainder.
	// This is the exact unobstructed eight-direction cost, excluding turn penalties.
	const Int diagonalSteps = dx < dy ? dx : dy;
	const Int orthogonalSteps = dx > dy ? dx - dy : dy - dx;
	return COST_DIAGONAL * diagonalSteps + COST_ORTHOGONAL * orthogonalSteps;
}

UnsignedInt PathfindCell::costToHierGoal( PathfindCell *goal )
{
	Int dx = m_pos.x - goal->getXIndex();
	Int dy = m_pos.y - goal->getYIndex();
	if (dx < 0) dx = -dx;
	if (dy < 0) dy = -dy;

	// Use diagonal steps for the shared distance, then orthogonal steps for the remainder.
	// This is the exact unobstructed eight-direction cost, excluding turn penalties.
	const Int diagonalSteps = dx < dy ? dx : dy;
	const Int orthogonalSteps = dx > dy ? dx - dy : dy - dx;
	return COST_DIAGONAL * diagonalSteps + COST_ORTHOGONAL * orthogonalSteps;
}

UnsignedInt PathfindCell::costSoFar( PathfindCell *parent )
{
	// very first node in path - no turns, no cost
	if (parent == nullptr)
		return 0;

	// add in number of turns in path so far
	ICoord2D prevDir;
	Int cost;

	prevDir.x = parent->getXIndex() - m_pos.x;
	prevDir.y = parent->getYIndex() - m_pos.y;

	// diagonal moves cost a bit more than orthogonal ones
	if (prevDir.x == 0 || prevDir.y == 0)
		cost = parent->getCostSoFar() + COST_ORTHOGONAL;
	else
		cost = parent->getCostSoFar() + COST_DIAGONAL;
	if (getPinched()) {
		cost += 1*COST_DIAGONAL;
	}

#if 1
	// Increase cost of turns.
	Int numTurns = 0;
	PathfindCell *prevCell = parent->getParentCell();
	if (prevCell) {

		ICoord2D dir;
		dir.x = prevCell->getXIndex() - parent->getXIndex();
		dir.y = prevCell->getYIndex() - parent->getYIndex();

		// count number of direction changes
		if (dir.x != prevDir.x || dir.y != prevDir.y)
		{
			Int dot = dir.x * prevDir.x + dir.y * prevDir.y;
			if (dot > 0)
				numTurns=4;				// 45 degree turn
			else if (dot == 0)
				numTurns = 8;		// 90 degree turn
			else
				numTurns = 16;		// 135 degree turn
		}
	}

	return cost + numTurns;
#else
	return cost;
#endif

}

Bool PathfindCell::typesMatch(const PathfindCell& targetCell, const PathfindCell& sourceCell)
{
	CellType targetType = targetCell.getType();
	CellType srcType = sourceCell.getType();
	if (targetType == srcType)
	{
		return true;
	}

	return false;
}

Bool PathfindCell::waterGround(const PathfindCell& targetCell, const PathfindCell& sourceCell)
{
	CellType targetType = targetCell.getType();
	CellType srcType = sourceCell.getType();
	if ((targetType == CELL_CLEAR &&
	     (srcType & CELL_WATER)))
	{
		return true;
	}
	if ((srcType == CELL_CLEAR &&
	     (targetType & CELL_WATER)))
	{
		return true;
	}

	return false;
}

Bool PathfindCell::groundRubble(const PathfindCell& targetCell, const PathfindCell& sourceCell)
{
	CellType targetType = targetCell.getType();
	CellType srcType = sourceCell.getType();
	if ((targetType == CELL_CLEAR &&
	     (srcType == CELL_RUBBLE)))
	{
		return true;
	}
	if ((srcType == CELL_CLEAR &&
	     (targetType == CELL_RUBBLE)))
	{
		return true;
	}

	return false;
}

Bool PathfindCell::terrain(const PathfindCell& targetCell, const PathfindCell& sourceCell)
{
	Int targetType = targetCell.getType();
	Int srcType = sourceCell.getType();
	if (targetType == CELL_OBSTACLE)
	{
		targetType = CELL_CLEAR;
	}
	if (srcType == CELL_OBSTACLE)
	{
		srcType = CELL_CLEAR;
	}
	if (targetType == srcType)
	{
		return true;
	}
	return false;
}

Bool PathfindCell::crusherGround(const PathfindCell& targetCell, const PathfindCell& sourceCell)
{
	Int targetType = targetCell.getType();
	Int srcType = sourceCell.getType();
	if (targetType == CELL_OBSTACLE)
	{
		if (targetCell.isObstacleFence())
		{
			if (srcType == CELL_CLEAR)
			{
				return true;
			}
		}
	}
	if (srcType == CELL_OBSTACLE)
	{
		if (sourceCell.isObstacleFence())
		{
			if (targetType == CELL_CLEAR)
			{
				return true;
			}
		}
	}
	return false;
}

Bool PathfindCell::groundCliff(const PathfindCell& targetCell, const PathfindCell& sourceCell)
{
	CellType targetType = targetCell.getType();
	CellType srcType = sourceCell.getType();

	if ((targetType == CELL_CLIFF) &&
	    (srcType == CELL_CLEAR))
	{
		return true;
	}
	if ((targetType == CELL_CLEAR) &&
	    (srcType == CELL_CLIFF))
	{
		return true;
	}
	return false;
}
