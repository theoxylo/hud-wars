/* 
 * Simple game framework by Theoxylo, copyright 2008
 *
 */

#include "LevelMap.h"
#include <stdlib.h>
#include <stdio.h>


#define DEBUG_LEVEL false

LevelMap::LevelMap()
{
    printf("LevelMap() called; using 64x64 map tiles (8x8 px per tile)\n");
    
    _numBlocks = 0;
    setNumBlocks(15);
    
	// outer border
    _mapBlocks[0] = MapBlock(00, 00, 04, 64);
    _mapBlocks[1] = MapBlock(00, 00, 64, 04);
    _mapBlocks[3] = MapBlock(60, 00, 04, 64);
    // inner blocks
    _mapBlocks[4] = MapBlock(16, 12, 20, 04);
    _mapBlocks[5] = MapBlock(16, 14, 04, 34);
    _mapBlocks[6] = MapBlock(02, 16, 06, 04);
    _mapBlocks[7] = MapBlock(40, 24, 12, 04);
    _mapBlocks[8] = MapBlock(40, 26, 04, 14);
    _mapBlocks[9] = MapBlock(28, 30, 14, 04);
    _mapBlocks[10] = MapBlock(52, 36, 04, 16);
    _mapBlocks[11] = MapBlock(12, 44, 06, 04);
    _mapBlocks[12] = MapBlock(28, 48, 04, 14);
    _mapBlocks[13] = MapBlock(40, 48, 16, 04);
    _mapBlocks[14] = MapBlock(16, 56, 04, 06);
	// outer border bottom
    _mapBlocks[2] = MapBlock(00, 60, 64, 04);
    
	// navigation nodes
	for (int i = 0; i < NUM_NAV_NODES; i++)
	{
		_navNodes[i] = new NavNode(i);
	}
	
	_navNodes[0]->_position.set(-1, -1); // special "no available path" node
    
    _navNodes[1]->_position.set(14 <<3, 10 <<3);
	_navNodes[1]->add(_navNodes[2]);
	_navNodes[1]->add(_navNodes[14]);
    
    _navNodes[2]->_position.set(38 <<3, 10 <<3);
	_navNodes[2]->add(_navNodes[1]);
	_navNodes[2]->add(_navNodes[3]);
	_navNodes[2]->add(_navNodes[15]);
    
    _navNodes[3]->_position.set(54 <<3, 22 <<3);
	_navNodes[3]->add(_navNodes[2]);
	_navNodes[3]->add(_navNodes[4]);
	_navNodes[3]->add(_navNodes[5]);
	_navNodes[3]->add(_navNodes[15]);
    
	_navNodes[4]->_position.set(58 <<3, 34 <<3);
	_navNodes[4]->add(_navNodes[3]);
	_navNodes[4]->add(_navNodes[5]);
	_navNodes[4]->add(_navNodes[8]);
    
    _navNodes[5]->_position.set(50 <<3, 34 <<3);
	_navNodes[5]->add(_navNodes[4]);
	_navNodes[5]->add(_navNodes[6]);
    
    _navNodes[6]->_position.set(48 <<3, 44 <<3);
	_navNodes[6]->add(_navNodes[5]);
	_navNodes[6]->add(_navNodes[7]);
    
    _navNodes[7]->_position.set(38 <<3, 46 <<3);
	_navNodes[7]->add(_navNodes[6]);
	_navNodes[7]->add(_navNodes[9]);
	_navNodes[7]->add(_navNodes[10]);
	_navNodes[7]->add(_navNodes[17]);
    
    _navNodes[8]->_position.set(58 <<3, 54 <<3);
	_navNodes[8]->add(_navNodes[4]);
	_navNodes[8]->add(_navNodes[9]);
    
    _navNodes[9]->_position.set(38 <<3, 54 <<3);
	_navNodes[9]->add(_navNodes[7]);
	_navNodes[9]->add(_navNodes[8]);
    
    _navNodes[10]->_position.set(24 <<3, 38 <<3);
	_navNodes[10]->add(_navNodes[7]);
	_navNodes[10]->add(_navNodes[16]);
	_navNodes[10]->add(_navNodes[17]);
    
    _navNodes[11]->_position.set(22 <<3, 50 <<3);
	_navNodes[11]->add(_navNodes[12]);
	_navNodes[11]->add(_navNodes[17]);
    
    _navNodes[12]->_position.set(10 <<3, 50 <<3);
	_navNodes[12]->add(_navNodes[11]);
	_navNodes[12]->add(_navNodes[13]);
    
    _navNodes[13]->_position.set(10 <<3, 42 <<3);
	_navNodes[13]->add(_navNodes[12]);
	_navNodes[13]->add(_navNodes[14]);
    
    _navNodes[14]->_position.set(12 <<3, 24 <<3);
	_navNodes[14]->add(_navNodes[1]);
	_navNodes[14]->add(_navNodes[13]);
    
    _navNodes[15]->_position.set(38 <<3, 20 <<3);
	_navNodes[15]->add(_navNodes[2]);
	_navNodes[15]->add(_navNodes[3]);
	_navNodes[15]->add(_navNodes[16]);
    
    _navNodes[16]->_position.set(24 <<3, 26 <<3);
	_navNodes[16]->add(_navNodes[10]);
	_navNodes[16]->add(_navNodes[15]);
    
    _navNodes[17]->_position.set(24 <<3, 44 <<3);
	_navNodes[17]->add(_navNodes[7]);
	_navNodes[17]->add(_navNodes[10]);
	_navNodes[17]->add(_navNodes[11]);
    
    // end map data
    
    // determines which tile is displayed at each 64x64 map location
	int tileIndex = 0;
    
    // draw open background tiles
	for(int row = 0; row < 64; row++)
	{
		for(int col = 0; col < 64; col++)
		{
            // TODO draw tile texture
			//mapMemory[get64by64Index(col, row)] = tileIndex;
		}
	}
    
    // draw map
    for (int i = 0; i < _numBlocks; i++)
    {
        for(int row = 0; row < _mapBlocks[i].h && row + _mapBlocks[i].y < 64; row++)
        {
            for(int col = 0; col < _mapBlocks[i].w && col + _mapBlocks[i].x < 64; col++)
            {
                //tileIndex = mapMemory[get64by64Index(col + _mapBlocks[i].x, row + _mapBlocks[i].y)];
                if (tileIndex == 1) continue;
                
                // highlight edges
                if (col == 0 || col == _mapBlocks[i].w - 1 || row == 0 || row == _mapBlocks[i].h - 1) tileIndex = 2;
                
                else tileIndex = 1;
                
                //mapMemory[get64by64Index(col + _mapBlocks[i].x, row + _mapBlocks[i].y)] = tileIndex;
            }
        }
    }
    // done drawing map blocks
    
    // mark 4x4 grid for easy counting
	/*
     tileIndex = 1;
     for(int row = 0; row < 64; row+=4)
     {
     for(int col = 0; col < 64; col+=4)
     {
     mapMemory[get64by64Index(col, row)] = tileIndex;
     }
     }
     */
    
    // mark nodes for testing
	/*
     tileIndex = 2;
     for (int i = 1; i < NUM_NAV_NODES; i++)
     {
     int col = _navNodes[i]->_position.x >>3;
     int row = _navNodes[i]->_position.y >>3;
     mapMemory[get64by64Index(col, row)] = tileIndex;
     }
     */
    
}

