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

#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "minunit.h"
#include "rtu.h"
#include "types.h"

extern inline uint32 rtu_signBit(sint32 v);
extern inline uint32 rtu_abs(sint32 v);
extern inline uint32 rtu_min(uint32 x, uint32 y);
extern inline uint32 rtu_max(uint32 x, uint32 y);
extern inline uint32 rtu_bound(uint32 min, uint32 x, uint32 max);
extern inline float32 rtu_altAng(float32 ang);
extern inline void rtu_memZero(void *p_mem, uint32 size);
extern inline bool rtu_similarToZero(float32 fst, float32 fuzz);
extern inline bool rtu_64similarToZero64(float64 fst, float64 fuzz);
extern inline bool rtu_similar(float32 fst, float32 snd, float32 fuzz);
extern inline uint32 rtu_factorial(uint32 n);
extern float32 rtu_div(float32 dividend, uint32 divisor, const rtu_globals *p_globals);
extern inline uint32 rtu_nCr(uint32 n, uint32 r, const rtu_globals *p_globals);
extern inline float32 rtu_fastDiv(float32 dividend, uint32 divisor, const rtu_globals *p_globals);
extern inline float32 rtu_fastATan(const float32 slope, const rtu_globals *p_globals);
extern inline void rtu_memSet(void *p, const uint32 v, const uint32 num_words32);

/*
void rtu_globalsLoadR(rtu_globals *p_globals) {
	if(p_globals) {
		G_p_rtu=p_globals;
	} else {
		LOG_ASSERT(false, "null rtu_globals ptr");
	}
}
*/

rtu_globals* rtu_globalsInit(void) {
  rtu_globals *p_globals=rtu_memAlloc(sizeof(rtu_globals));
  if(p_globals) {
	  rtu_memZero(p_globals, sizeof(rtu_globals));
	  *p_globals=rtu_globalsInitialFields();
  } else {
    LOG_ASSERT(false, "rtu_globals not malloced");
  }
  return p_globals;
}

void rtu_globalsDestroy(rtu_globals *p_globals) {
  if(p_globals) {
    rtu_memFree(p_globals);
  } else {
    LOG_ASSERT(false, "rtu_globals wasn't malloced"); 
  }
}

bool rtu_onFail(void) {
  return true;
}

rtu_globals rtu_globalsInitialFields(void) {
  rtu_globals g={
    .divisor_limit=0,
    .p_div_table=NULL,
    .p_atan_table=NULL,
    .atan_divisor=0
  };
  return g;
}

/*
rtu_globals G_rtu;
rtu_globals *G_p_rtu=&G_rtu;
*/
void rtu_initFastDiv(uint32 end_plus_one, rtu_globals *p_globals) {
  if(p_globals->p_div_table) {
    LOG_ASSERT(false, "p_div_table already initialised");
  } else {
	LOG_INFO("initialising G_div_table");
    uint32 size=end_plus_one;
    p_globals->p_div_table=rtu_memAlloc(size*sizeof(float32));
    if(p_globals->p_div_table) {
      for(uint32 i=1; i<end_plus_one; i++) {
	p_globals->p_div_table[i]=1/(float32)i;
      }
      p_globals->divisor_limit=end_plus_one;
    } else {
      LOG_ASSERT(false, "failed to malloc %u", size*sizeof(float32));
    }
  }
}

//this function is actually slower than a normal atanf invocation
void rtu_initFastATan(const uint32 divisor, rtu_globals *p_globals) {
#ifdef RTU_FAST_ATAN
  /* The table will have divisor+1 elements
   */
  if(divisor==0) {
    LOG_ASSERT(false, "divisor cannot be 0");
    return;
  }
  if(p_globals->p_atan_table) {
    LOG_ASSERT(false, "p_atan_table already initialised");
  } else {
    uint32 bytes=(divisor+1)*sizeof(float32);
    p_globals->p_atan_table=rtu_memAlloc(bytes);
    if(p_globals->p_atan_table) {
      float32 slope_inc=((float32)1)/divisor;
      float32 slope=0;
      uint32 i=0;
      for(; i<divisor; i++) {
	p_globals->p_atan_table[i]=ATAN32(slope);
	slope+=slope_inc;
      }
      p_globals->p_atan_table[i]=ATAN32(1.0);
      p_globals->atan_divisor=divisor;
    } else {
      LOG_ASSERT(false, "failed to malloc %u", bytes);
    }
  }
#endif
}

