#include "GameWorld.h"

#include <assert.h>


#define DEBUG_ENTITY (false && !(_gameWorld->_time % 60))

static bool DEBUG_WALLS = false;
static bool DEBUG_SHOOT = false;


void Entity::initialize(Entity* p)
{
	_radius = p->_radius;
    _gameWorld = p->_gameWorld;
    _owner = p->_owner;
    
    _sprite = _gameWorld->getNewSprite();
	_sprite->initSprite(p->_sprite);
    
    // make a copy of the animation instance so they can vary independently
    // uses default copy constructor?
    if (p->_animation) _animation = new Animation(*(p->_animation));
}

Entity::Entity(GameWorld* gw) :
_position(),
_velocity(),
_acceleration(),
_heading(),
_rotation(),
_moveHeading(),
_aimPosition(),
_gameWorld(gw)
{
    //id(); // creates id and registers this instance in static array the first time it is called
    //_instanceId = 0;

    // array of all entities in the game; no dynamic allocation at this time
    static Entity* __instances[MAX_NUM_INSTANCES];
    
    // ids will start at 1
    static int __nextInstanceId = 0;
    
    // update static members
    if (__nextInstanceId < MAX_NUM_INSTANCES)
    {
        __instances[__nextInstanceId] = this;
        _instanceId = ++__nextInstanceId;
    }
    
    
    printf("***********Entity() called; new instance created with id %d\n", _instanceId);
    
    //_gameWorld = 0;
        
    _active = true;
    _visible = true;
    _radius = 16; 
    _frame = 0;
    _spawnDelay = 0.0f;
    
    _animation = 0;
    _sprite = 0;
    _owner = 0;
    _vehicle = 0;    
    
    _edgeMode = Entity::EDGE_LIMIT;
    _turnSpeed = 0; // degrees per second in 360 degree circle
    
    // start with East heading;        
    // _rotation and _heading are synced by setHeading(int) and setHeading(Vector) methods
    setHeading(0);
    
    _maxSpeed = 128;
    _maxAccel = 160; // pixesl per second
    
    _mass = 1.0;
    
    _seekArriveDistance = 0;
    
    // for shooting
    _lastShotTimestamp = 0;
    _shotIntervalTime = 0;
    
    // hp
    _maxHealth = 10;
    _health = _maxHealth;
    _healthBar = 0;
    _shield = 0;
    
    _seekPawn = 0;
    _avoidPawn = 0;
    _path = 0;

    _timeOnScreen = 0;
    _currWaypoint = 0;   
}

Entity::~Entity()
{
    printf("Entity::~Entity() called for entity %d\n", id());
}


void Entity::update(float dTime)
{
    if (DEBUG_ENTITY) printf("Entity::update(dTime: %f), id: %d, gameWorld time: %d, elapsed time: %f\n", dTime, id(), _gameWorld->_time, _gameWorld->_elapsedTime);
    
    if (!_active) return;
    
    return;
}

bool Entity::checkBlockCollisions()
{
    if (DEBUG_WALLS) printf("Entity::checkBlockCollisions() called for Entity id %d\n", id());
    
    bool touching = false;
    
    // iterate through list of map blocks
    for (int b = 0; b < _gameWorld->_map->_numBlocks; b++)
    {
        if (DEBUG_WALLS) printf("Testing block %d\n", b);
        // use <<3 to convert map blocks to world pixel, 8 px per block
        BlockTouch bt = handleBlockCollision(_gameWorld->_map->_mapBlocks[b].x <<3, _gameWorld->_map->_mapBlocks[b].y <<3, _gameWorld->_map->_mapBlocks[b].w <<3, _gameWorld->_map->_mapBlocks[b].h <<3);
        if (DEBUG_WALLS) printf("Testing block %d, result: %d", b, bt);
        
        if (bt == Entity::NOT_NEAR_Y) break; // short circuit since blocks are sorted by y
        
        if (bt == Entity::NOT_NEAR) continue;
        
        touching = true;
        
    }
	return touching;
}

int Entity::id()
{
    return _instanceId;
}

/*
 * update backing sprite data
 */
void Entity::render()
{
    // short circuit if we have no sprite to render to
    if (!_sprite) return;
    
    // hide sprite if not active or not in viewport
    //_sprite->visible = _active && _visible && _gameWorld->isInView(*this);
	
	// only hide sprite is not active and visible, don't worry about view optimization
    _sprite->visible = _active && _visible;
    
    // short circuit if not visible anyway
    if (!_sprite->visible) return;
    
    _sprite->posX = _position.x;
    _sprite->posY = _position.y;    

    _sprite->rotation = _rotation % 360;
    
    _sprite->frame = _frame % _sprite->frames;
}

void Entity::spawn()
{
    //printf("Entity::spawn() called for entity id %d\n\n", id());
    
    _active = true;
    _visible = true;
    
    _owner = 0; // clear owner
    
    _position.set(_gameWorld->_map->getSpawnPosition());
    
    if (DEBUG_ENTITY) printf("Entity::spawn() called for entity id %d got position %f, %f\n", id(), _position.x, _position.y);
        
    _velocity.zero();
    _acceleration.zero();
	_health = _maxHealth;
	
    // reset animation
    if (_animation) _animation->reset();
    _frame = 0;    
    
    if (_path) _path->reset();

    if (_healthBar) _healthBar->_frame = 0;
    
    // easy way to slowly make it get harder
    // as enemies are killed off and respawn
    //_players[i]->_maxSpeed++;
    //_players[i]->_maxAccel++;
}

/*
void Entity::update(float dTime)
{
    if (DEBUG_ENTITY) printf("Entity::update(dTime: %f), id: %d, gameWorld time: %d, elapsed time: %f\n", dTime, id(), _gameWorld->_time, _gameWorld->_elapsedTime);
    
    if (!_active) return;
    
    updateAnimation(dTime);
    
    updateHealth(dTime);
    
    if (_seekPawn && _seekPawn->_active) 
    {
        // we have an active target entity to seek, so reset acceleration accordingly
        _acceleration.set(seek(_seekPawn->_position));
    }
    
    if (_avoidPawn && _avoidPawn->_active) 
    {
        // we have an active target pawn to seek, so reset acceleration accordingly
        _acceleration.add(avoid(_avoidPawn->_position, 64));
    }
    
    checkBlockCollisions();
    
    if (_acceleration.lengthSq() == 0) return;
    
    _acceleration.limit(_maxAccel);
    
    _velocity.add(dTime * _acceleration);
    
    _velocity.limit(_maxSpeed);
    
	_position.add(dTime * _velocity);
}
 */

