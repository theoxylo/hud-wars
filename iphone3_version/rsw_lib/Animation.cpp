#include "Animation.h"

Animation::Animation()
{
    _numFrames = 0;
    _repeat = true;
 
    reset();
}
void Animation::reset()
{
    _frameIndex = MAX_FRAMES + 1; // this is the "stopped" state
    _frameTime = 0;
}

void Animation::start()
{
    //printf("Animation::start() called\n");
    _frameIndex = 0;
    _frameTime = 0;
}

void Animation::setFrameCount(int frames)
{
    _numFrames = frames < MAX_FRAMES ? frames : MAX_FRAMES;
}

bool Animation::update()
{
    // short circuit if done playing
    if (_frameIndex >= _numFrames) return false;
    
    _frameTime++;
    while (_frameTime > _displayTimes[_frameIndex])
    {
        _frameTime -= _displayTimes[_frameIndex];
        _frameIndex++;

        if (_frameIndex >= _numFrames)
        {
            // reset frame index for repeat
            if (_repeat) _frameIndex = 0;
            else return false; // all done      
        }
    }
    return true; // still playing
}

int Animation::getFrame()
{
    // will return 0 if finished or not in use
    return _frameIndex < _numFrames ? _displayFrames[_frameIndex] : 0;
}