void rtu_destroyDiv(rtu_globals *p_globals) {
  if(p_globals->p_div_table) {
    rtu_memFree(p_globals->p_div_table);
    p_globals->p_div_table=NULL;
  } else {
    LOG_ASSERT(false, "p_div_table not initialised");
  }
}

void rtu_destroyATan(rtu_globals *p_globals) {
#ifdef RTU_FAST_ATAN
  if(p_globals->p_atan_table) {
    rtu_memFree(p_globals->p_atan_table);
    p_globals->p_atan_table=NULL;
  } else {
    LOG_ASSERT(false, "p_atan_table not initialised");
  }
#endif
}

uint32 G_RTU_LITTLE_ENDIAN=-1;
int rtu_littleEndian(void) {
  //only works if sizeof(char)<sizeof(int)
  if(G_RTU_LITTLE_ENDIAN==-1) {
    int num=1;
    G_RTU_LITTLE_ENDIAN=*(char *)&num==1;
  }
  return G_RTU_LITTLE_ENDIAN;
}

void rtu_solveQuadratic(float32 a, float32 b, float32 c, float32 *p_a1, float32 *p_a2) {
  float32 b_squared=b*b;
  float32 four_ac=4*a*c;
  float32 two_a=2*a;
  LOG_ASSERT(b_squared>=four_ac, "neg root. a %f, b %f, c %f", a, b, c);
  float32 root=SQRT32(b_squared-four_ac);
  *p_a1=(-b+root)/two_a;
  *p_a2=(-b-root)/two_a;
}

bool rtu_isAngAcuteBetween(float32 a1, float32 a2, float32 aInner) {
  LOG_ASSERT(a1>=-M_PI && a1<M_PI, "out of range a1 %f", a1);
  LOG_ASSERT(a2>=-M_PI && a2<M_PI, "out of range a2 %f", a2);
  LOG_ASSERT(aInner>=-M_PI && aInner<M_PI, "out of range aInner %f", aInner);
  float32 min=MIN(a1,a2);
  float32 max=MAX(a1,a2);

  if(max-min<=M_PI_2) {
    //min to max is an acute angle
    return (aInner<max) && (aInner>=min);
  }
  return false;
}

bool rtu_isMAcuteBetween(float32 m1, float32 m2, float32 mInner) {
  float32 a1_a=atan(m1);
  float32 a1_b=rtu_altAng(a1_a);

  float32 a2_a=atan(m2);
  //float32 a2_b=rtu_altAng(a2_a);

  float32 aInner_a=atan(mInner);
  float32 aInner_b=rtu_altAng(aInner_a);

  //First check if aInner_a is acute between either (a2_a and a1_a) or (a2_a and a1_b)
  //Return true if it is
  if(rtu_isAngAcuteBetween(a2_a, a1_a, aInner_a) || 
     rtu_isAngAcuteBetween(a2_a, a1_b, aInner_a)) {
    return true;
  }
  //If not then check if aInner_b is acute between either (a2_a and a1_a) or (a2_a and a1_b)
  //Return true if it is
  if(rtu_isAngAcuteBetween(a2_a, a1_a, aInner_b) || 
     rtu_isAngAcuteBetween(a2_a, a1_b, aInner_b)) {
    return true;
  }
  //Else return false
  return false;
}

#ifdef EMSCRIPTEN

#include <emscripten.h>

EM_JS(void, rtu_simpleLog, (const char *p_level, const char *p_file, int line, const char *pMsg), {
    glb().rtu.log(UTF8ToString(p_level)+
		  UTF8ToString(p_file)+':'+
		  line+'. '+
		  UTF8ToString(pMsg)); 
    /*
   console.log(UTF8ToString(p_level)+
		  UTF8ToString(p_file)+':'+
		  line+'. '+
		  UTF8ToString(pMsg));
    */
});