void Entity::updateAnimation(float dTime)
{
    if (_animation && _animation->update()) _frame = _animation->getFrame();
}

void Entity::updateHealth(float dTime)
{
    if (_healthBar)
    {
        _healthBar->_position.set(_position);
        _healthBar->_position.y -= (_radius + 3); // display health bar 3 pixels above pawn
        _healthBar->_animation->_displayFrames[1] = (_health * 10) / _maxHealth;
        _healthBar->update(dTime);
    }
}

void Entity::checkMissileCollisions()
{
    if (DEBUG_ENTITY) printf("Entity::checkMissileCollisions() called for entity id %d\n", id());    

    for (int j = 0; j < _gameWorld->MISSILE_COUNT; j++)
    {
        if (!_gameWorld->_missiles[j]->_active) continue;
        
        if (DEBUG_ENTITY) printf("Testing player id %d and missile id %d\n", id(), _gameWorld->_missiles[j]->id());
        
        // ignore pawns own missiles
        if (this == _gameWorld->_missiles[j]->_owner) continue;
        
        // ignore pawns own parents' missiles, specail case for shield pawn
        if (_owner == _gameWorld->_missiles[j]->_owner) continue;
        
        // all done for an obvious miss
        if (!isTouching(_gameWorld->_missiles[j])) continue;
        
        // a hit!
        if (DEBUG_ENTITY) printf("A HIT on player id %d with missile id %d at location %f, %f\n", id(), _gameWorld->_missiles[j]->id(), _position.x, _position.y);
        
        // knock back player when struck by missile
        Vector toPlayer = _position - _gameWorld->_missiles[j]->_position;
        _velocity.add(toPlayer);
        
        _gameWorld->queueSound(SOUND_HIT_ID);
        //queueSound(SOUND_BOUNCE_ID);        
        
        
        // leave missile active if you wanted "super missiles"; future power-up?
        _gameWorld->_missiles[j]->_active = false;
        
        _health--;
        
        // start hit animation
        if (_animation) _animation->start();
        
        // start healthbar animation
        if (_healthBar) _healthBar->_animation->start();
        
        // player was hit
        if (this == _gameWorld->_selectedEntity) 
        {
            _gameWorld->_score--;
        }
        else
        {
            _gameWorld->_score++;
        }
        
        if (_health > 0) continue; // still alive
        
        _active = false;
        
        spawn();
        
        // TODO: move following to Entity base
        if (this != _gameWorld->_selectedEntity) 
        {
            // try to spawn out of view
            for (int n = 0; n < 5 && _gameWorld->isInView(*this); n++)
            {
                spawn();
            }
        }

    } // done checking all missiles
    
    return;
}

void Entity::checkPlayerCollisions() // bounce
{
    // check collisions between entities
    for (int i = 0; i < GameWorld::PLAYER_COUNT; i++)
    {
        // skip self
        if (this == _gameWorld->_players[i]) continue;
        
        // skip inactive entities
        if (!_gameWorld->_players[i]->_active) continue;
        
        //  if touching, simple velocity bounce to this player
        // since now iterating over all players for each player,
        // only modify self and let other entity do same for itself
        
        if (isTouching(_gameWorld->_players[i]->_vehicle))
        {
            //handleCollision(_gameWorld->_players[i]->_vehicle);
        }
        else if (isTouching(_gameWorld->_players[i]))
        {
            Vector toPlayer = _position - _gameWorld->_players[i]->_position;
            _velocity.add(toPlayer);
            //handleCollision(_gameWorld->_players[i]);
        }
    }            
    
    return; 
}

// may need to be virtual
void Entity::handleCollision(const Entity* const other)
{
    if (!other || !_active || !other->_active) return;
    
    if (other->_velocity.lengthSq() > 1)
    {
        float massRatio = other->_mass / _mass;
        //printf("Mass ratio: %f\n", massRatio);
        
        if (massRatio > 1.0)
        {
            // other guy is heavier
            _velocity.add(other->_velocity * (massRatio / 10));
        }
        else
        {
            // we are heavier
            //_velocity
        }
    }
    
    return;
}

bool Entity::isFacing(Vector position)
{
    Vector toPosition = position - _position;
    if (toPosition.lengthSq() <= 1) return true; // at position

    return (_heading | toPosition) > 0;
}

void Entity::rotateTowardDirection(Vector direction, int delta360)
{
    //printf("turning toward vector x: %f, y: %f, delta360: %d\n", direction.x, direction.y, delta360);
    
    if (direction.lengthSq() < .1) return; // at position

    Vector v = _heading.perp(); // if dot product of perp is positive, turn right
    if ((_heading.perp() | direction) > 0)
    {
        rotate(delta360);
    }
    else
    {
        rotate(-delta360);
    }
}

void Entity::rotate(int delta360)
{
    setHeading(_rotation + delta360);
}

/*
 * compare square of distance between to square of sum of radii and required extra clearance
 */
bool Entity::isTouching(const Entity* const other)
{
    if (!other || !_active || !other->_active) return false;
    
    Vector toOther(other->_position - _position);
	float justFit(_radius + other->_radius - 2);
    
	return (toOther.lengthSq() < justFit * justFit);
}

