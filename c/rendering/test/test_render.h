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

#ifndef TEST_H
#define TEST_H

#include <SDL.h>

//define FULL_RENDER in order to render image. Don't define FULL_RENDER when profiling
#define FULL_RENDER

#ifdef FULL_RENDER
#define ITERATIONS 1
#else
#define ITERATIONS 50
#endif

#define X_OFF (0)
//#define X_OFF (-1500)
#define Y_OFF (0)
//#define Y_OFF (-2500)

#ifdef NDK
#define MAG 1.0
#else
#define MAG (2.0)
//#define MAG (1.6236305)
//#define MAG 3.5
#endif

void render(SDL_Surface *p_screen, SDL_Surface *p_surface, draw_globals *p_globals);
#endif
