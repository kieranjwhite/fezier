#ifndef TILE_H
#define TILE_H
#include "draw.h"

uint32 tile_render(uint32 *p_strokes, uint32 ord, uint32 left, uint32 top, uint32 w, uint32 h, float32 reduction);
uint32 tile_size();
void tile_reset();

#endif