Entity::BlockTouch Entity::handleBlockCollision(float x, float y, float w, float h)
{
    if (!_active || _edgeMode == EDGE_IGNORE) return NOT_NEAR;

    BlockTouch result = NOT_NEAR;

    // clamp angle
    _rotation %= 360;
    
	int grace = 1;
    int overlap = 0;

    int xOverlap = (_position.x + _radius) - x;
    if (xOverlap <= -grace) return NOT_NEAR;
    else // near or touching
    {
        overlap = xOverlap;
        result = NEAR_X;
    }


    int wOverlap = (x + w) - (_position.x - _radius);
    if (wOverlap <= -grace) return NOT_NEAR;
    else if (wOverlap < overlap)
    {
        overlap = wOverlap;
        result = NEAR_W;
    }

    int yOverlap = (_position.y + _radius) - y;
	// to short circuit iteration of blocks sorted by Y
	// NOT WORKING
    //if (yOverlap <= -grace) return NOT_NEAR_Y; 
    if (yOverlap <= -grace) return NOT_NEAR;
    else if (yOverlap < overlap)
    {
        overlap = yOverlap;
        result = NEAR_Y;
    }

    int hOverlap = (y + h) - (_position.y - _radius);
    if (hOverlap <= -grace) return NOT_NEAR;
    else if (hOverlap < overlap)
    {
        overlap = hOverlap;
        result = NEAR_H;
    }

    // near but not touching or not touching enough for hard correction
    //if (overlap <= grace) return result;
    
    // actual overlap beyond grace for adjustment purposes
    overlap = overlap > grace ? overlap - grace : 0;

    if (overlap && _edgeMode == EDGE_DISAPPEAR)
    {
        // TODO: need die() method? dangerouse to set _active to false without knowing who will call spawn()
        _active = false;
        return result;
    }

    // adjust position by least necessary amount to un-touch
    // and adjust other attributes for current EdgeMode
    switch (result)
    {
    case NEAR_X:

        if (_edgeMode == EDGE_LIMIT)
        {
            _position.x -= overlap;
        }
        else if (_edgeMode == EDGE_AVOID)
        {
            _position.x -= overlap;

            Vector desiredVelocity;
			if (_acceleration.y > 0) desiredVelocity.set(0, _maxSpeed); 
			else desiredVelocity.set(0, -_maxSpeed);
            Vector desiredAcceleration = desiredVelocity - _velocity;
            _acceleration.add(desiredAcceleration.limit(_maxAccel));
        }
        else if (_edgeMode == EDGE_REFLECT)
        {
            // hard limit position
            _position.x -= overlap;

            // reflect acceleration and velocity
            if (_acceleration.x > 0) _acceleration.x *= -1;
            if (_velocity.x > 0) _velocity.x *= -1;

            // adjust heading
            if (_rotation >= 0 && _rotation < 90) setHeading(180 - _rotation);
            else if (_rotation > 270 && _rotation <= 360) setHeading(540 - _rotation);
        }
        break;
    
    case NEAR_W:

        if (_edgeMode == EDGE_LIMIT)
        {
            _position.x += overlap;
        }
        else if (_edgeMode == EDGE_AVOID)
        {
            _position.x += overlap;

            Vector desiredVelocity;
			if (_acceleration.y > 0) desiredVelocity.set(0, _maxSpeed); 
			else desiredVelocity.set(0, -_maxSpeed);
            Vector desiredAcceleration = desiredVelocity - _velocity;
            _acceleration.add(desiredAcceleration.limit(_maxAccel));
        }
        else if (_edgeMode == EDGE_REFLECT)
        {
            _position.x += overlap;

            if (_acceleration.x < 0) _acceleration.x *= -1;
            if (_velocity.x < 0) _velocity.x *= -1;

            if (_rotation > 90 && _rotation <= 180) setHeading(180 - _rotation);
            else if (_rotation > 180 && _rotation < 270) setHeading(540 - _rotation);
        }
        break;

    case NEAR_Y:

        if (_edgeMode == EDGE_LIMIT)
        {
            _position.y -= overlap;
        }
        else if (_edgeMode == EDGE_AVOID)
        {
            _position.y -= overlap;

            Vector desiredVelocity;
			if (_acceleration.x > 0) desiredVelocity.set(_maxSpeed, 0); 
			else desiredVelocity.set(-_maxSpeed, 0);
            Vector desiredAcceleration = desiredVelocity - _velocity;
            _acceleration.add(desiredAcceleration.limit(_maxAccel));
        }
        else if (_edgeMode == EDGE_REFLECT)
        {
            _position.y -= overlap;

            if (_acceleration.y > 0) _acceleration.y *= -1;
            if (_velocity.y > 0) _velocity.y *= -1;

            if (_rotation > 0 && _rotation < 180) setHeading(360 - _rotation);
        }
        break;
    
    case NEAR_H:

        if (_edgeMode == EDGE_LIMIT)
        {
            _position.y += overlap;
        }
        else if (_edgeMode == EDGE_AVOID)
        {
            _position.y += overlap;

            Vector desiredVelocity;
			if (_acceleration.x > 0) desiredVelocity.set(_maxSpeed, 0); 
			else desiredVelocity.set(-_maxSpeed, 0);
            Vector desiredAcceleration = desiredVelocity - _velocity;
            _acceleration.add(desiredAcceleration.limit(_maxAccel));
        }
        else if (_edgeMode == EDGE_REFLECT)
        {
            _position.y += overlap;

            if (_acceleration.y < 0) _acceleration.y *= -1;
            if (_velocity.y < 0) _velocity.y *= -1;

            if (_rotation > 180 && _rotation < 360) setHeading(360 - _rotation);
        }
        break;

	case NOT_NEAR:
	case NOT_NEAR_Y:
		// do nothing
		break;
    }

    return result;
}

Vector Entity::seek(const Vector& position)
{
    //printf("Entity::seek() called with x = %f, y = %f\n", position.x, position.y);
    
    Vector toPos = position - _position;

    // scale max accel according to proportion of position distance to falloffDistance
    // e.g., for hud-based controls with fixed "throw"
    // or to decrease accel as target is approaced ala "arrive"
    float scaledAccel = _maxAccel; // default to max
    if (_seekArriveDistance > 0 && toPos.lengthSq() < (_seekArriveDistance * _seekArriveDistance))
    {
        // reduce accel if within falloff distance
        scaledAccel = (_maxAccel * toPos.length()) / _seekArriveDistance;
    }

    Vector desiredAcceleration = (toPos.limit(_maxSpeed) - _velocity);
    desiredAcceleration.limit(scaledAccel);

    return desiredAcceleration;
}

Vector Entity::avoid(Entity& other, int range)
{
	if (other._active) return avoid(other._position, range);

    Vector v;
    return v;
}

Vector Entity::avoid(Vector position, int range)
{
    Vector desiredAcceleration;

	// short circuit if not within range
    Vector toThis = _position - position;
    if (range <= 0) range = 64;
    if (toThis.lengthSq() > (range * range)) return desiredAcceleration;

	//_acceleration = (toThis * _maxAccel).perp().limit(_maxAccel);
	//_acceleration = (toThis * _maxAccel).limit(_maxAccel);
	//_acceleration.zero();

	//Vector desiredVelocity = (position - _position).perp();
	//Vector desiredVelocity = ((_position - position).perp() * _maxSpeed).limit(_maxSpeed);
	Vector desiredVelocity = (_position - position).limit(_maxSpeed);

    desiredAcceleration = desiredVelocity - _velocity;
	//_acceleration = (desiredAcceleration * _maxAccel).limit(_maxAccel);
    desiredAcceleration.limit(_maxAccel);

    return desiredAcceleration;
}

