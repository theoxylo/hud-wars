/* 
 * Simple game object framework by Theoxylo, copyright 2008
 */

#ifndef ENTITY_H
#define ENTITY_H



#include "Vector.h"
#include "LevelMap.h"
#include "Animation.h"
#include "GameWorld.h"
#include "SpriteLib.h"
#include "ControlInput.h"

class GameWorld;
class ShieldPawn;
class HealthBar;
class Vehicle;

/*
 * An Entity is a stateful game component such as a Pawn
 */
class Entity
{
protected:

    int _instanceId;

public:

    static const int MAX_NUM_INSTANCES = 128;
    static Entity* __instances[MAX_NUM_INSTANCES];
    static int __nextInstanceId;

    virtual int id();
    
    
    Entity(GameWorld* gw);
    
    virtual ~Entity();

    GameWorld * _gameWorld;


    virtual void update(float dTime);

    virtual void spawn();
    float _spawnDelay;
    
    void checkMissileCollisions();
    void checkPlayerCollisions();
    void handleCollision(const Entity* const other);

    // use existing pawn entity as a basis
    void initialize(Entity* other);

    // Update the internal sprite data for rendering
	void render();
	bool _active;
	bool _visible;

    // radius of the bounding circle, default is half the sprite size
    float _radius; // also used as center offset

    // position of the entity center on the game board
    Vector _position; // on gameboard (not screen)

    // parent
    Entity* _owner;
    
    Entity* _vehicle; // Entity subtype
    
    Sprite* _sprite;


    void updateAnimation(float dTime);
    Animation* _animation;
    int _frame; // current sprite frame (for rendering)


    // Orientation or "what direction is entity facing?": 
    // angle of rotation in degrees and unit vector - these 2 fields are kept in sync by the setHeading methods
    Vector _heading;
    int _rotation; // heading in 360 degree circle, 0 is East
    
    inline void setHeading(int angle)
    {
        _rotation = angle % 360;
		if (_rotation < 0) _rotation += 360;

        // keep vector repesentation synced
        //static float radPerDeg = 2.0 * M_PI / 360.0;
        
        _heading.set(cos(float(angle) * RAD_PER_DEG), sin(float(angle) * RAD_PER_DEG));
        
        //float angleRad = ((float)_rotation / 360.0) * 2 * M_PI;
        //_heading.set(cos(angleRad), sin(angleRad));
        
        //printf("setHeading(int %d) called, new vector heading x, y: %f, %f\n", angle, _heading.x, _heading.y);
    }
    
    inline void setHeading(Vector v) // by value
    {
        if (v.lengthSq() == 0) return;
        
        _heading = v.limit(1);
        //_heading = v.normalize();
        //_heading = v;
        
        // keep degree repesentation synced
        //static float degPerRad = 360 / (2.0 * M_PI);
        _rotation = acos(_heading.x) * DEG_PER_RAD;
        
        if (_heading.y < 0) _rotation = (360 - _rotation);
    }

    // position of the Entity center on the game board
    Vector _velocity; // on gameboard (not screen)
    Vector _acceleration; // on gameboard (not screen)

    float _maxSpeed;
    float _maxAccel;
    float _turnSpeed; // degrees per second to turnL/R(); may be negative (reverse)
    
    float _mass; // default 1.0

    Vector _moveHeading;
    
	// this is the world position the entity is aiming at
    // e.g. used for touch-2 aim-shoot cursor
    Vector _aimPosition;
    
    // determines wall handling
    enum EdgeMode {EDGE_DISAPPEAR, EDGE_REFLECT, EDGE_LIMIT, EDGE_AVOID, EDGE_IGNORE};
    EdgeMode _edgeMode;

    enum BlockTouch {NOT_NEAR, NOT_NEAR_Y, NEAR_X, NEAR_W, NEAR_Y, NEAR_H};
	BlockTouch handleBlockCollision(float x, float y, float w, float h);
    bool checkBlockCollisions();

    /*
     * higher level AI method to move toward other Entity
     */
	Vector seek(const Vector& position);

    float _seekArriveDistance; // threshold to reduce accel (linear) as approaching

    Entity* _seekPawn;
    Entity* _avoidPawn;
    
	Path* _path;

    /*
     * higher level AI method to move away from other pawn if within range
     */
	Vector avoid(Entity& other, int range);
	Vector avoid(Vector pos, int range);

    Entity * _currentMissile;
    int _lastShotTimestamp;
    int _shotIntervalTime;
    int _timeOnScreen;
	bool shoot();
	bool shoot(Vector targetPosition);
	bool shoot(Vector targetPosition, bool special);

   	/*
	 * return true if distance between positions is less than touch radii sum + clearance
	 * e.g.
	 */
	bool isTouching(const Entity* const other);


    bool isFacing(Vector position);
    void rotateTowardDirection(Vector direction, int delta360);

    void rotate(int delta360);

	// health hp
    void updateHealth(float dTime);
	int _health;
	int _maxHealth;
    Entity* _healthBar;
    ShieldPawn* _shield;
    
    void updatePathWaypoint();
    int _currWaypoint;

};

class HealthBar : public Entity
{
public:
    HealthBar(GameWorld* gameWorld);
    ~HealthBar();
    int id();
    void update(float dTime);
};

class BoxPawn : public Entity
{
public:
    BoxPawn(GameWorld* gameWorld);
    ~BoxPawn();
    int id();
    void update(float dTime);
};

class ShieldPawn : public Entity
    {
    public:
        ShieldPawn(GameWorld* gameWorld);
        ~ShieldPawn();
        int id();
        void update(float dTime);
        void spawn();
    };

class Weapon : public Entity
    {
    public:
        Weapon(GameWorld* gameWorld);
        ~Weapon();
        int id();
        void update(float dTime);
        void spawn();
    };

class Vehicle : public Entity
    {
    public:
        Vehicle(GameWorld* gameWorld);
        ~Vehicle();
        int id();
        void update(float dTime);
        
        float _friction; // determines handling of vehicle
    };

class Copter : public Vehicle
    {
    public:
        Copter(GameWorld* gameWorld);
        ~Copter();
        void update(float dTime);
    };

class GuardPawn : public Entity
{
public:
    GuardPawn(GameWorld* gameWorld);
    ~GuardPawn();
    int id();
    void update(float dTime);
    void spawn();
};

class HeroPawn : public Entity
{
public:
    HeroPawn(GameWorld* gameWorld);
    ~HeroPawn();
    int id();
    void update(float dTime);
    void spawn();
};

class FlyerPawn : public Entity
{
public:
    FlyerPawn(GameWorld* gameWorld);
    ~FlyerPawn();
    int id();
    void update(float dTime);
    void spawn();
};

class TouchControl : public Entity
{
    public:
        TouchControl(GameWorld* gameWorld);
        ~TouchControl();
        int id();
        void update(float dTime);
        
        int _touchNum;
        Vector _moveOffset;
        ControlInput _latestControlInput;
        int _touchDownTimestamp;
        int _touchMoveTimestamp;
        int _touchUpTimestamp;
};

class MissilePawn : public Entity
{
public:
    MissilePawn(GameWorld* gameWorld);
    ~MissilePawn();
    int id();
    void update(float dTime);
};

#endif
