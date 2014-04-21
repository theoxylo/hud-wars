/* 
 * Simple Sprite lib by Theoxylo, copyright 2008
 */

#ifndef SPRITE_MGR_H
#define SPRITE_MGR_H

class Sprite
{
    
public:

    Sprite();
    
    void initSprite(const int spriteId, const int sizePix, const int frameCount);
    void initSprite(Sprite* other);

	~Sprite();

    int id;
    
    int size; // square size in pixels

    int frame; // current sprite animation frame
    
    int frames; // number of possible frames

    int rotation; // 360 degrees

    bool visible;

    int posX; // world position in pixels of LEFT edge
    int posY; // world position in pixels of TOP edge
};

#endif

