/* Copyright 2015 Kieran White.
   This file is part of fezier.

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

#include <stdio.h>
#include <SDL.h>

#include "../../types.h"

void image_buf_commit(SDL_Surface *p_surface, uint32 *p_data, uint32 width, uint32 height) {
  //p_data is assumed to be in RGBA format

  SDL_PixelFormat *p_pf=p_surface->format;

  uint32 col=0,row=0;
  uint32 pitch=p_surface->pitch;
  uint32 pitch_pixels=pitch>>2;
  uint32 *p_start_dest=(uint32 *)(p_surface->pixels);
  uint32 *p_dest=p_start_dest;
  for(uint32 r=0; r<height; r++) {
    for(uint32 c=0; c<width; c++) {
      if(c>=pitch_pixels) {
	printf("image_buf_commit. break at %u \n", c);
	break;
      }
      uint32 col= *(p_data+(r*width)+c);
      *(p_dest++)=SDL_MapRGBA(p_pf, 
			      (col >> 0x10) & 0xff, 
			      (col >> 0x08) & 0xff, 
			      (col >> 0x00) & 0xff, 
			      (col >> 0x18 ) & 0xff);
      //p_dest++;
    }
    p_dest=(uint32 *)(((uint8 *)p_start_dest)+r*pitch);
  }
}
