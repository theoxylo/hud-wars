#ifndef CONTROL_INPUT_H
#define CONTROL_INPUT_H


static const bool DEBUG_TOUCH = false;
static const bool DEBUG_CONTROL = false;


typedef enum 
{
    iMode_none = -1,
    iMode_touchDown = 0,
    iMode_touchMoved,
    iMode_touchReleased,
    iMode_tap,
    iMode_numModes
    
} InputMode;

typedef struct
{
    int touchNumber; // touch 1 or touch 2?
    InputMode mode;
    
    int timestamp;
    
    int updateCount;
    
    bool isNew;
        
    int startX;
    int startY;
    
    bool hasMoved;

    int x; // most recent
    int x_trail_1;
    int x_trail_2;
    int x_trail_3;
    int x_trail_4;
    int x_trail_5;
    int x_trail_6;
    int x_trail_7;
    int x_trail_8;
    int x_trail_9;
    int x_trail_10;
    int x_trail_11;
    int x_trail_12;
    int x_trail_13;
    int x_trail_14;
    int x_trail_15;
    
    int y; // most recent
    int y_trail_1;
    int y_trail_2;
    int y_trail_3;
    int y_trail_4;
    int y_trail_5;
    int y_trail_6;
    int y_trail_7;
    int y_trail_8;
    int y_trail_9;
    int y_trail_10;
    int y_trail_11;
    int y_trail_12;
    int y_trail_13;
    int y_trail_14;
    int y_trail_15;
    
} ControlInput;


#endif