bool Entity::shoot()
{
    if (!_seekPawn) return false;

    return shoot(_seekPawn->_position);
}  

bool Entity::shoot(Vector targetPosition)
{
    return shoot(targetPosition, false); // not special
}

bool Entity::shoot(Vector targetPosition, bool special)
{
    if (DEBUG_SHOOT) printf("Shoot(%f, %f) at time %d\n", targetPosition.x, targetPosition.y, _gameWorld->_time);
    
    if (_gameWorld->_time < _lastShotTimestamp + _shotIntervalTime) 
    {
        return false;
    }
    
    Entity* missile = _gameWorld->nextMissile();
    if (_owner) missile->_owner = _owner;
    else missile->_owner = this;
    missile->_active = true;
    missile->_position.set(_position);
    missile->_moveHeading.set(targetPosition - _position);
    missile->setHeading(missile->_moveHeading);
    
    // for drift: 
    //missile->_velocity.set(_velocity);
    //missile->_velocity.add(missile->_heading * missile->_maxSpeed);
    //missile->_velocity.limit(missile->_maxSpeed);
    
    if (special) // special missile
    {
        missile->_velocity.set(missile->_heading * missile->_maxSpeed * 1.4);
        missile->_frame = 2;
    }
    else
    {
        missile->_velocity.set(missile->_heading * missile->_maxSpeed);
        missile->_frame = 0;
    }
    
    // add the velocity of the shooter for drift
	missile->_velocity.add(_velocity);
    
    _lastShotTimestamp = _gameWorld->_time;
 
    // just need better short shoot sound
    //_gameWorld->queueSound(SOUND_SHOOT_ID);
    
    return true;
}

void Entity::updatePathWaypoint()
{
    // update path and waypoints, if any
    if (!_path || !_seekPawn) return;
    
    if (_currWaypoint < 0) _currWaypoint = 0;
    else _currWaypoint = _path->currentWaypoint(_position);
    
    if (!_currWaypoint) // no waypoints, so refresh path
    {
        int fromNode = _gameWorld->_map->getClosestNode(_position);
        int toNode = _gameWorld->_map->getClosestNode(_seekPawn->_position);
        _gameWorld->_map->getPath(fromNode, toNode, _path);
        _currWaypoint = _path->currentWaypoint(_position);
    }
    
    if (_currWaypoint)
    {
        _acceleration.add(seek(_gameWorld->_map->getNodePosition(_currWaypoint)));
    }
 }

HealthBar::HealthBar(GameWorld* gw) :
Entity::Entity(gw)
{
    printf("HealthBar() called; new instance created with id %d\n", id());
}

HealthBar::~HealthBar()
{
    printf("HealthBar::~HealthBar() called on entity %d)\n", id());
}

int HealthBar::id()
{
    return 8000 + Entity::id();
}

void HealthBar::update(float dTime)
{
    if (DEBUG_ENTITY) printf("HealthBar::update(dTime: %f), id: %d, gameWorld time: %d, elapsed time: %f\n", dTime, id(), _gameWorld->_time, _gameWorld->_elapsedTime);
    
    updateAnimation(dTime);
}


BoxPawn::BoxPawn(GameWorld* gw) :
Entity::Entity(gw)
{
    printf("BoxPawn() called; new instance created with id %d\n", id());

    _edgeMode = Entity::EDGE_REFLECT;
    
    _maxSpeed = 60;
    _maxAccel = 160;
    _turnSpeed = 270; // degrees per second
	_maxHealth = 8;
    
    _velocity.x = random() * _maxSpeed;
    _velocity.y = random() * _maxSpeed;
    _velocity.limit(_maxSpeed);
}

BoxPawn::~BoxPawn()
{
    printf("BoxPawn::~BoxPawn() called on entity %d)\n", id());
}

int BoxPawn::id()
{
    return 3000 + Entity::id();
}

void BoxPawn::update(float dTime)
{
    if (DEBUG_ENTITY) printf("BoxPawn::update(dTime: %f), id: %d, gameWorld time: %d, elapsed time: %f\n", dTime, id(), _gameWorld->_time, _gameWorld->_elapsedTime);

    if (!_active) return;
    
    checkMissileCollisions();
    
    updateAnimation(dTime);
    
    updateHealth(dTime);
    
    checkPlayerCollisions();
    
    _velocity.limit(_maxSpeed);
    
	_position.add(dTime * _velocity);

    // reverse spin when bouncing off walls
    if (checkBlockCollisions()) _turnSpeed *= -1;
    
    rotate((int)(_turnSpeed * dTime));
    
    return;
}


ShieldPawn::ShieldPawn(GameWorld* gw) :
Entity::Entity(gw)
{
    printf("ShieldPawn() called; new instance created with id %d\n", id());
    
    _edgeMode = Entity::EDGE_IGNORE;
    
    _maxSpeed = 60;
    _maxAccel = 160;
    _turnSpeed = 400; // degrees per second
	_maxHealth = 5;
}

ShieldPawn::~ShieldPawn()
{
    printf("ShieldPawn::~ShieldPawn() called on entity %d)\n", id());
}

int ShieldPawn::id()
{
    return 7000 + Entity::id();
}

void ShieldPawn::update(float dTime)
{
    if (DEBUG_ENTITY) printf("ShieldPawn::update(dTime: %f), id: %d, gameWorld time: %d, elapsed time: %f\n", dTime, id(), _gameWorld->_time, _gameWorld->_elapsedTime);
    
    if (!_active) return;
    
    if (!isTouching(_gameWorld->_selectedEntity)) return;
    
    _owner = _gameWorld->_selectedEntity;
    _position = _owner->_position;
    _velocity = _owner->_velocity; // so that bounce will work
    if (_frame == 0) _frame = 1;
    rotate(-(int)(_turnSpeed * dTime));
    
    checkMissileCollisions();
    updateAnimation(dTime);
    
    if (_health == 0) spawn();
    
    return;
}

