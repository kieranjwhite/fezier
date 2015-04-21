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
#include "../draw.h"

void image_buf_commit(SDL_Surface *p_surface, draw_canvas *p_canvas, uint32 mag) {
  /* This function is unoptimised in that is makes no attempt to
   * confine updates to dirty regions of the p_data array. The
   * function is only intended for testing. In an end-product
   * platform-native function would usually be used to blit the pixel
   * array p_data to a surface or canvas.
   */
  
  //p_data is assumed to be in RGBA format

  uint32 fullWidth=draw_canvasRenderedWidth(p_canvas);
  uint32 fullHeight=draw_canvasRenderedHeight(p_canvas);
  draw_rectInt *p_dirty=draw_canvasExtantDirty(p_canvas);
  uint32 left=p_dirty->lt.x;
  uint32 top=p_dirty->lt.y;
  uint32 right=p_dirty->rb.x;
  uint32 bottom=p_dirty->rb.y;
  uint32 *p_data=p_canvas->p_bitmap;
  SDL_PixelFormat *p_pf=p_surface->format;

  //uint32 width=1+(right-left);
  //uint32 height=1+(bottom-top);
  uint32 col=0,row=0;
  uint32 pitch=p_surface->pitch;
  uint32 pitch_pixels=pitch>>2;
  uint32 *p_start_dest=(uint32 *)(p_surface->pixels);
  uint32 *p_start_row_dest;
  uint32 *p_dest=p_start_dest;
  for(uint32 r=top; r<bottom; r++) {
    for(uint32 c=left; c<right; c++) {
      if(c>=pitch_pixels) {
	printf("image_buf_commit. break at %u \n", c);
	break;
      }
      uint32 col= *(p_data+(r*fullWidth)+c);
      //*(p_dest++)=SDL_MapRGBA(p_pf, 
      uint32 renderedCol=SDL_MapRGBA(p_pf, 
				     (col >> 0x10) & 0xff, 
				     (col >> 0x08) & 0xff, 
				     (col >> 0x00) & 0xff, 
				     (col >> 0x18 ) & 0xff);
      p_start_row_dest=p_start_dest+(r*mag*fullWidth)+c*mag;
      //*p_dest=renderedCol;
      
      for(uint32 destYOffset=0; destYOffset<mag; destYOffset++) {
	  if(destYOffset+r*mag>fullHeight) {
	    break;
	  }
	p_dest=p_start_row_dest;
	for(uint32 destXOffset=0; destXOffset<mag; destXOffset++) {
	  if(destXOffset+c*mag>fullWidth) {
	    break;
	  }
	  *p_dest=renderedCol;
	  p_dest++;
	}
	p_start_row_dest+=fullWidth;
      }
      
    }
    //p_dest=(uint32 *)(((uint8 *)p_start_dest)+r*pitch);
  }

  draw_canvasResetDirty(p_canvas);
}
