/* 
 * Simple game object framework by Theoxylo, copyright 2008
 */

#ifndef LEVEL_MAP_H
#define LEVEL_MAP_H

#include "Vector.h"
#include "MapBlock.h"

class NavNode
{
public:

	NavNode(int id);

	NavNode* nextChild();
	int numChildren();

	void reset();

	bool add(NavNode* linkedNode);

	Vector _position;
	bool _visited;
    int _id;

	int _distanceTraveled;
	int _estDistanceRemaining;

protected:

	int _numChildNodes;
	int _currentChildNodeIndex;

	// storage
	static const int MAX_CHILD_NODES  = 5;
	NavNode* _childNodes[MAX_CHILD_NODES];
};

class LevelMap;

class Path
{
public:

	Path();

    // for path builder use
	void reset();
	bool addWaypoint(int nodeIndex);
	bool removeWaypoint();
    int lastWaypoint();

    // for Pawn use
	int currentWaypoint(const Vector& position);
    bool _loop;

    LevelMap* _map;

protected:
	
	static const int MAX_WAYPOINTS = 20;
	int _waypoints[MAX_WAYPOINTS];

	int _numWaypoints;
	int _totalDistance;
	int _currIndex;
};

class LevelMap
{
public:

    LevelMap();
	//void LevelMap::initialize();
	
    static const int MAX_NUM_BLOCKS = 16;
    static const int NUM_NAV_NODES = 18;
	
    MapBlock _mapBlocks[MAX_NUM_BLOCKS];
    void setNumBlocks(int numBlocks);
    int _numBlocks;
    
    const Vector getSpawnPosition();
    const Vector getNodePosition(int nodeIdx);

	NavNode* _navNodes[NUM_NAV_NODES];

	int getClosestNode(const Vector& v);
	bool getPath(int fromNode, int toNode, Path* path);

	enum MapName {MAP_1, MAP_2};

private:
    int get64by64Index(int x, int y);
};

#endif
