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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <SDL.h>

#include "../../rtu.h"
#include "../../types.h"
#include "../draw.h"
#include "test_render.h"

uint32 tests_run=0;

//----------------------------------------------------------

// A set of very useful macros that you will find in most
// code that I write whether I use them in a program or
// not.

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define abs(a) (((a)<0) ? -(a) : (a))
#define sign(a) (((a)<0) ? -1 : (a)>0 ? 1 : 0)

//----------------------------------------------------------

// The following code implements a Bresenham line drawing
// algorithm. There are 4 separate routines each optimized
// for one of the four pixel depths supported by SDL. SDL
// support many pixel formats, but it only support 8, 16,
// 24, and 32 bit pixels.

//----------------------------------------------------------

// Draw lines in 8 bit surfaces.

static void line8(SDL_Surface *s, 
                  sint32 x1, sint32 y1, 
                  sint32 x2, sint32 y2, 
                  uint32 color)
{
  sint32 d;
  sint32 x;
  sint32 y;
  sint32 ax;
  sint32 ay;
  sint32 sx;
  sint32 sy;
  sint32 dx;
  sint32 dy;

  uint8 *lineAddr;
  sint32 yOffset;

  dx = x2 - x1;  
  ax = abs(dx) << 1;  
  sx = sign(dx);

  dy = y2 - y1;  
  ay = abs(dy) << 1;  
  sy = sign(dy);
  yOffset = sy * s->pitch;

  x = x1;
  y = y1;

  lineAddr = ((uint8 *)(s->pixels)) + (y * s->pitch);
  if (ax>ay)
    {                      /* x dominant */
      d = ay - (ax >> 1);
      for (;;)
	{
	  *(lineAddr + x) = (uint8)color;

	  if (x == x2)
	    {
	      return;
	    }
	  if (d>=0)
	    {
	      y += sy;
	      lineAddr += yOffset;
	      d -= ax;
	    }
	  x += sx;
	  d += ay;
	}
    }
  else
    {                      /* y dominant */
      d = ax - (ay >> 1);
      for (;;)
	{
	  *(lineAddr + x) = (uint8)color;

	  if (y == y2)
	    {
	      return;
	    }
	  if (d>=0) 
	    {
	      x += sx;
	      d -= ay;
	    }
	  y += sy;
	  lineAddr += yOffset;
	  d += ax;
	}
    }
}

//----------------------------------------------------------

// Draw lines in 16 bit surfaces. Note that this code will
// also work on 15 bit surfaces.

static void line16(SDL_Surface *s, 
                   sint32 x1, sint32 y1, 
                   sint32 x2, sint32 y2, 
                   uint32 color)
{
  sint32 d;
  sint32 x;
  sint32 y;
  sint32 ax;
  sint32 ay;
  sint32 sx;
  sint32 sy;
  sint32 dx;
  sint32 dy;

  uint8 *lineAddr;
  sint32 yOffset;

  dx = x2 - x1;  
  ax = abs(dx) << 1;  
  sx = sign(dx);

  dy = y2 - y1;  
  ay = abs(dy) << 1;  
  sy = sign(dy);
  yOffset = sy * s->pitch;

  x = x1;
  y = y1;

  lineAddr = ((uint8 *)s->pixels) + (y * s->pitch);
  if (ax>ay)
    {                      /* x dominant */
      d = ay - (ax >> 1);
      for (;;)
	{
	  *((uint16 *)(lineAddr + (x << 1))) = (uint16)color;

	  if (x == x2)
	    {
	      return;
	    }
	  if (d>=0)
	    {
	      y += sy;
	      lineAddr += yOffset;
	      d -= ax;
	    }
	  x += sx;
	  d += ay;
	}
    }
  else
    {                      /* y dominant */
      d = ax - (ay >> 1);
      for (;;)
	{
	  *((uint16 *)(lineAddr + (x << 1))) = (uint16)color;

	  if (y == y2)
	    {
	      return;
	    }
	  if (d>=0) 
	    {
	      x += sx;
	      d -= ay;
	    }
	  y += sy;
	  lineAddr += yOffset;
	  d += ax;
	}
    }
}

//----------------------------------------------------------

// Draw lines in 24 bit surfaces. 24 bit surfaces require
// special handling because the pixels don't fall on even
// address boundaries. Instead of being able to store a
// single byte, word, or long you have to store 3
// individual bytes. As a result 24 bit graphics is slower
// than the other pixel sizes.

