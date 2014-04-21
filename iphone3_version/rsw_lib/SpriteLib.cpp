/* 
 * Simple Sprite lib by Theoxylo, copyright 2008
 */

#include "SpriteLib.h"
#include <stdio.h>

Sprite::Sprite() : id(0), size(0), frames(0), visible(0), frame(0), rotation(0), posX(0), posY(0)
{
    printf("Sprite() called\n");
}

void Sprite::initSprite(const int spriteId, const int sizePx, const int frameCount)
{
    printf("Sprite::initSprite(spriteId: %d, sizePx: %d, frameCount: %d\n", spriteId, sizePx, frameCount);
    
    id = spriteId;
    size = sizePx;
    frames = frameCount;
}

void Sprite::initSprite(Sprite* other)
{
	initSprite(other->id, other->size, other->frames);
}
