//  GameController.m
//
//  Created by Ted O'Neill on 10/25/08.
//  Copyright 2008 Theoxlo. All rights reserved.
//
#import "GameController.h"
#import "GameWorld.h"

#define isDebugEnabled              NO
#define driveMode                   NO

#define kRotationSpeed				90   //Degrees/s
#define kMaxVelocity				75   //Pixels/s
#define kMaxAcceleration			75   //Pixels/s

#define kListenerDistance			1.0  // Used for creating a realistic sound field

@implementation GameController

GameWorld gameWorld;

Vector _deltaMove;

- (void)initGame
 {
     NSLog(@"initGame called");
     _loopCount = 0;

     _mapBackground     = [[Texture2D alloc] initWithImage:[UIImage imageNamed:@"Background.png"]];
     _hudScreen = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"Hud.png"]];
          
     // needed, but why?
     //glBindTexture(GL_TEXTURE_2D, [_mapBackground name]);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

     // 0
     //_textures[TEXTURE_BASE_IDX_CURSOR]    = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"cursor_0.png"]];
     //_textures[TEXTURE_BASE_IDX_CURSOR +1] = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"cursor_1.png"]];
     
     // 10
     _textures[TEXTURE_BASE_IDX_HEALTHBAR]    = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"health_0.png"]];
     _textures[TEXTURE_BASE_IDX_HEALTHBAR +1] = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"health_1.png"]];
     _textures[TEXTURE_BASE_IDX_HEALTHBAR +2] = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"health_2.png"]];
     _textures[TEXTURE_BASE_IDX_HEALTHBAR +3] = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"health_3.png"]];
     _textures[TEXTURE_BASE_IDX_HEALTHBAR +4] = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"health_4.png"]];
     _textures[TEXTURE_BASE_IDX_HEALTHBAR +5] = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"health_5.png"]];
     _textures[TEXTURE_BASE_IDX_HEALTHBAR +6] = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"health_6.png"]];
     _textures[TEXTURE_BASE_IDX_HEALTHBAR +7] = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"health_7.png"]];
     _textures[TEXTURE_BASE_IDX_HEALTHBAR +8] = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"health_8.png"]];
     _textures[TEXTURE_BASE_IDX_HEALTHBAR +9] = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"health_9.png"]];
     
     // 20
     _textures[TEXTURE_BASE_IDX_PLAYER]    = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"player_0.png"]];
     _textures[TEXTURE_BASE_IDX_PLAYER +1] = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"player_1.png"]];
     
     // 30
     _textures[TEXTURE_BASE_IDX_VEHICLE +0] = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"vehicle_0.png"]];
     _textures[TEXTURE_BASE_IDX_VEHICLE +1] = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"vehicle_1.png"]];
     _textures[TEXTURE_BASE_IDX_VEHICLE +2] = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"copter_0.png"]];
     _textures[TEXTURE_BASE_IDX_VEHICLE +3] = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"copter_1.png"]];
     _textures[TEXTURE_BASE_IDX_VEHICLE +4] = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"jet_0.png"]];
     _textures[TEXTURE_BASE_IDX_VEHICLE +5] = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"jet_1.png"]];
     
     // 40
     _textures[TEXTURE_BASE_IDX_GUARD]     = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"guard_0.png"]];
     _textures[TEXTURE_BASE_IDX_GUARD +1]  = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"guard_1.png"]];

     // 50
     _textures[TEXTURE_BASE_IDX_BOX]       = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"boxpawn_0.png"]];
     
     // 60
     _textures[TEXTURE_BASE_IDX_FLYER]        = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"flyer_0.png"]];
     _textures[TEXTURE_BASE_IDX_FLYER +1]     = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"flyer_1.png"]];
     _textures[TEXTURE_BASE_IDX_FLYER +2]     = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"flyer_2.png"]];
     _textures[TEXTURE_BASE_IDX_FLYER +3]     = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"flyer_3.png"]];
     _textures[TEXTURE_BASE_IDX_FLYER +4]     = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"flyer_4.png"]];
     _textures[TEXTURE_BASE_IDX_FLYER +5]     = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"flyer_5.png"]];
     _textures[TEXTURE_BASE_IDX_FLYER +6]     = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"flyer_6.png"]];
     _textures[TEXTURE_BASE_IDX_FLYER +7]     = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"flyer_7.png"]];

     // 70
     _textures[TEXTURE_BASE_IDX_CROSSHAIR]    = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"crosshair_0.png"]];
     _textures[TEXTURE_BASE_IDX_CROSSHAIR +1] = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"crosshair_1.png"]];
     _textures[TEXTURE_BASE_IDX_CROSSHAIR +2] = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"crosshair_2.png"]];
     _textures[TEXTURE_BASE_IDX_CROSSHAIR +3] = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"crosshair_3.png"]];

     // 80
     _textures[TEXTURE_BASE_IDX_MISSILE]   = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"missile_0.png"]];     
     _textures[TEXTURE_BASE_IDX_MISSILE +1]= [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"missile_1.png"]];     
     _textures[TEXTURE_BASE_IDX_MISSILE +2]= [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"missile_2.png"]];     
     _textures[TEXTURE_BASE_IDX_MISSILE +3]= [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"missile_3.png"]];     

     // 90
     _textures[TEXTURE_BASE_IDX_SHIELD]    = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"shield_0.png"]];
     _textures[TEXTURE_BASE_IDX_SHIELD +1] = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"shield_1.png"]];
     _textures[TEXTURE_BASE_IDX_SHIELD +2] = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"shield_2.png"]];
     
     //100
     _textures[TEXTURE_BASE_IDX_WEAPON]    = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"weapon_0.png"]];     

     // for debugging missing images:
     /*
     for (int i = 0; i < 128; i++)
     {
         if (!_textures[i]) NSLog(@"No texture for idx: %d\n", i);
         else
         {
             NSLog(@"Found texture for idx: %d\n", i);
             NSLog([_textures[i] description]);
         }
     }
     NSLog(@"Player base texture id %d; description:", TEXTURE_BASE_IDX_PLAYER);
     NSLog([_textures[TEXTURE_BASE_IDX_PLAYER] description]);
     */
     
     // TODO sounds
     _playingMoveSound = NO;

     
     _ci[0].touchNumber = 1;
     _ci[0].mode = iMode_none;
     _ci[0].isNew = false;
     _ci[0].timestamp = 0;
     _ci[0].updateCount = 0;
     _ci[0].hasMoved = false;
     _ci[0].startX = 0;
     _ci[0].startY = 0;
     _ci[0].x = 0;
     _ci[0].y = 0;
     _ci[0].x_trail_1 = 0;
     _ci[0].x_trail_2 = 0;
     _ci[0].x_trail_3 = 0;
     _ci[0].x_trail_4 = 0;
     _ci[0].x_trail_5 = 0;
     _ci[0].x_trail_6 = 0;
     _ci[0].x_trail_7 = 0;
     _ci[0].x_trail_8 = 0;
     _ci[0].x_trail_9 = 0;
     _ci[0].x_trail_10 = 0;
     _ci[0].x_trail_11 = 0;
     _ci[0].x_trail_12 = 0;
     _ci[0].x_trail_13 = 0;
     _ci[0].x_trail_14 = 0;
     _ci[0].x_trail_15 = 0;
     _ci[0].y_trail_1 = 0;
     _ci[0].y_trail_2 = 0;
     _ci[0].y_trail_3 = 0;
     _ci[0].y_trail_4 = 0;
     _ci[0].y_trail_5 = 0;
     _ci[0].y_trail_6 = 0;
     _ci[0].y_trail_7 = 0;
     _ci[0].y_trail_8 = 0;
     _ci[0].y_trail_9 = 0;
     _ci[0].y_trail_10 = 0;
     _ci[0].y_trail_11 = 0;
     _ci[0].y_trail_12 = 0;
     _ci[0].y_trail_13 = 0;
     _ci[0].y_trail_14 = 0;
     _ci[0].y_trail_15 = 0;
     
     _ci[1].touchNumber = 2;
     _ci[1].mode = iMode_none;
     _ci[1].isNew = false;
     _ci[1].timestamp = 0;
     _ci[1].updateCount = 0;
     _ci[1].hasMoved = false;
     _ci[1].startX = 0;
     _ci[1].startY = 0;
     _ci[1].x = 0;
     _ci[1].y = 0;
     _ci[1].x_trail_1 = 0;
     _ci[1].x_trail_2 = 0;
     _ci[1].x_trail_3 = 0;
     _ci[1].x_trail_4 = 0;
     _ci[1].x_trail_5 = 0;
     _ci[1].x_trail_6 = 0;
     _ci[1].x_trail_7 = 0;
     _ci[1].x_trail_8 = 0;
     _ci[1].x_trail_9 = 0;
     _ci[1].x_trail_10 = 0;
     _ci[1].x_trail_11 = 0;
     _ci[1].x_trail_12 = 0;
     _ci[1].x_trail_13 = 0;
     _ci[1].x_trail_14 = 0;
     _ci[1].x_trail_15 = 0;
     _ci[1].y_trail_1 = 0;
     _ci[1].y_trail_2 = 0;
     _ci[1].y_trail_3 = 0;
     _ci[1].y_trail_4 = 0;
     _ci[1].y_trail_5 = 0;
     _ci[1].y_trail_6 = 0;
     _ci[1].y_trail_7 = 0;
     _ci[1].y_trail_8 = 0;
     _ci[1].y_trail_9 = 0;
     _ci[1].y_trail_10 = 0;
     _ci[1].y_trail_11 = 0;
     _ci[1].y_trail_12 = 0;
     _ci[1].y_trail_13 = 0;
     _ci[1].y_trail_14 = 0;
     _ci[1].y_trail_15 = 0;
     
     // done loading, so play start sound as Default.png (load screen) is replaced by Title.png (start screen)
     //SoundEngine_StartEffect(_sounds[kSound_Shoot]);
 }

