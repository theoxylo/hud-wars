/* 
 * Simple game framework by Theoxylo, copyright 2008
 *
 * (Sits atop SpriteLib)
 */

#include "GameWorld.h"

// #define DEBUG_GAMEWORLD (false && !(_time % 60))


#define DEBUG_COLLISIONS false

/*
 * GameWorld setup. Entities added with addEntity method (first added render on top)
 */
GameWorld::GameWorld()
{
    printf("GameWorld() called\n");
    
    _numSprites = 0;
    _currentSpriteIndex = 0;
    
    _map = new LevelMap();
    
    // scrolling 512x512 bg (e.g., 64x64 8x8px tiles)
    // scrolling 1024x1024 bg (e.g., 128x128 8x8px tiles)
    _size.x = 1024;
    _size.y = 1024;
    //_viewSize.x = 304;     // window
    //_viewSize.y = 304;     // window
    //_viewPosition.x = 104; // window
    //_viewPosition.y = 104; // window
    _viewSize.x = 320;   // full screen
    _viewSize.y = 480;   // full screen
    _viewPosition.x = 160; // full screen
    _viewPosition.y = 240; // full screen


    // Touch Controls
    printf("GameWorld: Setting up touch controls...\n");
    
    _touchControl1 = new TouchControl(this);
    addEntity(_touchControl1);
    _touchControl1->_sprite = getNewSprite();
    _touchControl1->_sprite->initSprite(TEXTURE_BASE_IDX_CROSSHAIR + 2, 32, 2);
    _touchControl1->_radius = 16;
    _touchControl1->_touchNum = 1;
    
    _touchControl2 = new TouchControl(this);
    addEntity(_touchControl2);
    _touchControl2->_sprite = getNewSprite();
    _touchControl2->_sprite->initSprite(TEXTURE_BASE_IDX_CROSSHAIR, 32, 2);
    _touchControl2->_radius = 16;
    _touchControl2->_touchNum = 2;
    
    // health bars
    printf("GameWorld: Setting up first healthbar as a template...\n");
    _healthBars[0] = addEntity(new HealthBar(this));
    _healthBars[0]->_sprite = getNewSprite();
    _healthBars[0]->_sprite->initSprite(TEXTURE_BASE_IDX_HEALTHBAR, 16, 10);
    _healthBars[0]->_radius = 8;
    // set up "show health level" animation
    _healthBars[0]->_animation = new Animation();
    _healthBars[0]->_animation->setFrameCount(3);
    _healthBars[0]->_animation->_repeat = false;
    _healthBars[0]->_animation->_displayFrames[0] = 0; // blank frame to allow hit animation to finish
    _healthBars[0]->_animation->_displayFrames[1] = 0; // this frame will be adjusted to match health 1 - 9
    _healthBars[0]->_animation->_displayFrames[2] = 0; // end on blank frame
    _healthBars[0]->_animation->_displayTimes[0] = 1;
    _healthBars[0]->_animation->_displayTimes[1] = 60;
    _healthBars[0]->_animation->_displayTimes[2] = 1;
    // end animation
    for (int i = 1; i < PLAYER_COUNT; i++)
    {
        printf("GameWorld: Setting up healthbar no. %d\n", i+1);
        _healthBars[i] = addEntity(new HealthBar(this));
        _healthBars[i]->initialize(_healthBars[0]);
        _healthBars[i]->_sprite = getNewSprite();
        _healthBars[i]->_sprite->initSprite(_healthBars[0]->_sprite);
    }

    // weapons
    printf("GameWorld: Setting up weapons...\n");
    Entity* weapon = addEntity(new Weapon(this));
    weapon->_sprite = getNewSprite();
    weapon->_sprite->initSprite(TEXTURE_BASE_IDX_WEAPON, 16, 1);
    weapon->_radius = 16;
    weapon->_visible = true;
    //weapon->_owner = _selectedEntity;
    
    // vehicles
    printf("GameWorld: Setting up vehicles...\n");
    _vehicles[0] = addEntity(new Vehicle(this));
    _vehicles[0]->_sprite = getNewSprite();
    _vehicles[0]->_sprite->initSprite(TEXTURE_BASE_IDX_VEHICLE, 32, 2);
	weapon->_owner = _vehicles[0];
	
    _vehicles[1] = addEntity(new Copter(this));
    _vehicles[1]->_sprite = getNewSprite();
    _vehicles[1]->_sprite->initSprite(TEXTURE_BASE_IDX_VEHICLE +2, 32, 2);
	
    for (int i = 0; i < VEHICLE_COUNT; i++)
    {
        _vehicles[i]->_radius = 16;   
        _vehicles[i]->_visible = false;
        // set up "take damage" animation
        _vehicles[i]->_animation = new Animation();
        _vehicles[i]->_animation->setFrameCount(2);
        _vehicles[i]->_animation->_repeat = false;
        _vehicles[i]->_animation->_displayFrames[0] = 1;
        _vehicles[i]->_animation->_displayFrames[1] = 0;
        _vehicles[i]->_animation->_displayTimes[0] = 10;
        _vehicles[i]->_animation->_displayTimes[1] = 1;
        // end animation
    }
    
    printf("GameWorld: Setting up player pawn\n");
    // player
    _players[0] = addEntity(new HeroPawn(this));
    _players[0]->_sprite = getNewSprite();
    _players[0]->_sprite->initSprite(TEXTURE_BASE_IDX_PLAYER, 16, 5);
    _players[0]->_radius = 8;  
    _players[0]->_healthBar = _healthBars[0];
    // set up "take damage" animation
    _players[0]->_animation = new Animation();
    _players[0]->_animation->setFrameCount(2);
    _players[0]->_animation->_repeat = false;
    _players[0]->_animation->_displayFrames[0] = 1;
    _players[0]->_animation->_displayFrames[1] = 0;
    _players[0]->_animation->_displayTimes[0] = 10;
    _players[0]->_animation->_displayTimes[1] = 1;
	// end animation
    
    // TODO support switching characters
	_selectedEntity = _players[0];

    printf("GameWorld: Setting up enemies...\n");

    // guards
    _players[1] = addEntity(new GuardPawn(this));
    _players[1]->_sprite = getNewSprite();
    _players[1]->_sprite->initSprite(TEXTURE_BASE_IDX_GUARD, 32, 3);
    _players[1]->_radius = 16; 
    _players[1]->_healthBar = _healthBars[1];
    // set up "take damage" animation
    _players[1]->_animation = new Animation();
    _players[1]->_animation->setFrameCount(2);
    _players[1]->_animation->_repeat = false;
    _players[1]->_animation->_displayFrames[0] = 1;
    _players[1]->_animation->_displayFrames[1] = 0;
    _players[1]->_animation->_displayTimes[0] = 20;
    _players[1]->_animation->_displayTimes[1] = 1;
	// end animation
    
    _players[2] = addEntity(new GuardPawn(this));
    _players[2]->initialize(_players[1]);
    _players[2]->_healthBar = _healthBars[2];


    // flyers
    _players[3] = addEntity(new FlyerPawn(this));
    _players[3]->_sprite = getNewSprite();
    _players[3]->_sprite->initSprite(TEXTURE_BASE_IDX_FLYER, 32, 8);
    _players[3]->_radius = 16; 
    _players[3]->_healthBar = _healthBars[3];

    _players[4] = addEntity(new FlyerPawn(this));
    _players[4]->initialize(_players[3]);
    _players[4]->_healthBar = _healthBars[4];
    
    // boxes
    _players[5] = addEntity(new BoxPawn(this));
    _players[5]->_sprite = getNewSprite();
    _players[5]->_sprite->initSprite(TEXTURE_BASE_IDX_BOX, 32, 1);
    _players[5]->_radius = 16;
    _players[5]->_healthBar = _healthBars[5];

    _players[6] = addEntity(new BoxPawn(this));
    _players[6]->initialize(_players[5]);
    _players[6]->_healthBar = _healthBars[6];
    
    // shield (body armor) pawn
    _players[7] = addEntity(new ShieldPawn(this));
    _players[7]->_sprite = getNewSprite();
    _players[7]->_sprite->initSprite(TEXTURE_BASE_IDX_SHIELD, 32, 3);
    _players[7]->_radius = 16;
    _players[7]->_healthBar = _healthBars[7];
    // set up "take damage" animation
    _players[7]->_animation = new Animation();
    _players[7]->_animation->setFrameCount(2);
    _players[7]->_animation->_repeat = false;
    _players[7]->_animation->_displayFrames[0] = 2;
    _players[7]->_animation->_displayFrames[1] = 1;
    _players[7]->_animation->_displayTimes[0] = 20;
    _players[7]->_animation->_displayTimes[1] = 1;
	// end animation
    _players[7]->_owner = _selectedEntity;
    
    // new flyers! lots
    for (int i = 0; i < 12; i++)
    {
        _players[i+8] = addEntity(new FlyerPawn(this));
        _players[i+8]->initialize(_players[3]);
        _players[i+8]->_healthBar = _healthBars[i+8];
        _players[i+8]->_seekPawn = _players[3];
        _players[i+8]->_avoidPawn = _players[i+7];
        _players[i+8]->_frame = i;
        _players[i+8]->spawn();
    }
	// total of 20 players... see PLAYER_COUNT
	
    printf("GameWorld: Setting up projectiles...\n");
    // projectile pool
    _missiles[0] = addEntity(new MissilePawn(this));
    _missiles[0]->_sprite = getNewSprite();
    _missiles[0]->_sprite->initSprite(TEXTURE_BASE_IDX_MISSILE, 8, 4);
    _missiles[0]->_radius = 4;  
    // set up missile animation
    /* none for now since using frames as missile types
    _missiles[0]->_animation = new Animation();
    _missiles[0]->_animation->setFrameCount(2);
    _missiles[0]->_animation->_repeat = true;
    _missiles[0]->_animation->_displayFrames[0] = 0;
    _missiles[0]->_animation->_displayFrames[1] = 1;
    _missiles[0]->_animation->_displayTimes[0] = 30;
    _missiles[0]->_animation->_displayTimes[1] = 20;
    _missiles[0]->_animation->start();
     */
    for (int i = 1; i < MISSILE_COUNT; i++)
    {
        _missiles[i] = addEntity(new MissilePawn(this));
        _missiles[i]->initialize(_missiles[0]);
    }
    _currentMissileIndex = 0;

    reset();
}

