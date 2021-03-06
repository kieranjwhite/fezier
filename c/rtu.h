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

#ifndef RTU_H
#define RTU_H

#include <math.h>
#include <stdbool.h>
#include <string.h>
#include "types.h"

//#define RTU_FAST_ATAN

//#ifdef EMSCRIPTEN
#if 0
#define M_E 2.71828182845904523536
#define M_LOG2E 1.44269504088896340736
#define M_LOG10E 0.434294481903251827651
#define M_LN2 0.693147180559945309417
#define M_LN10 2.30258509299404568402
#define M_PI 3.14159265358979323846
#define M_PI_2 1.57079632679489661923
#define M_PI_4 0.785398163397448309616
#define M_1_PI 0.318309886183790671538
#define M_2_PI 0.636619772367581343076
#define M_1_SQRTPI 0.564189583547756286948
#define M_2_SQRTPI 1.12837916709551257390
#define M_SQRT2 1.41421356237309504880
#define M_SQRT_2 0.707106781186547524401
#endif

#define TWO_PI (2*M_PI)
#define PI_OVER_SIX (5.235987755982988e-01)
#define PI_OVER_EIGHT (3.926990816987241e-01)
#define PI_OVER_2000 (1.570796326794897e-03)

#define MAX(X,Y) ((X)>(Y)?(X):(Y))
#define MIN(X,Y) ((X)>(Y)?(Y):(X))

inline uint32 rtu_signBit(sint32 v) {
  return v>>((sizeof(sint32)<<3)-1);
}

inline uint32 rtu_abs(sint32 v) {
  //see http://graphics.stanford.edu/~seander/bithacks.html#IntegerAbs for origin
  uint32 mask=rtu_signBit(v);
  return (v+mask) ^ mask;
}

//#define ABSF(X) ((X)<0?-(X):(X))
#define ABSF(X) ((((X)>=0)*(X))-(!((X)>=0))*(X))
#define SGN(X) (((X) > 0) - ((X) < 0))
#define RANGE(X,MIN,MAX_PLUS) ((X>=MIN && X<MAX_PLUS?0:(X<MIN?-1:+1)))
#define RANGE_INCL(X,MIN,MAX) ((X>=MIN && X<=MAX?0:(X<MIN?-1:+1)))
#define CMP(X,Y) (((X) > (Y)) - ((X) < (Y)))
#define DIM(X) (sizeof(X)/sizeof(X[0]))
#define IMPLIES(X,Y) ((!(X)) || ((X) && (Y)))

//output of next macros is binary, either -1 or +1
#define BSGN(X) (((X) >= 0) - ((X) < 0))
#define BCMP(X,Y) (((X) >= (Y)) - ((X) < (Y)))

#define RTU_APPNAME "com.hourglassapps.tiles"

#define LOG_INFO_LEVEL 0
#define LOG_ASSERT_LEVEL 1
#define LOG_ERROR_LEVEL 2
#define LOG_NO_REPORTING_LEVEL 3

#define LOG_LEVEL LOG_ERROR_LEVEL

typedef struct {
  uint32 divisor_limit;
  float32 *p_div_table;
  float32 *p_atan_table;
  uint32 atan_divisor;
} rtu_globals;

rtu_globals rtu_globalsInitialFields(void);
rtu_globals* rtu_globalsInit(void);
void rtu_globalsDestroy(rtu_globals *p_globals);
void rtu_globalsLoadR(rtu_globals *p_globals);
void rtu_globalsSave(rtu_globals *p_globals);

void rtu_log(const sint8 * p_level, const sint8 * p_file, uint32 line, const sint8* p_format, ... );
void rtu_solveQuadratic(float32 a, float32 b, float32 c, float32 *p_a1, float32 *p_a2);

bool rtu_isAngAcuteBetween(float32 a1, float32 a2, float32 aInner);
bool rtu_isMAcuteBetween(float32 m1, float32 m2, float32 mInner);

bool rtu_onFail(void);
void rtu_initFastATan(const uint32 divisor, rtu_globals *p_globals);
void rtu_destroyATan(rtu_globals *p_globals);
void rtu_initFastDiv(uint32 end_plus_one, rtu_globals *p_globals);
void rtu_destroyDiv(rtu_globals *p_globals);

#define REPORT(...) rtu_log("MESSAGE ", __FILE__, __LINE__,  __VA_ARGS__);

#if LOG_LEVEL <= LOG_ERROR_LEVEL

#define DO_ERROR(EXPR) EXPR
#define LOG_ERROR(...) rtu_log("ERROR ", __FILE__, __LINE__, __VA_ARGS__);

#if LOG_LEVEL <= LOG_ASSERT_LEVEL

#define DO_ASSERT(...) __VA_ARGS__
#define LOG_ASSERT(CONDITION, ...) if(!(CONDITION) && rtu_onFail()) { rtu_log("FAILED ASSERT ", __FILE__, __LINE__, __VA_ARGS__); }

#if LOG_LEVEL <= LOG_INFO_LEVEL

#define DO_INFO(...) __VA_ARGS__
#define LOG_INFO(...) rtu_log("", __FILE__, __LINE__, __VA_ARGS__);

#else

#define DO_INFO(...)
#define LOG_INFO(...)

#endif

#else

#define DO_INFO(...)
#define LOG_INFO(...)

#define DO_ASSERT(...)
#define LOG_ASSERT(...)

#endif

#else

#define DO_INFO(...)
#define LOG_INFO(...)

#define DO_ASSERT(...)
#define LOG_ASSERT(...)

#define LOG_ERROR(...)
#define DO_ERROR(...)

#endif

