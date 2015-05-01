# Fezier
A small, simple C library for rendering feathered Bézier curves.

# Copyright
Copyright 2015 Kieran White. This file is part of Fezier.

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

This library relies to two other libraries that are distributed
according to the terms specified by other licences:

minunit. This library comprises the file c/minunit.h, which was
modified by me (Kieran White) but was originally developed by another
author. This file is distributed here under the terms of the GNU
General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later
version. c/minunit.h is also distributed under the terms of the original
minunit licence. The terms of this licence are described in point two
of the header comments of c/minunit.h.

typesafe_cb. This library comprises the files in the
c/ccan/typesafe_cb directory. I have not modified these files and they
are distributed under the CC0 licence. See the file
c/ccan/typesafe_cb/LICENSE for details.

# Dependencies
This library can be included in your own programs
without requiring any other separately distributed libraries. However
Fezier does have a test harness (in c/rendering/test/main.c) and this
does require SDL 1.2.

# Instructions for use

<h3>Initialising Fezier</h3>
<pre>
//Globally declared struct
draw_globals *G_p_globals;

//Include these lines in your initialisation code
G_p_globals=draw_globalsInit();
rtu_initFastATan(DRAW_ATAN_DIVISORS, G_p_globals->p_rtu);
rtu_initFastDiv(DRAW_DIV_LIMIT, G_p_globals->p_rtu);

uint32 w=640;
uint32 h=480;

/* devicePixelRatio indicates the display's pixel density, larger
 * values indicate a larger DPI. This is used to decide on the optimum
 * rendering quality required for the current display.
 * A value of -1 disables this optimisation and ensures
 * that the scene is rendered at full resolution.
 * 1 is a good value for 150dpi devices, 2 for retina-type displays.
 * Other positive floating point values can also be provided.
 */
float32 devicePixelRatio=-1;
uint32 *p_pixels=rtu_memAlloc(w*h);
if(!p_pixels) {
   printf("malloc failure");
   exit(-1);
}
draw_init(w, h, devicePixelRatio, p_pixels, G_p_globals);
</pre>

<h3>Initialise a brush</h3>
<pre>
draw_brush brush;

/* colour is a uint32 in 0xaarrggbb format.
 *
 * Total width of the brush is breadth+(feather_width*0.5) since half the feathering
 * is inside the specified breadth, and half is outside the specified breadth.
draw_brushInit(&brush, colour, breadth, feather_width, G_p_globals);
</pre>

<h3>Initialise a stroke</h3>
<pre>
draw_stroke stroke;
//The same brush can be used across multiple strokes
draw_strokeInit(&stroke, &brush, G_p_globals);
//Currently a stroke doesn't need to be destroyed when you're finished with it.
</pre>

<h3>Draw a curve</h3>
<pre>
draw_vert start={ .x=100, .y=100 };
draw_strokeMoveTO(&stroke, &start, G_p_globals);

//draw_strokeQuadTo can be invoked more than once with different coords
draw_vert ctrl={ .x=150, .y=150 };
draw_vert end={ .x=200, .y=200 };
draw_strokeQuadTo(&stroke, &ctrl, &end, G_p_globals);

//draw the end cap
draw_strokeRender(&stroke, G_p_globals);
</pre>

Next you will need to composite G_p_globals->canvas.p_bitmap to your
canvas / surface. The details on how to do this vary depending on
which library you're using. An example of how do this in SDL can be
found in the test harness. The render function in the file
c/rendering/test/test_render.c contains the required code.

A dirty rectangle is returned by
draw_canvasDirty(&G_p_globals->canvas). If Fezier was initialised with
a devicePixelRatio of -1, the returned dirty rectangle specifies the
region within G_p_globals->canvas.p_bitmap where pixels have been
altered and their destination rectangle on your canvas / surface. The
dirty rectangle coordinates are inclusive.

If Fezier was initialised with a devicePixelRatio > 0, then the
destination coords on your canvas / surface must be calculated by
multiplying the the x,y coords in the dirty rectangle by the value
returned by draw_brushMagFactor(&brush) and the source pixels (still
specified by the dirty rectangle) must be scaled by the same value
during compositing.

The dirty recangle returned by the draw_canvasDirty function is reset
at the start of every draw_strokeMoveTo function call. To allow for
greater control there is also the extant dirty rectangle which is
returned by a call to draw_canvasExtantDirty. This dirty rectangle is
reset with a call to draw_strokeMoveTO (as before) but also by
invoking the following code anytime as desired:

<pre>
draw_canvasResetDirty(&G_p_globals->canvas);
</pre>

<h3>Finalising a brush</h3>
<pre>
draw_brushDestroy(&brush); //frees memory allocated when brush was initialised
</pre>

<h3>Finalising Fezier</h3>
<pre>
//Include these lines in your clean-up code at the end
rtu_memFree(G_p_globals->canvas.p_bitmap); //frees p_pixels array above
rtu_destroyDiv(G_p_globals->p_rtu);
rtu_destroyATan(G_p_globals->p_rtu);
draw_globalsDestroy(G_p_globals);
</pre>

# Compiling and running the test harness
<pre>
cd c/rendering/test
gcc -DHAVE_TYPEOF -DHAVE_BUILTIN_CHOOSE_EXPR -DHAVE_BUILTIN_TYPES_COMPATIBLE_P -gdwarf-2 -g3 -std=c99 -Wstrict-prototypes `pkg-config --cflags --libs sdl` test_render.c main.c image_buf.c ../draw.c ../../rtu.c -lm
./a.out
</pre>

# Contact details
E-mail me at kieran.white@hourglassapps.com