Entity* GameWorld::addEntity(Entity* e)
{
    if (_numEntities == MAX_NUM_ENTITIES) return e;
    
    _entities[_numEntities++] = e; // post increment for zero-based index
    
    return e;
}

Entity* GameWorld::nextMissile()
{
    return _missiles[_currentMissileIndex++ % MISSILE_COUNT];
}

// place pawns for game start
void GameWorld::reset()
{
    printf("GameWorld::reset() called\n");


    _score = 0;
    _time = 0;
    _elapsedTime = 0.0f;
    _currentMissileIndex = 0;

    _players[0]->_lastShotTimestamp = 0;
    _players[0]->spawn();
    
    // testing
    //_players[0]->_position.x = 256;
    //_players[0]->_position.y = 256;

    _selectedEntity = _players[0];
    adjustViewPosition(*_players[0]);

    printf("GameWorld::reset(): setting up players\n");

    // guards
    _players[1]->_seekPawn = _players[0];
    _players[1]->_lastShotTimestamp = 0;
    _players[1]->spawn();

    _players[2]->_seekPawn = _players[0];
    _players[2]->_lastShotTimestamp = 0;
    _players[2]->spawn();

    // flyers
    _players[3]->_seekPawn = _players[0];
    _players[3]->_avoidPawn = _players[4];
    _players[3]->spawn();

    _players[4]->_seekPawn = _players[3];
    _players[4]->spawn();
    
    // guard 1 will avoid player touch (but try to shoot)
    _players[1]->_avoidPawn = _players[0];

    // boxes
    _players[5]->spawn();
    _players[5]->_acceleration.set(160, 80);    
    
    _players[6]->spawn();
    _players[6]->_acceleration.set(-80, -160);    
    
    // health shield power-up
    _players[7]->spawn();
    
    for (int i = 1; i < 7; i++)
    {
        // try up to 15 more times to spawn enemies out of view
        for (int j = 0; j < 15; j++)
        {
            if (isInView(*_players[i])) continue;
            
            _players[i]->spawn();
        }
    }

    // testing overrides
    //_players[0]->_position.set(_map.getNodePosition(13));//testing
    //_players[1]->_position.set(_map.getNodePosition(11));//testing
    //_players[2]->_position.set(_map.getNodePosition(12));//testing

    printf("GameWorld::reset(): setting up other vehicles...\n");
    
    for (int i = 0; i < VEHICLE_COUNT; i++)
    {
        _vehicles[i]->spawn();
    }
    
    for (int i = 0; i < MISSILE_COUNT; i++)
    {
        _missiles[i]->_active = false;
    }

    // clear sound player queue sound ID list
    for (int i = 0; i < SOUND_QUEUE_LENGTH; i++)
    {
        _soundQueue[i] = 0;
    }
    _nextSoundQueueIdx = 0;
}

