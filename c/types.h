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

#ifndef types_h
#define types_h

typedef unsigned char uint8;
typedef char sint8;
typedef short int int16;
typedef unsigned short int uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef int sint32;
typedef float float32;
typedef double float64;

typedef enum {ok, noMemory,invalidPrimitive,emptyObject,nofile, unsupported, glErr, failedAssert} oError;

#define CEIL32(X) ceilf(X)
#define ROUND32(X) roundf(X)
#define LOG32(X) logf(X)
#define POW32(X,Y) powf(X,Y)
#define SQRT32(X) sqrtf(X)
#define FMOD32(X,Y) fmodf(X,Y)
#define COS32(X) cosf(X)
#define SIN32(X) sinf(X)
#define TAN32(X) tanf(X)
#define ATAN32(X) atanf(X)

#define CEIL64(X) ceil(X)
#define ROUND64(X) round(X)
#define LOG64(X) log(X)
#define POW64(X,Y) pow(X,Y)
#define SQRT64(X) sqrt(X)
#define FMOD64(X,Y) fmod(X,Y)
#define COS64(X) cos(X)
#define SIN64(X) sin(X)
#define TAN64(X) tan(X)
#define ATAN64(X) atan(X)

#endif