void LevelMap::setNumBlocks(int numBlocks)
{ 
    printf("LevelMap::setNumBlocks(%d) called\n", numBlocks);
    _numBlocks = numBlocks < MAX_NUM_BLOCKS ? numBlocks : MAX_NUM_BLOCKS; 
}

const Vector LevelMap::getSpawnPosition()
{ 
    if (DEBUG_LEVEL) printf("LevelMap::getSpawnPosition() called\n");
    
    // the business with the -1 is to avoid the 0th special "non" node
    return _navNodes[(rand() % (NUM_NAV_NODES - 1)) + 1]->_position;
}

const Vector LevelMap::getNodePosition(int nodeIdx)
{ 
    // handle invalid index request
    if (nodeIdx >= NUM_NAV_NODES || nodeIdx < 0)
    {
        nodeIdx = 0;
    }

    return _navNodes[nodeIdx]->_position;
}

bool LevelMap::getPath(int fromNodeId, int toNodeId, Path* path)
{
    if (DEBUG_LEVEL) printf("LevelMap::getPath(fromNodeId=%d, toNodeId=%d, path) called\n", fromNodeId, toNodeId);
    
    path->reset();
    path->_map = this;

    // test looping path
    /*
    path->_loop = true;
    path->addWaypoint(12);
    path->addWaypoint(13);
    path->addWaypoint(14);
    path->addWaypoint(1);
    path->addWaypoint(2);
    path->addWaypoint(3);
    path->addWaypoint(4);
    path->addWaypoint(8);
    path->addWaypoint(9);
    path->addWaypoint(7);
    path->addWaypoint(17);
    path->addWaypoint(11);
    return true;
    */

	for (int i = 0; i < NUM_NAV_NODES; i++)
	{
		_navNodes[i]->reset();
	}

    if (fromNodeId < 1 || fromNodeId >= NUM_NAV_NODES) return false;
    if (toNodeId < 1 || toNodeId >= NUM_NAV_NODES) return false;
    if (fromNodeId == toNodeId) return false;

    // first (start) node in path
    NavNode* currNode = _navNodes[fromNodeId];

	while(true)
    {
		currNode->_visited = true;

		if (!path->addWaypoint(currNode->_id)) return false; // path must be full

		if (currNode->_id == toNodeId)
		{
			// found complete path
			return true;
		}

		NavNode* child = 0;
	    NavNode* bestChild = 0;
		while (child = currNode->nextChild())
		{
			if (child->_visited) continue;

			child->_distanceTraveled = currNode->_distanceTraveled 
				+ (child->_position - currNode->_position).length();

			if (!child->_estDistanceRemaining) child->_estDistanceRemaining 
				= (_navNodes[toNodeId]->_position - child->_position).length();

			int childCost = child->_distanceTraveled + child->_estDistanceRemaining;

			if (!bestChild 
				|| childCost < bestChild->_distanceTraveled + bestChild->_estDistanceRemaining)
			{
				bestChild = child; // found a new best child
			}
		}

		if (bestChild)
		{
			currNode = bestChild;
        }
		else
		{
			path->removeWaypoint();
			currNode = _navNodes[path->lastWaypoint()];
		}
    }

    return true;
}

