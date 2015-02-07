# fezier
A small, simple C library for rendering feathered Bézier curves.

# Copyright
Copyright 2015 Kieran White. This file is part of fezier.

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
frezier does have a test harness (in c/rendering/test/main.c) and this
does require SDL 1.2.

# Instructions for use
Follow the example in the test harness, specifically the render
function in the file c/rendering/test/test_render.c

# Compiling and running the test harness
<pre>
cd c/rendering/test
gcc -DHAVE_TYPEOF -DHAVE_BUILTIN_CHOOSE_EXPR -DHAVE_BUILTIN_TYPES_COMPATIBLE_P -gdwarf-2 -g3 -std=c99 -Wstrict-prototypes `pkg-config --cflags --libs sdl` test_render.c main.c image_buf.c ../draw.c ../../rtu.c -lm
./a.out
</pre>

# Contact details
E-mail me at kieran.white@hourglassapps.com