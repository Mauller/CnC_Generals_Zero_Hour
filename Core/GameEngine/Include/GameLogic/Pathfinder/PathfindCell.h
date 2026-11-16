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

#include "PathfindConstants.h"

class Object;
class PathfindCellList;
class PathfindOpenHeap;

typedef UnsignedShort zoneStorageType;

/**
 * This represents one cell in the pathfinding grid.
 * These cells categorize the world into idealized cellular states,
 * and are also used for efficient A* pathfinding.
 * @todo Optimize memory usage of pathfind grid.
 */
class PathfindCell
{
	friend class PathfindOpenHeap;
public:

	enum CellType
	{
		CELL_CLEAR		= 0x00,									///< clear, unobstructed ground
		CELL_WATER		= 0x01,									///< water area
		CELL_CLIFF		= 0x02,									///< steep altitude change
		CELL_RUBBLE		= 0x03,									///< Cell is occupied by rubble.
		CELL_OBSTACLE	= 0x04,									///< Occupied by a structure
		CELL_BRIDGE_IMPASSABLE = 0x05,				///< Piece of a bridge that is impassable.
		CELL_IMPASSABLE = 0x06								///< Just plain impassable except for aircraft.
	};

	enum CellFlags
	{
		NO_UNITS		= 0x00,						///< No units in this cell.
		UNIT_GOAL		= 0x01,						///< A unit is heading to this cell.
		UNIT_PRESENT_MOVING	= 0x02,		///< A unit is moving through this cell.
		UNIT_PRESENT_FIXED	= 0x03,		///< A unit is stationary in this cell.
		UNIT_GOAL_OTHER_MOVING	= 0x05		///< A unit is moving through this cell, and another unit has this as it's goal.
	};

	/// reset the cell
	void reset();

	PathfindCell();

	Bool setTypeAsObstacle( Object *obstacle, Bool isFence, const ICoord2D &pos );				///< flag this cell as an obstacle, from the given one
	Bool removeObstacle( Object *obstacle );				///< unflag this cell as an obstacle, from the given one
	void setType( CellType type );	///< set the cell type
	CellType getType() const { return (CellType)m_type; }				///< get the cell type
	CellFlags getFlags() const { return (CellFlags)m_flags; }				///< get the cell type
	Bool isAircraftGoal() const {return m_aircraftGoal != 0;}

	Bool isObstaclePresent( ObjectID objID ) const;					///< return true if the given object ID is registered as an obstacle in this cell

	Bool isObstacleTransparent() const;
	Bool isObstacleFence() const;

	/// Return estimated cost from given cell to reach goal cell
	UnsignedInt costToGoal( PathfindCell *goal );

	UnsignedInt costToHierGoal( PathfindCell *goal );

	UnsignedInt costSoFar( PathfindCell *parent );

	/// Insert into the open heap, lowest cost first.
	void putOnOpenList(PathfindOpenHeap &list);

	/// remove self from "open" list
	void removeFromOpenList( PathfindOpenHeap &list );

	/// put self on "closed" list, return new list
	void putOnClosedList( PathfindCellList &list );

	/// remove self from "closed" list
	void removeFromClosedList( PathfindCellList &list );

	/// remove all cells from closed list.
	static Int releaseClosedList( PathfindCellList &list );

	/// remove all cells from closed list.
	static Int releaseOpenList( PathfindOpenHeap &list );

	PathfindCell *getNextClosed() {return m_nextClosed;}
	PathfindCell *getPrevClosed() {return m_prevClosed;}

	UnsignedShort getXIndex() const {return m_pos.x;}
	UnsignedShort getYIndex() const {return m_pos.y;}

	Bool isBlockedByAlly() const;
	void setBlockedByAlly(Bool blocked);

	Bool getOpen() const {return m_open;}
	Bool getClosed() const {return m_closed;}
	UnsignedInt getCostSoFar() const {return m_costSoFar;}
	UnsignedInt getTotalCost() const {return m_totalCost;}

	void setCostSoFar(UnsignedInt cost) { m_costSoFar = cost;}
	void setTotalCost(UnsignedInt cost) { m_totalCost = cost;}

	void setParentCell(PathfindCell* parent);
	void clearParentCell();
	void setParentCellHierarchical(PathfindCell* parent);
	PathfindCell* getParentCell() const {return m_pathParent;}

