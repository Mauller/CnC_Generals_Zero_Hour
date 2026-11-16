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
#include "GameLogic/Pathfinder/PathfindCellInfo.h"
#include "GameLogic/Pathfinder/PathfindCellList.h"
#include "GameLogic/Pathfinder/PathfindConstants.h"
#include "GameLogic/Module/BodyModule.h"

/**
 * Constructor
 */
PathfindCell::PathfindCell() :m_info(nullptr)
{
	reset();
}

/**
 * Destructor
 */
PathfindCell::~PathfindCell()
{
	if (m_info) PathfindCellInfo::releaseACellInfo(m_info);
	m_info = nullptr;
	static Bool warn = true;
	if (warn) {
		warn = false;
		DEBUG_LOG( ("PathfindCell::~PathfindCell m_info Allocated."));
	}
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
	if (m_info) {
		PathfindCellInfo::releaseACellInfo(m_info);
		m_info = nullptr;
	}
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
	DEBUG_ASSERTCRASH(m_info, ("Has to have info."));
	m_info->m_nextOpen = nullptr;
	m_info->m_prevOpen = nullptr;
	m_info->m_pathParent = nullptr;
	m_info->m_costSoFar = 0;		// start node, no cost to get here
	m_info->m_totalCost = 0;
	if (goalCell) {
		m_info->m_totalCost = costToGoal( goalCell );
	}
	{
		m_info->m_open = FALSE;
	}
	m_info->m_closed = FALSE;
	return true;
}

/**
 * Set the blocked by ally flag on the pathfind cell info.
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
 * Determine absolute total path cost difference between two cells.
 * Returns UINT_MAX if used with an uninitialised cell, so will be sorted as maximally dissimilar.
 */
UnsignedInt PathfindCell::getTotalCostDifference(PathfindCell& other) const
{
	if (m_info && other.m_info)
		return abs((Int)m_info->m_totalCost - (Int)other.m_info->m_totalCost);

	return UINT_MAX;
}

/**
 * Set the parent pointer.
 */
void PathfindCell::setParentCell( PathfindCell* parent  )
{
	DEBUG_ASSERTCRASH(m_info, ("Has to have info."));
	m_info->m_pathParent = parent->m_info;
	Int dx = m_info->m_pos.x - parent->m_info->m_pos.x;
	Int dy = m_info->m_pos.y - parent->m_info->m_pos.y;
	if (dx<-1 || dx>1 || dy<-1 || dy>1) {
		DEBUG_CRASH(("Invalid parent index."));
	}
}

/**
 * Set the parent pointer.
 */
void PathfindCell::setParentCellHierarchical( PathfindCell* parent  )
{
	DEBUG_ASSERTCRASH(m_info, ("Has to have info."));
	m_info->m_pathParent = parent->m_info;
}

/**
 * Reset the parent cell.
 */
void PathfindCell::clearParentCell(  )
{
	DEBUG_ASSERTCRASH(m_info, ("Has to have info."));
	m_info->m_pathParent = nullptr;
}

/**
 * Allocates an info record for a cell.
 */
Bool PathfindCell::allocateInfo( const ICoord2D &pos )
{
	if (!m_info) {
		m_info = PathfindCellInfo::getACellInfo(this, pos);
		return (m_info != nullptr);
	}
	return true;
}

/**
 * Releases an info record for a cell.
 */
void PathfindCell::releaseInfo()
{
	// TheSuperHackers @bugfix Mauller/SkyAero 05/06/2025 Parent cell links need clearing to prevent dangling pointers on starting points that can link them to an invalid parent cell.
	// Parent cells are only cleared within Pathfinder::prependCells, so cells that do not make it onto the final path do not get their parent cell cleared.
	// Cells with a special flags also do not get their PathfindCellInfo cleared and therefore can leave a parent cell set on a starting cell.
	if (m_info) {
		m_info->m_pathParent = nullptr;
	}

	if (m_flags != NO_UNITS || m_aircraftGoal) {
		return;
	}

	if (!m_info) {
		return;
	}

	DEBUG_ASSERTCRASH(m_info->m_prevOpen==nullptr && m_info->m_nextOpen==nullptr, ("Shouldn't be linked."));
	DEBUG_ASSERTCRASH(m_info->m_open==0 && m_info->m_closed==0, ("Shouldn't be linked."));
	DEBUG_ASSERTCRASH(m_info->m_goalUnitID==INVALID_ID && m_info->m_posUnitID==INVALID_ID, ("Shouldn't be occupied."));
	DEBUG_ASSERTCRASH(m_info->m_goalAircraftID==INVALID_ID , ("Shouldn't be occupied by aircraft."));
	if (m_info->m_prevOpen || m_info->m_nextOpen || m_info->m_open || m_info->m_closed) {
		// Bad release.  Skip for now, better leak than crash.  jba.
		return;
	}

	PathfindCellInfo::releaseACellInfo(m_info);
	m_info = nullptr;

}