void GameWorld::setControlInput(ControlInput ci)
{
    //printf("setControlInput for touch %d at time %d", ci.touchNumber, _time);
    ci.timestamp = _time;

    // only storing 2 most recent touch events for touch 1 and 2 for now
    // 1. could store more touches if needed, but 2 seems to be enough
    // 2. could store more history if needed, but individual entities can keep history if needed
    switch (ci.touchNumber)
    {
        case 1:
            _ci1 = ci;
            break;
            
        case 2:
            _ci2 = ci;
            break;
    }
}

void GameWorld::update(float dTime)
{
    //printf("GameWorld::update(%f) called\n", dTime);
    
    // always update time (actually, the total frame count); to be used by world clients (e.g. pawn shoot delay)
    _time++;
    
    // new float seconds time
    _elapsedTime += dTime;
    

    // touch 1 tap does a soft pause, could set a way point, etc
    if (_ci1.isNew && _ci1.mode == iMode_touchReleased)
    {
        // only process this release event once
        _ci1.isNew = false;
        
        if (DEBUG_TOUCH) printf("Processing new release event for touch 1; hasMoved: %d; timestamp: %d\n", _ci1.hasMoved, _ci1.timestamp);
        
        //support slow tap for other function? if (_ci1.x == _ci1.startX)
        
        
        if (!_ci1.hasMoved && _time > 10)
        {
            _paused = !_paused;
            //_zoom_out = !_zoom_out;
            
            if (!_paused && _gameOver)
            {
                // restart the game if coming out of game-over pause
                _gameOver = false;
                reset();
            }
        }
    }
    
    
    if (_ci2.isNew && _ci2.mode == iMode_touchReleased)  // TODO touch 2 events are being triggerd constantly by touch 1!
    {
        // only process this release event once
        _ci2.isNew = false;
        if (DEBUG_TOUCH) printf("Processing new release event for touch 2; hasMoved: %d\n", _ci2.hasMoved);
        
        // support slow tap? if (_ci2.x == _ci2.startX)...
        // custom tap speed? if (touchTime < tapSpeed && _ci2.x == _ci2.startX)...
        // double tap? if (touchTime < doubleTapSpeed)...
        
        //if (!_ci2.hasMoved) _cursor->_active = !(_cursor->_active);
        
        if (!_ci2.hasMoved) _zoom_out = !_zoom_out;
        //_zoom_out = true;
    
        //if (_cursor->_active) printf("Cursor is active at time %f!\n", _elapsedTime);
        //else  printf("Cursor is NOT active at time %f!\n", _elapsedTime);
        
    }
     
    /* zoom in while shooting, like iron sights...
    if (_ci2.mode == iMode_touchMoved) _zoom_out = false;
    else _zoom_out = true;
     */
   
    //printf("GameWorld::update(%f): process cursor\n", dTime);
    
    
    if (DEBUG_GAMEWORLD) printf("GameWorld::update(dTime: %f), time (frames): %d, total time: %f\n", dTime, _time, _elapsedTime);

	/*
    if (_cursor->_active)
    {
        _cursor->update(dTime);
    }
	*/
    
    if (_paused)
    { 
        // drag view to observe frozen world in hud frame
        if (_ci1.isNew && _ci1.mode == iMode_touchMoved)
        {
            _ci1.isNew = false;

            Vector scrollAmount;
            scrollAmount.set(_ci1.x_trail_1 - _ci1.x, _ci1.y_trail_1 - _ci1.y);
            _viewPosition.add(scrollAmount * 2);// * -2);
        }
            
        return; // cursor will still be updated above
    }
    
    /* game over?
    if (_selectedEntity->_health == 0)
    {
        _paused = true;
        _gameOver = true;
        return;
    }
     */
    
    for (int i = 0; i < _numEntities; i++)
    {
        _entities[i]->update(dTime);
    }
    
    adjustViewPosition(*_selectedEntity);
}