	Bool startPathfind( PathfindCell *goalCell );
	Bool getPinched() const {return m_pinched;}
	void setPinched(Bool pinch) {m_pinched = pinch;	}

	void setPosition(Int x, Int y) { m_pos.x = x; m_pos.y = y; }
	// Tracks logical use of scratch data; storage is permanent for every cell.
	void initializeSearchData();
	Bool isInitialized() const { return m_initialized; }
	void clearSearchData();
	zoneStorageType getZone() const {return m_zone;}
	void setZone(zoneStorageType zone) {m_zone = zone;}
	void setGoalUnit(ObjectID unit, const ICoord2D &pos );
	void setGoalAircraft(ObjectID unit, const ICoord2D &pos );
	void setPosUnit(ObjectID unit, const ICoord2D &pos );
	ObjectID getGoalUnit() const {return m_goalUnitID;}
	ObjectID getGoalAircraft() const {return m_goalAircraftID;}
	ObjectID getPosUnit() const {return m_posUnitID;}

	ObjectID getObstacleID() const;

	void setLayer( PathfindLayerEnum layer ) { m_layer = layer; }	///< set the cell layer
	PathfindLayerEnum getLayer() const { return (PathfindLayerEnum)m_layer; }				///< get the cell layer

	void setConnectLayer( PathfindLayerEnum layer ) { m_connectsToLayer = layer; }	///< set the cell layer	connect id
	PathfindLayerEnum getConnectLayer() const { return (PathfindLayerEnum)m_connectsToLayer; }				///< get the cell layer connect id

	static Bool typesMatch(const PathfindCell& targetCell, const PathfindCell& sourceCell);
	static Bool waterGround(const PathfindCell& targetCell, const PathfindCell& sourceCell);
	static Bool groundRubble(const PathfindCell& targetCell, const PathfindCell& sourceCell);
	static Bool terrain(const PathfindCell& targetCell, const PathfindCell& sourceCell);
	static Bool crusherGround(const PathfindCell& targetCell, const PathfindCell& sourceCell);
	static Bool groundCliff(const PathfindCell& targetCell, const PathfindCell& sourceCell);

private:
	// Intrusive links are used only by the closed list.
	PathfindCell *m_nextClosed, *m_prevClosed, *m_pathParent;
	UnsignedInt m_heapIndex;
	UnsignedInt64 m_openSequence;
	UnsignedShort m_totalCost, m_costSoFar;
	ICoord2D m_pos;
	ObjectID m_goalUnitID, m_posUnitID, m_goalAircraftID;

	Bool hasHigherPriorityThan(const PathfindCell &other) const
	{
		return m_totalCost < other.m_totalCost ||
			(m_totalCost == other.m_totalCost && m_costSoFar > other.m_costSoFar);
	}
	ObjectID m_obstacleID;	                  ///< the object ID who overlaps this cell
	UnsignedInt m_initialized : 1;
	UnsignedInt m_open : 1;
	UnsignedInt m_closed : 1;
	UnsignedInt m_blockedByAlly : 1;          ///< True if this cell is blocked by an allied unit.
	UnsignedInt m_obstacleIsFence : 1;        ///< True if occupied by a fence.
	UnsignedInt m_obstacleIsTransparent : 1;  ///< True if obstacle is transparent (undefined if obstacleid is invalid)

	zoneStorageType m_zone : 14;              ///< Zone. Each zone is a set of adjacent terrain type.  If from & to in the same zone, you can successfully pathfind.  If not,
	                                          /// you still may be able to if you can cross multiple terrain types.
	UnsignedShort m_aircraftGoal : 1;         ///< This is an aircraft goal cell.
	UnsignedShort m_pinched : 1;              ///< This cell is surrounded by obstacle cells.
	UnsignedByte m_type : 4;                  ///< what type of cell terrain this is.
	UnsignedByte m_flags : 4;                 ///< what type of units are in or moving through this cell.
	UnsignedByte m_connectsToLayer : 4;       ///< This cell can pathfind onto this layer, if > LAYER_TOP.
	UnsignedByte m_layer : 4;                 ///< Layer of this cell.
};

typedef PathfindCell *PathfindCellP;
