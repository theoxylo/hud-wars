/* 
 * Simple game object framework by Theoxylo, copyright 2008
 */

#ifndef GAME_WORLD_H
#define GAME_WORLD_H

#define DEBUG_GAMEWORLD (false && !(_time % 60))

#include <stdlib.h>
#include <stdio.h>
#include "EntityLib.h"
#include "SpriteLib.h"
#include "ControlInput.h"

enum 
{
	SOUND_SHOOT_ID  = 3,	
    SOUND_BOUNCE_ID   = 4,
    SOUND_HIT_ID    = 5,
	NUM_SOUNDS
};

enum 
{
	//TEXTURE_BASE_IDX_CURSOR = 0,
    TEXTURE_BASE_IDX_HEALTHBAR = 10,
	TEXTURE_BASE_IDX_PLAYER = 20,
    TEXTURE_BASE_IDX_VEHICLE = 30,
    TEXTURE_BASE_IDX_GUARD = 40,
    TEXTURE_BASE_IDX_BOX = 50,
    TEXTURE_BASE_IDX_FLYER = 60,
	TEXTURE_BASE_IDX_CROSSHAIR = 70,
    TEXTURE_BASE_IDX_MISSILE = 80,
    TEXTURE_BASE_IDX_SHIELD = 90,
    TEXTURE_BASE_IDX_WEAPON = 100
};

class GameWorld
{
public:

    static const int MISSILE_COUNT = 20;
    static const int PLAYER_COUNT = 20;
    static const int VEHICLE_COUNT = 2;
    static const int SOUND_QUEUE_LENGTH = 3;

    /*
     * All quantities in pixels
     *
     * Default constructor is set up for full screen with no scrolling
     */
    GameWorld();

    void reset();

    void update(float dTime);

	void render();

    void adjustViewPosition(Entity& p);

	bool isInView(Entity& p);


    Vector _size; // eg 512x512 or 1024x1024

    Vector _viewSize; // eg 302x304

    // position of screen viewport center relative to board
    Vector _viewPosition;
    
    // Entities

    Entity* addEntity(Entity* e);
    int _numEntities;
    static const int MAX_NUM_ENTITIES = 128;
    Entity* _entities[MAX_NUM_ENTITIES];
    
    TouchControl* _touchControl1;
    TouchControl* _touchControl2;
    
    Entity*    _players[PLAYER_COUNT];
    
    Entity*   _healthBars[PLAYER_COUNT];
    //Entity*  _cursor;
	Entity* _missiles[MISSILE_COUNT];
	Entity*     _vehicles[VEHICLE_COUNT];
    
    // backing sprites for entities
    int _numSprites;
    int _currentSpriteIndex;
    static const int MAX_NUM_SPRITES = 128;
    Sprite* _worldSprites[MAX_NUM_SPRITES];
    Sprite* getNewSprite();
    // for iterating over all sprites
    void resetNextSprite();
    Sprite* getNextSprite();

	int _score;
    
    int _time; // frame count since game reset
    
    float _elapsedTime; // seconds since game reset; should be equal to _time/FPS;

    LevelMap* _map;

	int _currentMissileIndex;
    Entity* nextMissile();

    Entity* _selectedEntity;
    enum {NONE_SELECTED = 0};
    int selectedEntityId();
    
    // control inputs
    void setControlInput(ControlInput ci);
    ControlInput _ci1;
    ControlInput _ci2;
    
    bool _paused; // soft pause; game will continue to render may choose to update selectively
    bool _zoom_out; // used to set gl projection
    
    bool _gameOver;
    
    void queueSound(int id);
    int _nextSoundQueueIdx;
    int _soundQueue[SOUND_QUEUE_LENGTH];

};

#endif
