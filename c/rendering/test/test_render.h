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

#include <SDL.h>

//define FULL_RENDER in order to render image. Don't define FULL_RENDER when profiling
#define FULL_RENDER

#ifdef FULL_RENDER
#define ITERATIONS 1
#else
#define ITERATIONS 50
#endif

#define TEST_H
void render(SDL_Surface *p_screen, SDL_Surface *p_surface, draw_globals *p_globals);
#endif