void GameWorld::render()
{
    for (int i = 0; i < _numEntities; i++)
    {
        _entities[i]->render();
    }
    
    resetNextSprite();
}

void GameWorld::adjustViewPosition(Entity& pawn)
{
    if (DEBUG_GAMEWORLD) printf("GameWorld::adjustViewPosition() called with pawn id %d\n", pawn.id());
    
    _viewPosition = pawn._position;// - (_viewSize / 2);
    
    // ensure valid view position

    /*
    bool stopScrollAtMapEdge = false;
    if (stopScrollAtMapEdge)
    {
        if (_viewPosition.x < 0) _viewPosition.x = 0;
        if (_viewPosition.y < 0) _viewPosition.y = 0;
        if (_viewPosition.x > _size.x - _viewSize.x) _viewPosition.x = _size.x - _viewSize.x;
        if (_viewPosition.y > _size.y - _viewSize.y) _viewPosition.y = _size.y - _viewSize.y;
    }
    */
    
    if (DEBUG_GAMEWORLD) printf("GameWorld::adjustViewPosition() complete with x, y: %f, %f\n", _viewPosition.x, _viewPosition.y);
}

bool GameWorld::isInView(Entity& p)
{
    if (DEBUG_GAMEWORLD) printf("GameWorld::isInView() called with entity id %d\n", p.id());
    
    if (p._position.x + p._radius <= _viewPosition.x - _viewSize.x/2) return false;
    if (p._position.x - p._radius >= _viewPosition.x + _viewSize.x/2) return false;
    if (p._position.y + p._radius <= _viewPosition.y - _viewSize.y/2) return false;
    if (p._position.y - p._radius > _viewPosition.y + _viewSize.y/2) return false;
    
    return true;
}

int GameWorld::selectedEntityId()
{
    return _selectedEntity ? _selectedEntity->id() : NONE_SELECTED;
}

Sprite* GameWorld::getNewSprite()
{
    if (_numSprites == MAX_NUM_SPRITES) return 0;
    
    _worldSprites[_numSprites++] = new Sprite();
    return _worldSprites[_numSprites -1];
}

void GameWorld::resetNextSprite()
{
    //_currentSpriteIndex = 0;
    _currentSpriteIndex = _numSprites -1; // start with last sprite
}

Sprite* GameWorld::getNextSprite()
{
    if (_numSprites == 0) return 0;
    //if (_currentSpriteIndex >= _numSprites) return 0;
    if (_currentSpriteIndex < 0) return 0;

    //return _worldSprites[_currentSpriteIndex++];
    return _worldSprites[_currentSpriteIndex--];
}

void GameWorld::queueSound(int id)
{
    if (_nextSoundQueueIdx >= SOUND_QUEUE_LENGTH)
    {
        return;
    }
    
    _soundQueue[_nextSoundQueueIdx] = id;
    
    // increment available ID for next call
    ++_nextSoundQueueIdx;
}