static void line24(SDL_Surface *s, 
                   sint32 x1, sint32 y1, 
                   sint32 x2, sint32 y2, 
                   uint32 color)
{
  sint32 d;
  sint32 x;
  sint32 y;
  sint32 ax;
  sint32 ay;
  sint32 sx;
  sint32 sy;
  sint32 dx;
  sint32 dy;

  uint8 *lineAddr;
  sint32 yOffset;

#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
  color <<= 8;
#endif

  dx = x2 - x1;  
  ax = abs(dx) << 1;  
  sx = sign(dx);

  dy = y2 - y1;  
  ay = abs(dy) << 1;  
  sy = sign(dy);
  yOffset = sy * s->pitch;

  x = x1;
  y = y1;

  lineAddr = ((uint8 *)(s->pixels)) + (y * s->pitch);
  if (ax>ay)
    {                      /* x dominant */
      d = ay - (ax >> 1);
      for (;;)
	{
	  uint8 *p = (lineAddr + (x * 3));
	  memcpy(p, &color, 3);

	  if (x == x2)
	    {
	      return;
	    }
	  if (d>=0)
	    {
	      y += sy;
	      lineAddr += yOffset;
	      d -= ax;
	    }
	  x += sx;
	  d += ay;
	}
    }
  else
    {                      /* y dominant */
      d = ax - (ay >> 1);
      for (;;)
	{
	  uint8 *p = (lineAddr + (x * 3));
	  memcpy(p, &color, 3);

	  if (y == y2)
	    {
	      return;
	    }
	  if (d>=0) 
	    {
	      x += sx;
	      d -= ay;
	    }
	  y += sy;
	  lineAddr += yOffset;
	  d += ax;
	}
    }
}

//----------------------------------------------------------

// Draw lines in 32 bit surfaces. Note that this routine
// ignores alpha values. It writes them into the surface
// if they are included in the pixel, but does nothing
// else with them.

static void line32(SDL_Surface *s, 
                   sint32 x1, sint32 y1, 
                   sint32 x2, sint32 y2, 
                   uint32 color)
{
  sint32 d;
  sint32 x;
  sint32 y;
  sint32 ax;
  sint32 ay;
  sint32 sx;
  sint32 sy;
  sint32 dx;
  sint32 dy;

  uint8 *lineAddr;
  sint32 yOffset;

  dx = x2 - x1;  
  ax = abs(dx) << 1;  
  sx = sign(dx);

  dy = y2 - y1;  
  ay = abs(dy) << 1;  
  sy = sign(dy);
  yOffset = sy * s->pitch;

  x = x1;
  y = y1;

  lineAddr = ((uint8 *)(s->pixels)) + (y * s->pitch);
  if (ax>ay)
    {                      /* x dominant */
      d = ay - (ax >> 1);
      for (;;)
	{
	  *((uint32 *)(lineAddr + (x << 2))) = (uint32)color;

	  if (x == x2)
	    {
	      return;
	    }
	  if (d>=0)
	    {
	      y += sy;
	      lineAddr += yOffset;
	      d -= ax;
	    }
	  x += sx;
	  d += ay;
	}
    }
  else
    {                      /* y dominant */
      d = ax - (ay >> 1);
      for (;;)
	{
	  *((uint32 *)(lineAddr + (x << 2))) = (uint32)color;

	  if (y == y2)
	    {
	      return;
	    }
	  if (d>=0) 
	    {
	      x += sx;
	      d -= ay;
	    }
	  y += sy;
	  lineAddr += yOffset;
	  d += ax;
	}
    }
}

//----------------------------------------------------------

// Examine the depth of a surface and select a line
// drawing routine optimized for the bytes/pixel of the
// surface.

static void line(SDL_Surface *s, 
                 sint32 x1, sint32 y1, 
                 sint32 x2, sint32 y2, 
                 uint32 color)
{
  switch (s->format->BytesPerPixel)
    {
    case 1:
      line8(s, x1, y1, x2, y2, color);
      break;
    case 2:
      line16(s, x1, y1, x2, y2, color);
      break;
    case 3:
      line24(s, x1, y1, x2, y2, color);
      break;
    case 4:
      line32(s, x1, y1, x2, y2, color);
      break;
    }
}

#define DRAW_DIR_TEST DRAW_DIR_DL

sint32 main_unit_tests(void) {
  sint8 *result = draw_test();
  if(result != 0) {
    printf("%s\n", result);
  } else {
    printf("ALL DRAW TESTS PASSED\n");
  }
  printf("Tests run: %d\n", tests_run);
   
  return result != 0;
}