void ShieldPawn::spawn()
{
    Entity::spawn();
    
    setHeading(0);
    
    if (_owner) _owner->_shield = 0;
    _owner = 0;
}

Weapon::Weapon(GameWorld* gw) :
Entity::Entity(gw)
{
    printf("Weapon() called; new instance created with id %d\n", id());
    
    _edgeMode = Entity::EDGE_IGNORE;
    
    _maxSpeed = 0;
    _maxAccel = 0;
    _turnSpeed = 100; // degrees per second
	_maxHealth = 5;

    _shotIntervalTime = 5;
}

Weapon::~Weapon()
{
    printf("Weapon::~Weapon() called on entity %d)\n", id());
}

int Weapon::id()
{
    return 7500 + Entity::id();
}

void Weapon::update(float dTime)
{
    if (DEBUG_ENTITY) printf("Weapon::update(dTime: %f), id: %d, gameWorld time: %d, elapsed time: %f\n", dTime, id(), _gameWorld->_time, _gameWorld->_elapsedTime);
    
    if (!_active || !_owner) return;
	
    //if (!isTouching(_gameWorld->_selectedEntity)) return; // for collectable weapon
    
    //_owner = _gameWorld->_selectedEntity;
    _position = _owner->_position;
	//_rotation = _owner->_rotation;
    
	// if tank is not occupied by player, gun is not functional
	if (!_owner->_owner) return;
        
    ControlInput ci2 = _gameWorld->_ci2;
    if (ci2.mode == iMode_touchDown)
	{
	}
	else if (ci2.mode == iMode_touchMoved)
    {
        //_visible = true;
        
		// normal drag aiming
		_aimPosition.x = _position.x + (ci2.x - ci2.startX);
		_aimPosition.y = _position.y + (ci2.y - ci2.startY);
		
        if (_aimPosition.lengthSq() > 2)
        {
            rotateTowardDirection(_aimPosition - _position, dTime * _turnSpeed);
            rotateTowardDirection(_aimPosition - _position, dTime * _turnSpeed);
            
			/*
            // if crosshair is touching either enemy, fire special shots
            // should we be checking a _gameWorld flag for this or something?
            if (_gameWorld->_touchControl2->isTouching(_gameWorld->_players[1]) 
                || _gameWorld->_touchControl2->isTouching(_gameWorld->_players[2]))
            {
                //printf("special shoot");
                shoot(_aimPosition, true);
            }
            else
            {
                shoot(_position + (_heading * 10), true);
            }
			*/
			shoot(_position + (_heading * 10), true);
        }
    }
    else if (ci2.mode == iMode_touchReleased)
    {
        //_visible = false;
        // TODO: need timestamp to detect tap at next touch
    }
}

void Weapon::spawn()
{
    Entity::spawn();
    
    setHeading(0);
    _owner = 0;
}

Vehicle::Vehicle(GameWorld* gw) :
Entity::Entity(gw)
{
    printf("Vehicle() called; new instance created with id %d\n", id());
    
    _edgeMode = Entity::EDGE_LIMIT;
    
    _maxSpeed = 100;
    _maxAccel = 160;
    _turnSpeed = 80; // degrees per second
	_maxHealth = 6;
    
    _mass = 3.0;
    
    _friction = .90;
}

Vehicle::~Vehicle()
{
    printf("~Vehicle() called on entity %d)\n", id());
}

int Vehicle::id()
{
    return 11000 + Entity::id();
}

void Vehicle::update(float dTime)
{
    if (DEBUG_ENTITY) printf("Vehicle::update(dTime: %f), id: %d, gameWorld time: %d, elapsed time: %f\n", dTime, id(), _gameWorld->_time, _gameWorld->_elapsedTime);
    
    if (!_active || !_owner) return;
    
    updateHealth(dTime);
    checkMissileCollisions();
    //checkPlayerCollisions(); // vehicle not slowed by other players for now
    checkBlockCollisions();
    updateAnimation(dTime);
    
    ControlInput ci1 = _gameWorld->_ci1; // move, pause input -- take a copy to avoid threading issues
    
    if (ci1.mode == iMode_touchDown)
    {
    }
    else if (ci1.mode == iMode_touchMoved)
    {
        // follow cursor mode: good for vehicle control
        _moveHeading.x = ci1.x - ci1.startX;
        _moveHeading.y = ci1.y - ci1.startY;
    }
    if (ci1.mode == iMode_touchReleased)
    {
        _moveHeading.zero();
    }
        
	// simulate some friction
	_velocity.x *= _friction;
	_velocity.y *= _friction;

    // rotate to adjust current heading if we have a desired heading
    if (_moveHeading.lengthSq() > 2)
    {
        float factor = _moveHeading.length() / 32; // px drag sens for max accel
        if (factor > 1.0) factor = 1.0;
        
        if (_velocity.length() > 16)
        {
            Vector turnToward = _moveHeading;
			/* reverse?
            if ((_heading | _moveHeading) < 0)
			{
				// drive in reverse
				turnToward.reverse();
				factor *= -1;
			}
			*/
            
            rotateTowardDirection(turnToward, dTime * _turnSpeed); // limited by turn speed
            rotateTowardDirection(turnToward, dTime * _turnSpeed); // limited by turn speed
        }   
        
        //_velocity = _heading * _maxSpeed * factor;
        
        // can only accelerate directly forward
        _acceleration = _heading * _maxAccel * factor;
                
        // simulate some friction
        //printf("friction: %f", _friction);
        //_velocity.x *= _friction;
        //_velocity.y *= _friction;
        
        _velocity.add(_acceleration * dTime);
        _velocity.limit(_maxSpeed);
		
		// limit velocity to heading to simulate traction
		/*
		int velocityDegrees = getVectorAngleDegrees(_velocity);
        int velocityDevianceDeg = velocityDegrees - _rotation;
		
		float velocityFactor = cos(velocityDevianceDeg * DEG_PER_RAD);
		if (velocityFactor < 0) velocityFactor *= -1;
		if (velocityFactor < .7) velocityFactor = .7;
		
		
		_velocity = _velocity * velocityFactor;
        _velocity.limit(_maxSpeed);
		*/
		
		//int i = 0;
		//if (i++ % 60 == 0) 
		//printf("velocityDegrees: %d, rotation: %d, velocityDevianceDeg: %d, velocityFactor: %f\n", velocityDegrees, _rotation, velocityDevianceDeg, velocityFactor);

        /*
        float len = _velocity.length();            
        _velocity.x = _heading.x * len;
        _velocity.y = _heading.y * len;
        */
    }
    else
    {
    }
    
    _position = _position + (_velocity * dTime);    
}