inline void rtu_memSet(void *p, const uint32 v, const uint32 num_words32) {
  /*
    Replacement for memset but instead of filling mem one bytes at a
    time, it does it one 32-bit word at a time.

    Note the final arg is NOT the number of bytes, rather it's the
    number of 32-bit words to be filled.
  */
  
  uint32 *p_end=((uint32 *)p)+num_words32;
  uint32 *p_cur;
  for(p_cur=(uint32 *)p; p_cur<p_end; p_cur++) {
    *p_cur=v;
  }
}

inline uint32 rtu_min(uint32 x, uint32 y) {
  //from http://www.geeksforgeeks.org/compute-the-minimum-or-maximum-max-of-two-integers-without-branching/
  uint32 res=y ^ ((x ^ y) & -(x < y));
  LOG_ASSERT(res<=x && res<=y, "rtu_min failure: %u %u",x ,y);
  return res;
}

inline uint32 rtu_max(uint32 x, uint32 y) {
  //from http://www.geeksforgeeks.org/compute-the-minimum-or-maximum-max-of-two-integers-without-branching/
  uint32 res=x ^ ((x ^ y) & -(x < y));
  LOG_ASSERT(res>=x && res>=y, "rtu_max failure: %u %u",x ,y);
  return res;
}

inline uint32 rtu_bound(uint32 min, uint32 x, uint32 max) {
  LOG_ASSERT(min<=max, "broken precondition: %u %u", min, max);
  return rtu_max(min, rtu_min(x, max));
}

inline void rtu_memZero(void *p_mem, uint32 size) {
  LOG_ASSERT(p_mem!=0, "attempt to zero null pointer. size: %u", size);
  memset(p_mem, 0, size);
}

inline bool rtu_similarToZero(float32 fst, float32 fuzz) {
  return ABSF(fst)<fuzz;
}

inline bool rtu_64similarToZero64(float64 fst, float64 fuzz) {
  return ABSF(fst)<fuzz;
}

inline bool rtu_similar(float32 fst, float32 snd, float32 fuzz) {
  return ABSF(fst-snd)<fuzz;
}

inline uint32 rtu_factorial(uint32 n) {
  LOG_ASSERT(n<=12 && sizeof(uint32)>=4, "factorial will be out of range for %u", n);
  uint32 fact=1;
  for(uint32 idx=2; idx<=n; idx++) {
    fact*=idx;
  }
  return fact;
}

/*
extern rtu_globals G_rtu;
extern rtu_globals *G_p_rtu;
*/

inline float32 rtu_fastDiv(float32 dividend, uint32 divisor, const rtu_globals *p_globals) {
  LOG_ASSERT(p_globals->p_div_table && divisor!=0 && divisor<p_globals->divisor_limit, "largest divisor is %u", p_globals->divisor_limit);
  return dividend*p_globals->p_div_table[divisor];
}

inline float32 rtu_fastATan(const float32 slope, const rtu_globals *p_globals) {
#ifdef RTU_FAST_ATAN
  /* Assumes divisor is p_globals->atan_divisor, slope_dividen>=0.
   * Returns values between -pi and pi inclusive
   */

  float32 multiplier=-((((sint32)(slope<0))<<1)-1); //multiplier=slope<0?-1:1
  float32 pos_slope=slope*multiplier;
  float32 pos_tan;

  if(pos_slope>1) {
    uint32 atan_idx=ROUND32(p_globals->atan_divisor/pos_slope);
    LOG_ASSERT(atan_idx<=p_globals->atan_divisor, "out of range idx for slope %f, divisor %f", pos_slope, p_globals->atan_divisor);
    pos_tan=M_PI_2-p_globals->p_atan_table[atan_idx];
  } else {
    uint32 atan_idx=ROUND32(pos_slope*p_globals->atan_divisor);    
    LOG_ASSERT(atan_idx<=p_globals->atan_divisor, "out of range idx for slope (of <= 1) %f, divisor %f", pos_slope, p_globals->atan_divisor);
    pos_tan=p_globals->p_atan_table[atan_idx];
  }
  
  return pos_tan*multiplier;
#else
  return ATAN32(slope);
#endif
}

inline float32 rtu_div(float32 dividend, uint32 divisor, const rtu_globals *p_globals) {
	if(divisor<p_globals->divisor_limit) {
	  return rtu_fastDiv(dividend, divisor, p_globals);
	}
	LOG_INFO("slow division: %f / %u", dividend, divisor);
	return dividend/divisor;
}

inline uint32 rtu_nCr(uint32 n, uint32 r, const rtu_globals *p_globals) {
  LOG_ASSERT(n>=r, "invalid nCr args: %u, %u", n, r);
  float32 ncr=rtu_div(rtu_factorial(n), (rtu_factorial(r)*rtu_factorial(n-r)), p_globals);
  LOG_ASSERT(ncr==(uint32)ncr, "non integral result from ncr: %u nCr %u=%f", n, r, ncr);
  return (uint32)ncr;
}

inline float32 rtu_fractional(float32 num) {
  LOG_ASSERT(num>=0, "only works for +ve numbers: %f", num);
  uint32 num_int=num;
  return num-num_int;
}

inline float32 rtu_altAng(float32 ang) {
  LOG_ASSERT(ang>=-M_PI_2 && ang<=M_PI_2, "out of range ang %f", ang);
  float32 res=(FMOD32((ang+M_PI), TWO_PI))-M_PI;
  if(res>=M_PI) {
    res-=TWO_PI;
  }
  LOG_ASSERT(res>=-M_PI && res<M_PI, "out of range ang %f", ang);
  return res;
}

static inline float32 rtu_lineC(float32 x, float32 y, float32 m) {
  return y-m*x;
}

void *rtu_memAlloc(uint32 bytes);
void rtu_memFree(void *p_mem);
sint8 *rtu_test(void);

#endif