sint32 main(sint32 argc, char **argv)
{
  //gameTime gt;
  char *name = argv[0];

  SDL_Surface *screen = NULL;
  SDL_Event event;
  SDL_PixelFormat *pf = NULL;
  uint32 grey;
  uint32 red;
  uint32 green;
  uint32 blue;

  sint32 screenWidth = 800;
  sint32 screenHeight = 1280;

  bool done = false;

  // Try to initialize SDL. If it fails, then give up.

  if (-1 == SDL_Init(SDL_INIT_EVERYTHING))
    {
      printf("Can't initialize SDL\n");
      exit(1);
    }

  // Safety first. If the program exits in an unexpected
  // way the atexit() call should ensure that SDL will be
  // shut down properly and the screen returned to a
  // reasonable state.

  atexit(SDL_Quit);

  // Initialize the display. Here I'm asking for a screenWidth*screenHeight
  // window with any pixel format and any pixel depth. If
  // you uncomment SDL_FULLSCREEN you should get a 640x480
  // full screen display.

  screen = SDL_SetVideoMode(screenWidth, 
                            screenHeight, 
                            0, 
                            SDL_ANYFORMAT |
                            //SDL_FULLSCREEN |
                            SDL_SWSURFACE
			    );

  if (NULL == screen)
    {
      printf("Can't set video mode\n");
      exit(1);
    }

  // Grab the pixel format for the screen. SDL_MapRGB()
  // needs the pixel format to create pixels that are laid
  // out correctly for the screen.

  pf = screen->format;

  //Create the pixel values used in the program. Grey is
  //for clearing the background and the other three are
  //for line colors. Note that in SDL you specify color
  //intensities in the rang 0 to 255 (hex ff). That
  //doesn't mean that you always get 24 or 32 bits of
  //color. If the format doesn't support the full color
  //range, SDL scales it to the range that is correct for
  //the pixel format.

  grey = SDL_MapRGB(pf, 0xbf, 0xbf, 0xbf);

  // Set the window caption and the icon caption for the
  // program. In this case I'm just setting it to whatever
  // the name of the program happens to be.

  SDL_WM_SetCaption(name, name);

  SDL_FillRect(screen, NULL, grey);

  uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  LOG_INFO("big endian");
  rmask = 0xff000000;
  gmask = 0x00ff0000;
  bmask = 0x0000ff00;
  amask = 0x000000ff;
#else
  LOG_INFO("little endian");
  rmask = 0x000000ff;
  gmask = 0x0000ff00;
  bmask = 0x00ff0000;
  amask = 0xff000000;
#endif

  SDL_Surface *temp = SDL_CreateRGBSurface(
					   0, screenWidth, screenHeight, 32, rmask, gmask, bmask, amask
					   );
  SDL_Surface *surface = SDL_DisplayFormatAlpha( temp );
  SDL_FreeSurface( temp );

  draw_globals *p_globals=draw_globalsInit();
  main_unit_tests();
  render(screen, surface, p_globals, screenWidth, screenHeight);

  SDL_Flip(screen);

  while (!done)
    {

      // Loop while reading all pending event.

      while (!done && SDL_PollEvent(&event))
	{
	  switch (event.type)
	    {

	      // Here we are looking for two special keys. If we
	      // get an event telling us that the escape key has
	      // been pressed the program will quit. If we see
	      // the F1 key we either start or stop the
	      // animation by starting or stopping the clock.

	    case SDL_KEYDOWN:
	      switch(event.key.keysym.sym)
		{
		case SDLK_ESCAPE:
		  done = true;
		  break;

		default:
		  break;
		}
	      break;

	      // The SDL_QUIT event is generated when you click
	      // on the close button on a window. If we see that
	      // event we should exit the program. So, we do.

	    case SDL_QUIT:
	      done = true;
	      break;
	    }
	}

      // The call to SDL_Delay(10) forces the program to
      // pause for 10 milliseconds and has the effect of
      // limiting the frame rate to less than 100
      // frames/second. It also keeps the program from
      // hogging the CPU which seems to result in smoother
      // animation because the program isn't interrupted by
      // the operating system for long periods.

      SDL_Delay(10);
    
    }

  SDL_FreeSurface(surface);
  // When we get here, just clean up and quit. Yes, the
  // atexit() call makes this redundant. But, it doesn't
  // hurt and I'd rather be safe than sorry.
  SDL_Quit();

  draw_finalise(p_globals);
  rtu_finaliseDiv(p_globals->p_rtu);
  draw_globalsDestroy(p_globals);
}