Copter::Copter(GameWorld* gw) :
Vehicle::Vehicle(gw)
{
    printf("Copter() called; new instance created with id %d\n", id());
    
    _edgeMode = Entity::EDGE_IGNORE;
    
    _maxSpeed = 140;
    _maxAccel = 100;
    _turnSpeed = 80; // degrees per second
	_maxHealth = 6;
    _shotIntervalTime = 5;
    _mass = 3.0;
    _friction = .98;
}

Copter::~Copter()
{
    printf("~Copter() called on entity %d)\n", id());
}

void Copter::update(float dTime)
{
    if (DEBUG_ENTITY) printf("Copter::update(dTime: %f), id: %d, gameWorld time: %d, elapsed time: %f\n", dTime, id(), _gameWorld->_time, _gameWorld->_elapsedTime);
        
    if (!_active || !_owner) return;
    
    updateHealth(dTime);
    checkMissileCollisions();
	
	// comment out this line if vehicle should not be slowed by collisions with other players
    //checkPlayerCollisions(); 

    checkBlockCollisions();
    updateAnimation(dTime);
    
    ControlInput ci1 = _gameWorld->_ci1; // move, pause input -- take a copy to avoid threading issues
    ControlInput ci2 = _gameWorld->_ci2; // rotate, aim -- take a copy to avoid threading issues
    
    if (ci1.mode == iMode_touchDown)
    {
    }
    else if (ci1.mode == iMode_touchMoved)
    {
        // follow cursor mode: good for vehicle control
        _moveHeading.x = ci1.x - ci1.startX;
        _moveHeading.y = ci1.y - ci1.startY;
    }
    if (ci1.mode == iMode_touchReleased)
    {
        _moveHeading.zero();
    }
    
	// use touch2 for rotation and shooting
    if (ci2.mode == iMode_touchDown)
    {
		//shoot(_position + (_heading * 10));
	}
	else if (ci2.mode == iMode_touchMoved)
	{
		_aimPosition.x = ci2.x - ci2.startX;
		_aimPosition.y = ci2.y - ci2.startY;
		//if ((_heading | _moveHeading) < 0) turnToward.reverse();
		
		if (_aimPosition.lengthSq() > 2)
		{
			rotateTowardDirection(_aimPosition, dTime * _turnSpeed); // limited by turn speed
			rotateTowardDirection(_aimPosition, dTime * _turnSpeed); // limited by turn speed
		}
		shoot(_position + (_heading * 10));

    }
    else if (ci2.mode == iMode_touchReleased)
    {
        _aimPosition.zero();
    }
    
    if (_moveHeading.lengthSq() > 2)
    {
		/*
		// rotate to adjust current heading if we have a desired heading
        if (_velocity.length() > 16)
        {
            Vector turnToward = _moveHeading;
            //if ((_heading | _moveHeading) < 0) turnToward.reverse();
            
            rotateTowardDirection(turnToward, dTime * _turnSpeed); // limited by turn speed
            rotateTowardDirection(turnToward, dTime * _turnSpeed); // limited by turn speed
        } 
		*/

		// px drag sens for max accel (at regular zoom TODO: adjust for zoom level
        int MAX_MOVE_CONTROL_THROW = 64;
        
		float factor = 1.0f;
		float moveHeadingLength = _moveHeading.length();
		if (moveHeadingLength < MAX_MOVE_CONTROL_THROW) factor = moveHeadingLength / MAX_MOVE_CONTROL_THROW; 
        //if (factor > 1.0) factor = 1.0;
        
        // simulate some friction
        //printf("friction: %f", _friction);
        _velocity.x *= _friction;
        _velocity.y *= _friction;
        
        // can only accelerate directly forward
        //_acceleration = _heading * _maxAccel * factor;
        
        // can accelerate in any direction
        _acceleration = _moveHeading * _maxAccel;
		_acceleration.limit(_maxAccel);
		_acceleration.x *= factor;
		_acceleration.y *= factor;
        
        // 
        _velocity.add(_acceleration * dTime);
        _velocity.limit(_maxSpeed);
        
		// The following velocity calculation method lets the user instantly control
		// the actual speed but the direction is still according to the heading
        //_velocity = _heading * _maxSpeed * factor;
    }
    else
    {
        // simulate some friction
        _velocity.x *= _friction;
        _velocity.y *= _friction;
    }
    
    _position = _position + (_velocity * dTime);    
}

GuardPawn::GuardPawn(GameWorld* gw) :
Entity::Entity(gw)
{
    printf("GuardPawn() called; new instance created with id %d\n", id());
    
    _edgeMode = Entity::EDGE_LIMIT;
    _maxSpeed = 60;
    _maxAccel = 200;
    _turnSpeed = 360; 
    _seekArriveDistance = 8;
    _shotIntervalTime = 30;
	_path = new Path();
	_maxHealth = 5;
}

GuardPawn::~GuardPawn()
{
    printf("GuardPawn::~GuardPawn() called on entity %d)\n", id());
}

int GuardPawn::id()
{
    return 6000 + Entity::id();
}