/**
 * Sets the goal unit into the info record for a cell.
 */
void PathfindCell::setGoalUnit(ObjectID unitID, const ICoord2D &pos )
{
	if (unitID==INVALID_ID) {
		// removing goal.
		if (m_info) {
			m_info->m_goalUnitID = INVALID_ID;
			if (m_info->m_posUnitID == INVALID_ID) {
				// No units here.
				DEBUG_ASSERTCRASH(m_flags==UNIT_GOAL, ("Bad flags."));
				m_flags = NO_UNITS;
				releaseInfo();
			} else{
				m_flags = UNIT_PRESENT_MOVING;
			}
		}	else {
			DEBUG_ASSERTCRASH(m_flags == NO_UNITS, ("Bad flags."));
		}
	} else {
		// adding goal.
		if (!m_info) {
			DEBUG_ASSERTCRASH(m_flags == NO_UNITS, ("Bad flags."));
			allocateInfo(pos);
		}
		if (!m_info) {
			DEBUG_CRASH(("Ran out of pathfind cells - fatal error!!!!! jba."));
			return;
		}
		m_info->m_goalUnitID = unitID;
		if (unitID==m_info->m_posUnitID) {
			m_flags = UNIT_PRESENT_FIXED;
		} else if (m_info->m_posUnitID==INVALID_ID) {
			m_flags = UNIT_GOAL;
		}	else {
			m_flags = UNIT_GOAL_OTHER_MOVING;
		}
	}
}

/**
 * Sets the goal aircraft into the info record for a cell.
 */
void PathfindCell::setGoalAircraft(ObjectID unitID, const ICoord2D &pos )
{
	if (unitID==INVALID_ID) {
		// removing goal.
		if (m_info) {
			m_info->m_goalAircraftID = INVALID_ID;
			m_aircraftGoal = false;
			releaseInfo();
		}	else {
			DEBUG_ASSERTCRASH(m_aircraftGoal==false, ("Bad flags."));
		}
	} else {
		// adding goal.
		if (!m_info) {
			DEBUG_ASSERTCRASH(m_aircraftGoal==false, ("Bad flags."));
			allocateInfo(pos);
		}
		if (!m_info) {
			DEBUG_CRASH(("Ran out of pathfind cells - fatal error!!!!! jba."));
			return;
		}
		m_info->m_goalAircraftID = unitID;
		m_aircraftGoal = true;
	}
}

/**
 * Sets the position unit into the info record for a cell.
 */