- (void)updateGame:(float)dTime
{
    _loopCount++;
    _totalGameTime += dTime;
    
    if (DEBUG_GAME_CONTROLLER && !(_loopCount % 60))
    {
        NSLog(@"************************************");
        NSLog(@"Loop count: %d, Total game time: %f\n", _loopCount, _totalGameTime);   
    }
    
    // update model (agent behavior, physics) and check state every frame
    // update model (agent behavior, physics) and check state every frame
    // update model (agent behavior, physics) and check state every frame
    // update model (agent behavior, physics) and check state every frame
    // update model (agent behavior, physics) and check state every frame
    // update model (agent behavior, physics) and check state every frame
    // update model (agent behavior, physics) and check state every frame
    // update model (agent behavior, physics) and check state every frame
    // update model (agent behavior, physics) and check state every frame
    //
    gameWorld.update(dTime);
    
    
    // play queued gameWorld sounds
    
    // play one sound per update cycle -- this is really a stack, not a queue, so some sounds may get delayed
    // during busy times until newer sounds are played
    if (gameWorld._nextSoundQueueIdx > 0)
    {
        //int currentSoundQueueIdx = --gameWorld._nextSoundQueueIdx;
        //UInt32 soundId = gameWorld._soundQueue[currentSoundQueueIdx];
        //if (soundId < NUM_SOUNDS) SoundEngine_StartEffect(_sounds[soundId]);
    }
    
    /* or, could play all queued sounds each cycle:
    for (int i = 0; i < gameWorld._nextSoundQueueIdx; i++)
    {
        UInt32 soundId = gameWorld._soundQueue[i];
        if (soundId > NUM_SOUNDS -1) continue;
        
        SoundEngine_StartEffect(_sounds[soundId]);
    }
    gameWorld._nextSoundQueueIdx = 0;
    */
    
    if (isDebugEnabled && !(_loopCount % 160))
    {
        NSLog(@"GameController - time (seconds): %f", dTime);
        NSLog(@"GameController - Loop count: %d", _loopCount);
        NSLog(@"GameController: Total Game Time (seconds): %f", _totalGameTime);
        NSLog(@"GameController: touch 1, number of updates: %d", gameWorld._ci1.updateCount);
        
        NSLog(@"Score: %d", gameWorld._score);
        NSLog(@"Game time (total frames): %d", gameWorld._time); // frame count
        NSLog(@"Game time: %f", gameWorld._elapsedTime); // seconds
        NSLog(@"P1 Pos: x = %f, y = %f", gameWorld._players[0]->_position.x, gameWorld._players[0]->_position.y);
        NSLog(@"P1 Vel: x = %f, y = %f", gameWorld._players[0]->_velocity.x, gameWorld._players[0]->_velocity.y);
        NSLog(@"P1 Acc: x = %f, y = %f", gameWorld._players[0]->_acceleration.x, gameWorld._players[0]->_acceleration.y);
        NSLog(@"P1 Rot: x = %f, y = %f, angle = %d", gameWorld._players[0]->_heading.x, gameWorld._players[0]->_heading.y, gameWorld._players[0]->_rotation);
        NSLog(@"E1 Pos: x = %f, y = %f", gameWorld._players[1]->_position.x, gameWorld._players[1]->_position.y);
        NSLog(@"E1 Vel: x = %f, y = %f", gameWorld._players[1]->_velocity.x, gameWorld._players[1]->_velocity.y);
        NSLog(@"ViewPs: x = %f, y = %f", gameWorld._viewPosition.x, gameWorld._viewPosition.y);
        NSLog(@"M1 Pos: x = %f, y = %f", gameWorld._missiles[0]->_position.x, gameWorld._missiles[0]->_position.y);
        NSLog(@"P1 NavNodeIdx:  %d", gameWorld._map->getClosestNode(gameWorld._players[0]->_position));
        NSLog(@"Collision E1-M1:  %d", gameWorld._players[1]->isTouching(gameWorld._missiles[0]));
        NSLog(@"\nSelected entity id: %d\n", gameWorld.selectedEntityId());
    }
        
    glClearColor(.2, .2, .3, 1); // grey
    glClear(GL_COLOR_BUFFER_BIT);
    
    //Render the HUD full screen
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrthof(0, 320, 0, 480, -1, 1);
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, 320, 480); // full screen
    glScissor(0, 0, 320, 480); // full screen

	[_hudScreen drawAtPoint:CGPointMake(160, 240)]; // offset to center
    
    
    // portlet shape and position to match HUD type, currently only landscape is supported
    // 0 for centered hud (landscape), 1 for top hud (portrait), 2 for small window (minimap?), other for full screen
    //int enum HUD_MODE = {portrait_frame, landscape_frame, full_screen}; 
    
    static int hud_mode = 2; // hm = portrait_frame;
    
    if (gameWorld._paused)
    {
        hud_mode = 1; // toolbox mode
        //gameWorld._zoom_out = true;
    }
    else
    {
        hud_mode = 2; // full screen action
        //gameWorld._zoom_out = false;
    }
	
	// always show frame:
	//hud_mode = 1;
    
    
    switch (hud_mode) 
    {
        case 0: // portrait - TODO: add app/view support for phone orientation
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            if (gameWorld._zoom_out) glOrthof(-304, 304, -304, 304, -1, 1); // zoomed out x2
            else glOrthof(-152, 152, -152, 152, -1, 1); // native zoom
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glViewport(8, 168, 304, 304);  // top HUD portlet for square portrait view
            glScissor(8, 168, 304, 304);  // top HUD portlet for square portrait view
            break;
            
        case 1: // landscape - toolbox frame mode for pause screen
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            if (gameWorld._zoom_out) glOrthof(-304, 304, -304, 304, -1, 1); // zoomed out x2
            else glOrthof(-152, 152, -152, 152, -1, 1); // native zoom
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glViewport(8, 88, 304, 304);   // centered HUD portlet for square landscape view
            glScissor(8, 88, 304, 304);   // centered HUD portlet for square landscape view
            break;
            
        case 2: // full screen action mode
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            if (gameWorld._zoom_out) glOrthof(-220, 220, -330, 330, -1, 1); // zoomed out
            else glOrthof(-160, 160, -240, 240, -1, 1); // native zoom
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glViewport(0, 0, 320, 480);    // full screen
            glScissor(0, 0, 320, 480);    // full screen
            break;
    }
    
    //Draw map background based on view position
    float drawPositionX = gameWorld._size.x/4 - gameWorld._viewPosition.x;
    float drawPositionY = gameWorld._size.y/4 - gameWorld._viewPosition.y;
    [_mapBackground drawAtPoint:CGPointMake(drawPositionX, -drawPositionY)];
    

    // test bg tile render
    /*
    for (int y = -240; y <= 752; y += 32)
    {
        for (int x = -240; x <= 752; x += 32)
        {
            [_textures[TEXTURE_BASE_IDX_SHIELD +1] drawAtPoint:CGPointMake(x - gameWorld._viewPosition.x, -(y - gameWorld._viewPosition.y))];
        }
    }
	*/
     
    
    // debug
    //[_textures[TEXTURE_BASE_IDX_SHIELD +1] drawAtPoint:CGPointZero]; // screen center
    //[_textures[TEXTURE_BASE_IDX_SHIELD +2] drawAtPoint:CGPointMake(0 - gameWorld._viewPosition.x, -(0 - gameWorld._viewPosition.y))]; // game world origin (0,0)

    
    // this flushes all pawn render data to gameWorld._worldSprites and resets getNextSprite() iterator
    gameWorld.render();
    
    Sprite* sprite;
    while (sprite = gameWorld.getNextSprite())
    {
        if (!sprite->visible) continue; 
        
        TextureData td;
        td.id = sprite->id + sprite->frame;
        td.positionX = sprite->posX;
        td.positionY = sprite->posY;
        td.rotation = sprite->rotation;
        
        if (DEBUG_TEXTURE) printf("\n\nSprite Texture Data - sprite id: %d, sprite frame: %d, texture id: %d, x: %f, y: %f, rotation: %d\n", sprite->id, sprite->frame, td.id, td.positionX, td.positionY, td.rotation);
        
        [self renderTexture:td];
	}
    
}