void GuardPawn::update(float dTime)
{
    if (DEBUG_ENTITY) printf("GuardPawn::update(dTime: %f), id: %d, gameWorld time: %d, elapsed time: %f\n", dTime, id(), _gameWorld->_time, _gameWorld->_elapsedTime);

    if (_spawnDelay > 0)
    {
        _spawnDelay -= dTime;
        _active = false;
        //printf("New GuardPawn._spawnDelay value: %f\n", _spawnDelay);
        return;
    }
    else
    {
        _active = true;
    }
    
    if (!_active)
	{
		_currWaypoint = -1; // clear waypoint
		return;
	}
    
    checkPlayerCollisions();
    
    checkMissileCollisions();

    updateAnimation(dTime);
    
    updateHealth(dTime);

    updatePathWaypoint();
    
    if (_avoidPawn) _acceleration.add(avoid(*_avoidPawn, 64));
    
    checkBlockCollisions();

    if (DEBUG_SHOOT) printf("Checking target... seekPawn id %d, active %d, isInView %d\n", _seekPawn->id(), _seekPawn->_active, _gameWorld->isInView(*this));
	if (_seekPawn && _seekPawn->_active && _gameWorld->isInView(*this))
    {
        _timeOnScreen++;
        if (DEBUG_SHOOT) printf("Time on screen for id %d: %d\n", id(), _timeOnScreen);

        // instantly aim along current heading
        Vector toPosition = _seekPawn->_position - _position;
        setHeading(toPosition);

        if (_timeOnScreen > 30 && isFacing(_seekPawn->_position))
        {
            if (DEBUG_SHOOT) printf("Calling shoot(position: %f, %f)", _seekPawn->_position.x, _seekPawn->_position.y);
            shoot(_seekPawn->_position);
        }
		if (_acceleration.lengthSq() == 0) return;
    }
    else
    {
        _timeOnScreen = 0;
		if (_acceleration.lengthSq() == 0) return;
		setHeading(_acceleration);
	}

	_acceleration.limit(_maxAccel);

	_velocity.add(dTime * _acceleration);

	_velocity.limit(_maxSpeed);

	// apply friction proportional to velocity
	//_velocity = (_velocity * 95) / 100;

	_position.add(dTime * _velocity);

	_acceleration.zero();
}

void GuardPawn::spawn()
{
    Entity::spawn();
    
    _spawnDelay = 5.0;
}

HeroPawn::HeroPawn(GameWorld* gw) :
Entity::Entity(gw)
{
    printf("HeroPawn() called; new instance created with id %d\n", id());
    
    _edgeMode           = Entity::EDGE_LIMIT;
    _maxSpeed           = 70;
    _maxAccel           = 160;
    _turnSpeed          = 360;
    _seekArriveDistance = 2;
	_maxHealth          = 10;
    _shotIntervalTime   = 5;
}

HeroPawn::~HeroPawn()
{
    printf("HeroPawn::~HeroPawn() called on entity %d)\n", id());
}

int HeroPawn::id()
{
    return 1000 + Entity::id();
}

void HeroPawn::update(float dTime)
{
    if (DEBUG_ENTITY) printf("HeroPawn::update(dTime: %f), id: %d, gameWorld time: %d, elapsed time: %f\n", dTime, id(), _gameWorld->_time, _gameWorld->_elapsedTime);
    
    if (!_active) return;
    
    if (_vehicle && _vehicle->_active && _vehicle->_owner == this)
    {
        if (_shield)
        {
            // drop shield upon vehicle entry
            _shield->spawn();
            _shield = 0;
        }        
        _position = _vehicle->_position;
        _velocity = _vehicle->_velocity; // e.g., so that shooting will be accurate
        _acceleration = _vehicle->_acceleration;
        _moveHeading = _vehicle->_moveHeading;
        setHeading(_vehicle->_rotation);
        
        return;
    }
    else
    {
        _vehicle = 0;
        
        // check for new vehicle entry
        for (int i = 0; i < _gameWorld->VEHICLE_COUNT; i++)
        {
            if (!_gameWorld->_vehicles[i]->_owner && isTouching(_gameWorld->_vehicles[i]))
            {
                _vehicle = _gameWorld->_vehicles[i];
                _vehicle->_owner = this;
                
                updateHealth(dTime);
                updateAnimation(dTime);
                return;
            }
            
        }
    }
    
    checkMissileCollisions();    
    checkPlayerCollisions();
    checkBlockCollisions();
    updateHealth(dTime);
    updateAnimation(dTime);
    
    ControlInput ci1 = _gameWorld->_ci1; // move, pause input -- take a copy to avoid threading issues

    if (ci1.mode == iMode_touchDown)
    {
    }
    else if (ci1.mode == iMode_touchMoved)
    {
        // swipe mode
        _moveHeading.x = ci1.x - ci1.x_trail_15;
        _moveHeading.y = ci1.y - ci1.y_trail_15;
    }
    if (ci1.mode == iMode_touchReleased)
    {
        _moveHeading.zero();
    }
    
    // rotate to adjust current heading if we have a desired heading
    if (_moveHeading.lengthSq() > 2)
    {
        setHeading(_moveHeading);
        
        // set acceleration
        _acceleration = _heading * _maxAccel;       
        _velocity.add(_acceleration * dTime);
        _velocity.limit(_maxSpeed);
    }
    else // no desired heading at this time, eg touch released
    {
        // slowly come to a stop to simulate friction
        _velocity.x *= .90f;
        _velocity.y *= .90f;
    }

    _position = _position + (_velocity * dTime);
    
    // ez 1-touch shoot
    /*
    if (_moveHeading.lengthSq() > 1)     // support single touch shooting
    {
        Vector aim = _position + _moveHeading;
        shoot(aim);
    }*/

    ControlInput ci2 = _gameWorld->_ci2;
    if (ci2.mode == iMode_touchDown)
	{
	}
	else if (ci2.mode == iMode_touchMoved)
    {
		_aimPosition.x = ci2.x - ci2.startX;
		_aimPosition.y = ci2.y - ci2.startY;
		
        if (_aimPosition.lengthSq() > 2)
        {
			shoot(_position + _aimPosition);
        }
    }
    else if (ci2.mode == iMode_touchReleased)
    {
    }
}

void HeroPawn::spawn()
{
    //int lastFrame = _frame;
    Entity::spawn();
    
    if (_shield && _shield->_owner == this)
    {
        _shield->spawn();
    }
    _shield = 0;
    
    // level-up!
    //_frame = ++lastFrame;
    //_maxAccel += 5;
    //_maxSpeed += 5;
    //_turnSpeed *= -1;
}

TouchControl::TouchControl(GameWorld* gw) :
    Entity::Entity(gw),
    _touchNum(),
    _moveOffset(),
    _latestControlInput(),
    _touchDownTimestamp(),
    _touchMoveTimestamp(),
    _touchUpTimestamp()
{
    printf("TouchControl(GameWorld*) called; new instance created with id %d\n", id());
    
    _edgeMode = Entity::EDGE_IGNORE;
}

TouchControl::~TouchControl()
{
    printf("TouchControl::~TouchControl() called on entity %d)\n", id());
}

int TouchControl::id()
{
    return 2000 + Entity::id();
}