#define G_RTU_LINE_LEN 512
sint8 G_RTU_LINE[G_RTU_LINE_LEN];
void rtu_log(const sint8 * p_level, const sint8 * p_file, uint32 line, const sint8* p_format, ... ) {
  va_list args;
  
  va_start( args, p_format );
  uint32 num_bytes_expanded = vsnprintf(G_RTU_LINE, G_RTU_LINE_LEN, p_format, args);
  va_end( args );
  if(num_bytes_expanded>=G_RTU_LINE_LEN) {
    G_RTU_LINE[G_RTU_LINE_LEN-1]=0;
  }
    
  rtu_simpleLog(p_level, p_file, (sint32)line, G_RTU_LINE);
}

#else

#ifdef NDK

#include <android/log.h>

#define G_RTU_LINE_LEN 512
sint8 G_RTU_LINE[G_RTU_LINE_LEN];

void rtu_log(const sint8 * p_level, const sint8 * p_file, uint32 line, const sint8* p_format, ... ) {
	va_list args;
	va_start( args, p_format );
	vsnprintf(G_RTU_LINE, G_RTU_LINE_LEN, p_format, args);
	va_end( args );

	__android_log_print(ANDROID_LOG_FATAL, RTU_APPNAME, "%s %s:%u. %s", p_level, p_file, line, G_RTU_LINE);
}

#else

void rtu_log(const sint8 * p_level, const sint8 * p_file, uint32 line, const sint8* p_format, ... ) {
  va_list args;

  printf("%s%s:%u. ", p_level, p_file, line);
  va_start( args, p_format );
  vprintf(p_format, args);
  va_end( args );
  printf("\n");
}

#endif

#endif

void *rtu_memAlloc(uint32 bytes) {
  void *p_mem=malloc(bytes);
  //LOG_INFO("malloced %u bytes. returning %p", bytes, p_mem);
  return p_mem;
}

void rtu_memFree(void *p_mem) {
  //LOG_INFO("freeing %p", p_mem);
  free(p_mem);
}

