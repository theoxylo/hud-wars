/* 
 * Simple game object framework by Theoxylo, copyright 2008
 */

#ifndef MAP_BLOCK_H
#define MAP_BLOCK_H

class MapBlock
{
public:

    // position of upper left corner and width, height
    int x, y, w, h;

    MapBlock(): x(0), y(0), w(0), h(0) {}

    MapBlock(int ix, int iy, int iw, int ih): x(ix), y(iy), w(iw), h(ih) {}
};

#endif