int LevelMap::get64by64Index(int x, int y)
{
	if (x < 32)
	{
		if (y < 32) return y * 32 + x;
		else return  (y - 32) * 32 + x + 2048;
	}
	else
	{
		if (y < 32) return y * 32 + x - 32 + 1024;
		else return  (y - 32) * 32 + x -32 + 3072;
	}
}

int LevelMap::getClosestNode(const Vector& v)
{
	int closestDistSq = 0;
    int closestNodeNum = 0;
	for (int i = 1; i < NUM_NAV_NODES; i++)
	{
		int distSq = (v - _navNodes[i]->_position).lengthSq();
		if (i == 1 || distSq < closestDistSq) 
        {
            closestDistSq = distSq;
            closestNodeNum = i;
        }
	}
	return closestNodeNum;
}

NavNode::NavNode(int id)
{
    _id = id;
	_numChildNodes = 0;

    reset();
}

bool NavNode::add(NavNode* node)
{
	if (_currentChildNodeIndex >= MAX_CHILD_NODES) return false;

	_childNodes[_currentChildNodeIndex++] = node;
	_numChildNodes++;

	return true;
}

int NavNode::numChildren()
{
	return _numChildNodes;
}

void NavNode::reset()
{
	_currentChildNodeIndex = 0;
	_visited = false;

	_distanceTraveled = 0;
	_estDistanceRemaining = 0;
}

NavNode* NavNode::nextChild()
{
	_visited = true;

	if (_currentChildNodeIndex < _numChildNodes) 
		return _childNodes[_currentChildNodeIndex++];

	return 0;
}

Path::Path()
{
	reset();
}

void Path::reset()
{
	_numWaypoints = 0;
	_totalDistance = 0;
	_currIndex = 0;
    _loop = false;

	for (int i = 0; i < MAX_WAYPOINTS; i++)
	{
		_waypoints[i] = 0;
	}
}

bool Path::addWaypoint(int navNodeIdx)
{
	if (_numWaypoints >= MAX_WAYPOINTS) return false;

	_waypoints[_numWaypoints++] = navNodeIdx;
	return true;
}

bool Path::removeWaypoint()
{
    // can't remove very first waypoint
	if (_numWaypoints < 2) return false;

	_waypoints[--_numWaypoints] = 0;
	return true;
}

int Path::lastWaypoint()
{
	if (_numWaypoints < 1) return 0;
	return _waypoints[_numWaypoints - 1];
}

int Path::currentWaypoint(const Vector& position)
{
    /* working test impl
    Vector toPos = _map->getNodePosition(currWaypoint) - position;
    if (currWaypoint == 0 || toPos.lengthSq() < 320*320)
    {
        return  (rand() % (18 - 1)) + 1;
    }
    return currWaypoint;
    */

    if (_currIndex >= _numWaypoints)
    {
        if (_loop) _currIndex = 0;
        else return 0; // no more waypoints available for non-looping path
    }

    static const int NODE_TOUCH_DIST = 5; // 5 pixels
    Vector toPos = _map->getNodePosition(_waypoints[_currIndex]) - position;
    //toPos -= position;
    
    if (toPos.lengthSq() > NODE_TOUCH_DIST * NODE_TOUCH_DIST)
    {
        // havn't yet reached current waypoint
        return _waypoints[_currIndex];
    }

    // have reached current waypoint, so advance to next
    _currIndex++;

    // no more waypoints
    if (_currIndex >= _numWaypoints)
    {
        if (_loop) _currIndex = 0;
        else return 0;
    }
    return _waypoints[_currIndex];
}