void TouchControl::update(float dTime)
{
    if (DEBUG_ENTITY) printf("TouchControl::update(dTime: %f), id: %d, gameWorld time: %d, elapsed time: %f; touch num: %d\n", dTime, id(), _gameWorld->_time, _gameWorld->_elapsedTime, _touchNum);
    
    if (!_active) return;
	
	// to disable both touch cursors:
	//return;
    
    if (_touchNum == 1)
    {
        _latestControlInput = _gameWorld->_ci1;
    }
    else // default: if (_touchNum == 2) 
    {
        _latestControlInput = _gameWorld->_ci2;
    }
    
    //with sticky touch (stays where it was left): 
    //_position = _gameWorld->_selectedEntity->_position + _moveOffset;
    
    //swipe mode (reset to player position):
    _position = _gameWorld->_selectedEntity->_position;
	
	// TODO: hud mode (control cursor always stays in hud control box):
	//_position = // need position on screen, not in game world
	
    _position.x += (_latestControlInput.x - _latestControlInput.startX);
    _position.y += (_latestControlInput.y - _latestControlInput.startY);
    
    // a new touch after some time - 1 second in this case
    if (_latestControlInput.mode == iMode_touchDown && _latestControlInput.timestamp > _touchDownTimestamp)
    {
        if (_touchDownTimestamp - _touchUpTimestamp > 60)
        {
            _moveOffset.zero(); // reset move offset for a new touch TODO: tune timing for slow tap vs new touch
        }
        _touchDownTimestamp = _latestControlInput.timestamp;
        _frame = 0;
        _visible = true;
    }
    else if (_latestControlInput.mode == iMode_touchMoved && _latestControlInput.timestamp > _touchMoveTimestamp)
    {
        //_position.x += (_latestControlInput.x - _latestControlInput.startX);
        //_position.y += (_latestControlInput.y - _latestControlInput.startY);
        
        _touchMoveTimestamp = _latestControlInput.timestamp;
        _frame = 0;
        _visible = true;
    }
    else if (_latestControlInput.mode == iMode_touchReleased && _latestControlInput.timestamp > _touchUpTimestamp)
    {
        //printf("found new timestamp %d for touch num RELEASE: %d\n", _latestControlInput.timestamp, _touchNum);
        _touchUpTimestamp = _latestControlInput.timestamp;
        //_frame = 0;
        _visible = false;
        
        _moveOffset.x += _latestControlInput.x - _latestControlInput.startX;
        _moveOffset.y += _latestControlInput.y - _latestControlInput.startY;
    }
	
	// always hide move control cursor for now
    if (_touchNum == 1)
    {
        //_visible = false;
    }
	
}

MissilePawn::MissilePawn(GameWorld* gw) :
Entity::Entity(gw)
{
    printf("MissilePawn() called; new instance created with id %d\n", id());
    
    //_edgeMode = Entity::EDGE_REFLECT;
    //_edgeMode = Entity::EDGE_LIMIT;
    _edgeMode = Entity::EDGE_DISAPPEAR;
    
    _maxSpeed = 216;
    _maxAccel = 800;
}

MissilePawn::~MissilePawn()
{
    printf("MissilePawn::~MissilePawn() called on entity %d)\n", id());
}

int MissilePawn::id()
{
    return 9000 + Entity::id();
}

void MissilePawn::update(float dTime)
{
    if (DEBUG_ENTITY) printf("MissilePawn::update(dTime: %f), id: %d, gameWorld time: %d, elapsed time: %f\n", dTime, id(), _gameWorld->_time, _gameWorld->_elapsedTime);
    
    if (!_active) return;
    
    //checkPlayerCollisions();
    
    updateAnimation(dTime);
    
    if (checkBlockCollisions())
    {
        // play sound at each bounce
        //_gameWorld->queueSound(SOUND_BOUNCE_ID); // need better sound effect
        
        _active = false;
    }
    
    if (_velocity.lengthSq() == 0) return;

    _position.add(_velocity * dTime);
    
}

FlyerPawn::FlyerPawn(GameWorld* gw) :
Entity::Entity(gw)
{
    printf("FlyerPawn() called; new instance created with id %d\n", id());
    
    _edgeMode = Entity::EDGE_REFLECT;

    _maxSpeed = 120;
    _maxAccel = 80;
    _turnSpeed = 240;
    _seekArriveDistance = 64;
	_maxHealth = 3;
    _avoidPawn = 0;
}

FlyerPawn::~FlyerPawn()
{
    printf("FlyerPawn::~FlyerPawn() called on entity %d)\n", id());
}

int FlyerPawn::id()
{
    return 4000 + Entity::id();
}

void FlyerPawn::update(float dTime)
{
    if (DEBUG_ENTITY) printf("FlyerPawn::update(dTime: %f), id: %d, gameWorld time: %d, elapsed time: %f\n", dTime, id(), _gameWorld->_time, _gameWorld->_elapsedTime);

    if (!_active) return;
    
    checkMissileCollisions();
    updateAnimation(dTime);    
    updateHealth(dTime);
    checkPlayerCollisions();
    
    _acceleration = _seekPawn->_position - _position; // accelerate directly toward the target pawn
    
    setHeading(_velocity); // flyby, ignore player

    if (_avoidPawn) _acceleration.add(avoid(*_avoidPawn, 64));

    if (_acceleration.lengthSq() == 0) return;

    _acceleration.limit(_maxAccel);
    _velocity.add(dTime * _acceleration);
    _velocity.limit(_maxSpeed);
	_position.add(dTime * _velocity);
    
    // hard limit position to edge of gameworld, if not could lose entity 
    // but origin of world is top-left corner of map, so add some room for flyers, say 128?
    int extraRoom = 256; // let them go off screen and then fly across in waves chasing thier leader
    if (_position.x + _radius > _gameWorld->_size.x/2 + extraRoom) _position.x = _gameWorld->_size.x/2 - _radius + extraRoom;
    else if (_position.x - _radius < -extraRoom) _position.x = _radius - extraRoom;
    if (_position.y + _radius > _gameWorld->_size.y/2 + extraRoom) _position.y = _gameWorld->_size.y/2 - _radius + extraRoom;
    else if (_position.y - _radius < -extraRoom) _position.y = _radius - extraRoom;
    
    return;
}

void FlyerPawn::spawn()
{
    int lastFrame = _frame;
    Entity::spawn();
    
    // level-up!
    _frame = ++lastFrame;
    //_maxAccel += 5;
    //_maxSpeed += 5;
    //_turnSpeed *= -1;
}