- (void) renderTexture:(TextureData)texture
{
    //NSLog(@"Texture id: %d", texture.id);
    //NSLog(@"Texture world position: %f, %f", texture.positionX, texture.positionY);
    
    glPushMatrix();    
    
    glTranslatef(texture.positionX - gameWorld._viewPosition.x, gameWorld._viewPosition.y - texture.positionY, 0); // player in upper screen area (portrait)
     
    // rotate
    glRotatef(360 - texture.rotation, 0, 0, 1);
    
    //Draw texture
    [_textures[texture.id] drawAtPoint:CGPointZero];
    
    glPopMatrix();
}

- (void)sendTouchControlInput:(int)num AndMode:(int)mode AndX:(float)x AndY:(float)y
{
    // only support first 2 touches
    if (num != 1 && num != 2)
    {
        NSLog(@"******************************* ERROR: invalid touch num %d", num);
        return;
    }
    
    _ci[num-1].updateCount++;
    _ci[num-1].isNew = true;
    
    if (mode == iMode_touchDown)
    {
        _ci[num-1].mode = iMode_touchDown;
        _ci[num-1].hasMoved = false;
        
        _ci[num-1].startX = x;
        _ci[num-1].startY = y;
        
        // assign x start history
        _ci[num-1].x = x;
        _ci[num-1].x_trail_1 = x;
        _ci[num-1].x_trail_2 = x;
        _ci[num-1].x_trail_3 = x;
        _ci[num-1].x_trail_4 = x;
        _ci[num-1].x_trail_5 = x;
        _ci[num-1].x_trail_6 = x;
        _ci[num-1].x_trail_7 = x;
        _ci[num-1].x_trail_8 = x;
        _ci[num-1].x_trail_9 = x;
        _ci[num-1].x_trail_10 = x;
        _ci[num-1].x_trail_11 = x;
        _ci[num-1].x_trail_12 = x;
        _ci[num-1].x_trail_13 = x;
        _ci[num-1].x_trail_14 = x;
        _ci[num-1].x_trail_15 = x;

        // assign y start history
        _ci[num-1].y = y;
        _ci[num-1].y_trail_1 = y;        
        _ci[num-1].y_trail_2 = y;
        _ci[num-1].y_trail_3 = y;
        _ci[num-1].y_trail_4 = y;
        _ci[num-1].y_trail_5 = y;
        _ci[num-1].y_trail_6 = y;
        _ci[num-1].y_trail_7 = y;
        _ci[num-1].y_trail_8 = y;
        _ci[num-1].y_trail_9 = y;
        _ci[num-1].y_trail_10 = y;
        _ci[num-1].y_trail_11 = y;
        _ci[num-1].y_trail_12 = y;
        _ci[num-1].y_trail_13 = y;
        _ci[num-1].y_trail_14 = y;
        _ci[num-1].y_trail_15 = y;
    }
    else if (mode == iMode_touchMoved)
    {
        _ci[num-1].mode = iMode_touchMoved;
        _ci[num-1].hasMoved = true;
        
        // update x history
        _ci[num-1].x_trail_15 = _ci[num-1].x_trail_14;
        _ci[num-1].x_trail_14 = _ci[num-1].x_trail_13;
        _ci[num-1].x_trail_13 = _ci[num-1].x_trail_12;
        _ci[num-1].x_trail_12 = _ci[num-1].x_trail_11;
        _ci[num-1].x_trail_11 = _ci[num-1].x_trail_10;
        _ci[num-1].x_trail_10 = _ci[num-1].x_trail_9;
        _ci[num-1].x_trail_9 = _ci[num-1].x_trail_8;
        _ci[num-1].x_trail_8 = _ci[num-1].x_trail_7;
        _ci[num-1].x_trail_7 = _ci[num-1].x_trail_6;
        _ci[num-1].x_trail_6 = _ci[num-1].x_trail_5;
        _ci[num-1].x_trail_5 = _ci[num-1].x_trail_4;
        _ci[num-1].x_trail_4 = _ci[num-1].x_trail_3;
        _ci[num-1].x_trail_3 = _ci[num-1].x_trail_2;
        _ci[num-1].x_trail_2 = _ci[num-1].x_trail_1;
        _ci[num-1].x_trail_1 = _ci[num-1].x;
        _ci[num-1].x = x;
        
        // update y history
        _ci[num-1].y_trail_15 = _ci[num-1].y_trail_14;
        _ci[num-1].y_trail_14 = _ci[num-1].y_trail_13;
        _ci[num-1].y_trail_13 = _ci[num-1].y_trail_12;
        _ci[num-1].y_trail_12 = _ci[num-1].y_trail_11;
        _ci[num-1].y_trail_11 = _ci[num-1].y_trail_10;
        _ci[num-1].y_trail_10 = _ci[num-1].y_trail_9;
        _ci[num-1].y_trail_9 = _ci[num-1].y_trail_8;
        _ci[num-1].y_trail_8 = _ci[num-1].y_trail_7;
        _ci[num-1].y_trail_7 = _ci[num-1].y_trail_6;
        _ci[num-1].y_trail_6 = _ci[num-1].y_trail_5;
        _ci[num-1].y_trail_5 = _ci[num-1].y_trail_4;
        _ci[num-1].y_trail_4 = _ci[num-1].y_trail_3;
        _ci[num-1].y_trail_3 = _ci[num-1].y_trail_2;
        _ci[num-1].y_trail_2 = _ci[num-1].y_trail_1;
        _ci[num-1].y_trail_1 = _ci[num-1].y;
        _ci[num-1].y = y;

    }
    else if (mode == iMode_touchReleased)
    {
        _ci[num-1].mode = iMode_touchReleased;
    }
    
    // send data to game world
    gameWorld.setControlInput(_ci[num-1]);
}


- (void) dealloc 
{
    //SoundEngine_Teardown();	
    
	for(unsigned i = 0; i < NUM_TEXTURES; ++i)
    {
		if (_textures[i]) [_textures[i] release];	
	}
    
	[super dealloc];
}


- (void) updateSound:(float)dTime
{
	BOOL				playMoveSound = NO;
    
    // start or stop move sound & update its position
    if (playMoveSound && !_playingMoveSound)
    {
        //SoundEngine_StartEffect( _sounds[kSound_Thrust]);
    }
    else if (!playMoveSound && _playingMoveSound)
    {
        //SoundEngine_StopEffect(_sounds[kSound_Thrust], false);
    }
    
    if (playMoveSound)
    {
        //float distance = (2.0 * (160/320)) - 1.0;
        //SoundEngine_SetEffectPosition(_sounds[kSound_Thrust], distance, 0.0, 0.0);
    }
    _playingMoveSound = playMoveSound;
    
}    

@end