void PathfindCell::setPosUnit(ObjectID unitID, const ICoord2D &pos )
{
	if (unitID==INVALID_ID) {
		// removing position.
		if (m_info) {
			m_info->m_posUnitID = INVALID_ID;
			if (m_info->m_goalUnitID == INVALID_ID) {
				// No units here.
				DEBUG_ASSERTCRASH(m_flags==UNIT_PRESENT_MOVING, ("Bad flags."));
				m_flags = NO_UNITS;
				releaseInfo();
			}	else {
				m_flags = UNIT_GOAL;
			}
		}	else {
			DEBUG_ASSERTCRASH(m_flags == NO_UNITS, ("Bad flags."));
		}
	} else {
		// adding goal.
		if (!m_info) {
			DEBUG_ASSERTCRASH(m_flags == NO_UNITS, ("Bad flags."));
			allocateInfo(pos);
		}
		if (!m_info) {
			DEBUG_CRASH(("Ran out of pathfind cells - fatal error!!!!! jba."));
			return;
		}
		if (m_info->m_goalUnitID!=INVALID_ID && (m_info->m_goalUnitID==m_info->m_posUnitID)) {
			// A unit is already occupying this cell.
			return;
		}
		m_info->m_posUnitID = unitID;
		if (unitID==m_info->m_goalUnitID) {
			m_flags = UNIT_PRESENT_FIXED;
		} else if (m_info->m_goalUnitID==INVALID_ID) {
			m_flags = UNIT_PRESENT_MOVING;
		}	else {
			m_flags = UNIT_GOAL_OTHER_MOVING;
		}
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

// Forward insertion sort, returns early if the list is being initialized or we are prepending the list
void PathfindCell::forwardInsertionSort(PathfindCellList& list)
{
	DEBUG_ASSERTCRASH(m_info, ("Has to have info."));
	DEBUG_ASSERTCRASH(m_info->m_closed == FALSE && m_info->m_open == FALSE, ("Serious error - Invalid flags. jba"));

	// mark the new cell as being on the open list
	m_info->m_open = true;
	m_info->m_closed = false;

	if (list.m_head == nullptr) {
		m_info->m_prevOpen = nullptr;
		m_info->m_nextOpen = nullptr;
		list.m_head = this;
		list.m_tail = this;
		return;
	}

	// If the node needs inserting before the current list head
	if (m_info->hasHigherPriorityThan(*list.m_head->m_info)) {
		m_info->m_prevOpen = nullptr;
		list.m_head->m_info->m_prevOpen = this->m_info;
		m_info->m_nextOpen = list.m_head->m_info;
		list.m_head = this;
		return;
	}

	// Traverse the list to find correct position
	PathfindCell* current = list.m_head;
	while (current->m_info->m_nextOpen && !m_info->hasHigherPriorityThan(*current->m_info->m_nextOpen)) {
		current = current->getNextOpen();
	}

	// Insert the new node in the correct position
	m_info->m_nextOpen = current->m_info->m_nextOpen;
	if (current->m_info->m_nextOpen != nullptr) {
		current->m_info->m_nextOpen->m_prevOpen = this->m_info;
	}
	else {
		list.m_tail = this;
	}

	current->m_info->m_nextOpen = this->m_info;
	m_info->m_prevOpen = current->m_info;
}

// Reverse insertion sort, returns early if the list is being initialized or we are appending the list
void PathfindCell::reverseInsertionSort(PathfindCellList& list)
{
	DEBUG_ASSERTCRASH(m_info, ("Has to have info."));
	DEBUG_ASSERTCRASH(m_info->m_closed == FALSE && m_info->m_open == FALSE, ("Serious error - Invalid flags. jba"));

	// mark the new cell as being on the open list
	m_info->m_open = true;
	m_info->m_closed = false;

	if (list.m_tail == nullptr) {
		m_info->m_prevOpen = nullptr;
		m_info->m_nextOpen = nullptr;
		list.m_tail = this;
		list.m_head = this;
		return;
	}

	// If the node needs inserting after the current list tail
	if (!m_info->hasHigherPriorityThan(*list.m_tail->m_info)) {
		m_info->m_prevOpen = list.m_tail->m_info;
		list.m_tail->m_info->m_nextOpen = this->m_info;
		m_info->m_nextOpen = nullptr;
		list.m_tail = this;
		return;
	}

	// Traverse the list to find correct position
	PathfindCell* current = list.m_tail;
	while (current->m_info->m_prevOpen && m_info->hasHigherPriorityThan(*current->m_info->m_prevOpen)) {
		current = current->getPrevOpen();
	}

	// Insert the new node in the correct position
	m_info->m_prevOpen = current->m_info->m_prevOpen;
	if (current->m_info->m_prevOpen != nullptr) {
		current->m_info->m_prevOpen->m_nextOpen = this->m_info;
	}
	else {
		list.m_head = this;
	}

	current->m_info->m_prevOpen = this->m_info;
	m_info->m_nextOpen = current->m_info;
}

/// put self on "open" list in ascending cost order, return new list
void PathfindCell::putOnSortedOpenList( PathfindCellList &list )
{

	// TheSuperHackers @performance Mauller 20/03/2026 Implement reverse insertion sorting.
	// Long and complex paths often append PathfindCell's, with high total path costs, to the open list.
	// Appending and reverse traversal allow faster insertion of these cells, reducing pathfinding overhead by 50 - 66%.
	if (list.canReverseSort(*this)) {
		reverseInsertionSort(list);
	}
	else {
		forwardInsertionSort(list);
	}
}

/// remove self from "open" list
void PathfindCell::removeFromOpenList( PathfindCellList &list )
{
	DEBUG_ASSERTCRASH(m_info, ("Has to have info."));
	DEBUG_ASSERTCRASH(m_info->m_closed==FALSE && m_info->m_open==TRUE, ("Serious error - Invalid flags. jba"));
	if (m_info->m_nextOpen)
		m_info->m_nextOpen->m_prevOpen = m_info->m_prevOpen;
	else {
		list.m_tail = getPrevOpen();
	}

	if (m_info->m_prevOpen)
		m_info->m_prevOpen->m_nextOpen = m_info->m_nextOpen;
	else
		list.m_head = getNextOpen();

	m_info->m_open = false;
	m_info->m_nextOpen = nullptr;
	m_info->m_prevOpen = nullptr;

}

/// remove all cells from "open" list
Int PathfindCell::releaseOpenList( PathfindCellList &list )
{
	Int count = 0;
	while (list.m_head) {
		count++;
		DEBUG_ASSERTCRASH(list.m_head->m_info, ("Has to have info."));
		DEBUG_ASSERTCRASH(list.m_head->m_info->m_closed==FALSE && list.m_head->m_info->m_open==TRUE, ("Serious error - Invalid flags. jba"));
		PathfindCell *cur = list.m_head;
		PathfindCellInfo *curInfo = list.m_head->m_info;

		if (curInfo->m_nextOpen) {
			list.m_head = curInfo->m_nextOpen->m_cell;
		} else {
			list.reset();
		}
		DEBUG_ASSERTCRASH(cur == curInfo->m_cell, ("Bad backpointer in PathfindCellInfo"));
		curInfo->m_nextOpen = nullptr;
		curInfo->m_prevOpen = nullptr;
		curInfo->m_open = FALSE;
		cur->releaseInfo();
	}
	return count;
}

/// remove all cells from "closed" list
Int PathfindCell::releaseClosedList( PathfindCellList &list )
{
	Int count = 0;
	while (list.m_head) {
		count++;
		DEBUG_ASSERTCRASH(list.m_head->m_info, ("Has to have info."));
		DEBUG_ASSERTCRASH(list.m_head->m_info->m_closed==TRUE && list.m_head->m_info->m_open==FALSE, ("Serious error - Invalid flags. jba"));
		PathfindCell *cur = list.m_head;
		PathfindCellInfo *curInfo = list.m_head->m_info;

		if (curInfo->m_nextOpen) {
			list.m_head = curInfo->m_nextOpen->m_cell;
		} else {
			list.reset();
		}
		DEBUG_ASSERTCRASH(cur == curInfo->m_cell, ("Bad backpointer in PathfindCellInfo"));
		curInfo->m_nextOpen = nullptr;
		curInfo->m_prevOpen = nullptr;
		curInfo->m_closed = FALSE;
		cur->releaseInfo();
	}
	return count;
}

/// put self on "closed" list, return new list
void PathfindCell::putOnClosedList( PathfindCellList &list )
{
	DEBUG_ASSERTCRASH(m_info, ("Has to have info."));
	DEBUG_ASSERTCRASH(m_info->m_closed==FALSE && m_info->m_open==FALSE, ("Serious error - Invalid flags. jba"));
	// only put on list if not already on it
	if (m_info->m_closed == FALSE)
	{
		m_info->m_closed = FALSE;
		m_info->m_closed = TRUE;

		m_info->m_prevOpen = nullptr;
		m_info->m_nextOpen = list.m_head ? list.m_head->m_info : nullptr;
		if (list.m_head)
			list.m_head->m_info->m_prevOpen = this->m_info;

		list.m_head = this;
	}

}

/// remove self from "closed" list
void PathfindCell::removeFromClosedList( PathfindCellList &list )
{
	DEBUG_ASSERTCRASH(m_info, ("Has to have info."));
	DEBUG_ASSERTCRASH(m_info->m_closed==TRUE && m_info->m_open==FALSE, ("Serious error - Invalid flags. jba"));
	if (m_info->m_nextOpen)
		m_info->m_nextOpen->m_prevOpen = m_info->m_prevOpen;

	if (m_info->m_prevOpen)
		m_info->m_prevOpen->m_nextOpen = m_info->m_nextOpen;
	else
		list.m_head = getNextOpen();

	m_info->m_closed = false;
	m_info->m_nextOpen = nullptr;
	m_info->m_prevOpen = nullptr;

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
	DEBUG_ASSERTCRASH(m_info, ("Has to have info."));
	Int dx = m_info->m_pos.x - goal->getXIndex();
	Int dy = m_info->m_pos.y - goal->getYIndex();
#define NO_REAL_DIST
#ifdef REAL_DIST
	Int cost = COST_ORTHOGONAL*WWMath::Sqrt((float)(dx*dx + dy*dy));
#else
	if (dx<0) dx = -dx;
	if (dy<0) dy = -dy;
	Int cost;
	if (dx>dy) {
		cost= COST_ORTHOGONAL*dx + (COST_ORTHOGONAL*dy)/2;
	}	else {
		cost= COST_ORTHOGONAL*dy + (COST_ORTHOGONAL*dx)/2;
	}

#endif

	return cost;
}

UnsignedInt PathfindCell::costToHierGoal( PathfindCell *goal )
{
	if( !m_info )
	{
		DEBUG_CRASH( ("Has to have info.") );
		return 100000; //...patch hack 1.01
	}
	Int dx = m_info->m_pos.x - goal->getXIndex();
	Int dy = m_info->m_pos.y - goal->getYIndex();
	Int cost = REAL_TO_INT_FLOOR(COST_ORTHOGONAL*WWMath::Sqrt((float)(dx*dx + dy*dy)) + 0.5f);
	return cost;
}

UnsignedInt PathfindCell::costSoFar( PathfindCell *parent )
{
	DEBUG_ASSERTCRASH(m_info, ("Has to have info."));
	// very first node in path - no turns, no cost
	if (parent == nullptr)
		return 0;

	// add in number of turns in path so far
	ICoord2D prevDir;
	Int cost;

	prevDir.x = parent->getXIndex() - m_info->m_pos.x;
	prevDir.y = parent->getYIndex() - m_info->m_pos.y;

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
