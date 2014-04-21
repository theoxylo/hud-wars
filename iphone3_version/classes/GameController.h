//  GameController.h
//
//  Created by Ted O'Neill on 10/25/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//
#import "Texture2D.h"
//#import "SoundEngine.h"
#import "ControlInput.h"

static const bool DEBUG_GAME_CONTROLLER = false;
static const bool DEBUG_TEXTURE = false;

#define NUM_TEXTURES 128


enum 
{
    kSound_none   = 0,
	kSound_Thrust = 1,
	kSound_Start  = 2,
	kSound_Shoot  = 3,
	kSound_Bounce = 4,
	kSound_Hit    = 5,
	kNumSounds
};

typedef struct
{
    float x;
    float y;
    
} Vector2D;

typedef struct
{
    int id; // _textures index

    float positionX;
    float positionY;

    int rotation;     // degrees 360      
        
} TextureData;

@interface GameController : NSObject 
{
    float _totalGameTime;
    
    int _loopCount;
    
	Texture2D*  _textures[NUM_TEXTURES];
    Texture2D*	_hudScreen;
    Texture2D*	_mapBackground;

    // sound
    UInt32					_sounds[kNumSounds];
    BOOL					_playingMoveSound;
    
    ControlInput            _ci[2];
}

- (void)initGame;
- (void)updateGame:(float)dTime;

- (void)renderTexture:(TextureData)td;

- (void)sendTouchControlInput:(int)num AndMode:(int)mode AndX:(float)x AndY:(float)y;


- (void)updateSound:(float)dTime;

@end
