/* 
 * Simple game object framework by Theoxylo, copyright 2008
 */

#ifndef ANIMATION_H
#define ANIMATION_H

#include <stdio.h>

class Animation
{
public:

    Animation();
    void reset();
    void start();
    bool update();
    int getFrame(); // maps to sprite image frame for display

    // current state
    int _frameIndex; // currently displayed animation frame
    int _frameTime;  // steps since start of current frame

    // animation data; TODO: load from script file
    int _numFrames;
    void setFrameCount(int numFrames);
    bool _repeat;
    static const int MAX_FRAMES = 16;
    int _displayFrames[MAX_FRAMES];
    int _displayTimes[MAX_FRAMES];
};

#endif
