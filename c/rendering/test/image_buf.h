/* Copyright 2015 Kieran White.
   This file is part of Fezier.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef IMAGE_BUF_H
#define IMAGE_BUF_H

#include "../draw.h"
#include "../../types.h"
#ifndef NDK
#include <SDL.h>
#endif

#define IMAGE_BUF_SCREEN_WIDTH 1600
#define IMAGE_BUF_SCREEN_HEIGHT 2195
//#define IMAGE_BUF_SCREEN_WIDTH 35
//#define IMAGE_BUF_SCREEN_HEIGHT 30
#define IMAGE_BUF_PIX_RATIO -1

#ifndef NDK
void image_buf_commit(SDL_Surface *p_surface, draw_canvas *p_canvas, uint32 mag);
#endif

#endif