static sint8 *rtu_test_fastATan(void) {
#ifdef RTU_FAST_ATAN
  rtu_globals *p_globals=rtu_globalsInit();
  uint32 divisors=2;
  rtu_initFastATan(divisors, p_globals);
  float32 fuzz=M_PI/(divisors*4);
  if(!p_globals) {
    LOG_ASSERT(false, "failed to malloc");
    return 0;
  }

  float32 angs[]={0, 0.25, 0.5, 0.75, 1.00, 1.5, 2, 4, 9999};
  float32 answers[]={0, 0.24498, 0.46365, 0.64350, 0.78540, 0.98279, 1.1071, 1.3258, 1.5707};
  LOG_ASSERT(DIM(angs)==DIM(answers), "incorrect number of answers");
  
  mu_assert("incorrect atan for 0", rtu_similar(rtu_fastATan(angs[0], p_globals), answers[0], fuzz));
  mu_assert("incorrect atan for 0.25", rtu_similar(rtu_fastATan(angs[1], p_globals), answers[1], fuzz));
  mu_assert("incorrect atan for 0.5", rtu_similar(rtu_fastATan(angs[2], p_globals), answers[2], fuzz));
  mu_assert("incorrect atan for 0.75", rtu_similar(rtu_fastATan(angs[3], p_globals), answers[3], fuzz));
  mu_assert("incorrect atan for 1.00", rtu_similar(rtu_fastATan(angs[4], p_globals), answers[4], fuzz));
  mu_assert("incorrect atan for 1.5", rtu_similar(rtu_fastATan(angs[5], p_globals), answers[5], fuzz));
  mu_assert("incorrect atan for 2", rtu_similar(rtu_fastATan(angs[6], p_globals), answers[6], fuzz));
  mu_assert("incorrect atan for 4", rtu_similar(rtu_fastATan(angs[7], p_globals), answers[7], fuzz));
  mu_assert("incorrect atan for 9999", rtu_similar(rtu_fastATan(angs[8], p_globals), answers[8], fuzz));

  mu_assert("incorrect atan for -0", rtu_similar(rtu_fastATan(-angs[0], p_globals), -answers[0], fuzz));
  mu_assert("incorrect atan for -0.25", rtu_similar(rtu_fastATan(-angs[1], p_globals), -answers[1], fuzz));
  mu_assert("incorrect atan for -0.5", rtu_similar(rtu_fastATan(-angs[2], p_globals), -answers[2], fuzz));
  mu_assert("incorrect atan for -0.75", rtu_similar(rtu_fastATan(-angs[3], p_globals), -answers[3], fuzz));
  mu_assert("incorrect atan for -1.00", rtu_similar(rtu_fastATan(-angs[4], p_globals), -answers[4], fuzz));
  mu_assert("incorrect atan for -1.5", rtu_similar(rtu_fastATan(-angs[5], p_globals), -answers[5], fuzz));
  mu_assert("incorrect atan for -2", rtu_similar(rtu_fastATan(-angs[6], p_globals), -answers[6], fuzz));
  mu_assert("incorrect atan for -4", rtu_similar(rtu_fastATan(-angs[7], p_globals), -answers[7], fuzz));
  mu_assert("incorrect atan for -9999", rtu_similar(rtu_fastATan(-angs[8], p_globals), -answers[8], fuzz));

  //speed test, disabled because it takes time to run, enable if required
#if 0
  struct timeval tval_before, tval_after, tval_result_fast, tval_result_slow;
  float64 answer_sum_slow=0, answer_sum_fast=0;

  uint32 slow_loops=0, fast_loops=0;
  uint32 num_per_loop=10000;
  uint32 num_tests=DIM(angs);
  gettimeofday(&tval_before, NULL);
  do {
    for(uint32 idx=0; idx<num_per_loop; idx++) {
      for(uint32 test=0; test<num_tests; test++) {
	//answer_sum_slow+=ATAN32(angs[test]);
	answer_sum_slow++;
      }
      for(uint32 test=0; test<num_tests; test++) {
	//answer_sum_slow+=ATAN32(-angs[test]);
	answer_sum_slow--;
      }
    }

    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result_slow);
    slow_loops++;
  } while(tval_result_slow.tv_sec<10);

  LOG_INFO("slow answer %f", answer_sum_slow);
  gettimeofday(&tval_before, NULL);
  do {
    for(uint32 idx=0; idx<num_per_loop; idx++) {
      for(uint32 test=0; test<num_tests; test++) {
	//answer_sum_fast+=rtu_fastATan(angs[test], p_globals);
	answer_sum_fast++;
      }
      for(uint32 test=0; test<num_tests; test++) {
	//answer_sum_fast+=rtu_fastATan(-angs[test], p_globals);
	answer_sum_fast--;
      }
    }

    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result_slow);
    fast_loops++;
  } while(fast_loops<slow_loops);

  /*

  for(uint32 idx=0; idx<num_per_loop*slow_loops; idx++) {
    for(uint32 test=0; test<num_tests; test++) {
      answer_sum_fast+=rtu_fastATan(angs[test], p_globals);
    }
    for(uint32 test=0; test<num_tests; test++) {
      answer_sum_fast+=rtu_fastATan(-angs[test], p_globals);
    }
  }
  */
  gettimeofday(&tval_after, NULL);
  timersub(&tval_after, &tval_before, &tval_result_fast);
  LOG_INFO("fast answer %f", answer_sum_fast);

  LOG_ASSERT(tval_result_slow.tv_sec>=1, "speed test it too short to be meaningful");
  LOG_INFO("slow_loops: %u slow time: %ld.%06ld fast time: %ld.%06ld", slow_loops*num_per_loop*num_tests*2, (long int)tval_result_slow.tv_sec, (long int)tval_result_slow.tv_usec, (long int)tval_result_fast.tv_sec, (long int)tval_result_fast.tv_usec);
  mu_assert("fast atans took longer to calculate", (tval_result_fast.tv_sec<tval_result_slow.tv_sec) || (tval_result_fast.tv_sec==tval_result_slow.tv_sec && tval_result_fast.tv_usec<tval_result_slow.tv_usec));
#endif
  
  rtu_destroyATan(p_globals);
  rtu_globalsDestroy(p_globals);
#endif
  return 0;
}

sint8 *rtu_test(void) {
  mu_run_test(rtu_test_fastATan);
  return 0;
}

