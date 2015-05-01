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

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "draw.h"
#include "../minunit.h"
#include "../rtu.h"
#include "../types.h"
#include "../ccan/typesafe_cb/typesafe_cb.h"

bool draw_canvasSetup(draw_canvas *p_canvas, uint32 w, uint32 h, uint32* p_pixels) {
  p_canvas->p_bitmap=p_pixels;
  if(!p_canvas->p_bitmap) {
    LOG_ASSERT(false, "null canvas provided");
    p_canvas->w=0;
    p_canvas->h=0;
    return false;
  } else {
    p_canvas->w=w;
    p_canvas->h=h;
    return true;
  }
}

void draw_canvasResetDirty(draw_canvas *p_canvas) {
    draw_rectIntInit(&p_canvas->extantDirty);
}

void draw_init(const uint32 w, const uint32 h, const float32 devicePixelRatio, uint32 *p_pixels, draw_globals *p_globals) {
  uint32 size=sizeof(uint32)*w*h;
  if(p_pixels) {
    rtu_memZero(p_pixels, size);
  }
  LOG_INFO("pix ratio: %f", devicePixelRatio);
  draw_canvasInit(&p_globals->canvas, w, h, devicePixelRatio);
  draw_canvasSetup(&p_globals->canvas, w, h, p_pixels);
}

extern inline void draw_atOffset(uint32 pOffset, uint32 col, const draw_globals *p_globals);
extern inline uint32 draw_get(uint32 pOffset, const draw_globals *p_globals);
extern inline void draw_canvasClear(draw_canvas *p_canvas, uint32 offset, uint32 len);
extern inline uint32 draw_canvasRenderedWidth(const draw_canvas *p_canvas);
extern inline uint32 draw_canvasRenderedHeight(const draw_canvas *p_canvas);
extern inline uint32 draw_canvasWidth(const draw_canvas *p_canvas);
extern inline uint32 draw_canvasHeight(const draw_canvas *p_canvas);
extern inline void draw_onOffset(uint32 pOffset, uint32 col, const draw_globals *p_globals);
extern inline void draw_vertDot(const draw_vert *p_0, uint32 col, const draw_globals *p_globals);
extern inline void draw_dot(uint32 x, uint32 y, uint32 col, const draw_globals *p_globals);
extern inline draw_vert draw_copy(const draw_vert *p_0);
extern inline draw_vert draw_add(const draw_vert *p_v0, const draw_vert *p_v1);
extern inline draw_vert draw_neg(const draw_vert *p_v0);
extern inline draw_vert draw_abs(const draw_vert *p_v0);
extern inline draw_vert draw_diff(const draw_vert *p_v0, const draw_vert *p_v1);
extern inline draw_vert draw_by(const draw_vert *p_v0, const float32 scalar);
extern inline draw_vert draw_byOneByOne(const draw_vert *p_v0, const draw_vert *p_multiplier);
extern inline draw_vert draw_divide(const draw_vert *p_v0, const float32 scalar);
extern inline draw_vert draw_divideByUInt(const draw_vert *p_v0, const uint32 scalar, const draw_globals *p_globals);
extern inline draw_vert draw_divideOneByOne(const draw_vert *p_v0, const draw_vert *p_divisor);
extern inline float32 draw_mag(const draw_vert *p_vert);
extern inline draw_vert draw_mean(const draw_vert *p_one, const draw_vert *p_two);
extern inline draw_vert draw_norm(const draw_vert *p_tan);
extern inline draw_vert draw_fd2Norm(const draw_vert *p_fd, float32 width);
extern inline draw_vert draw_fd2Tan(const draw_vert *p_fd, float32 width);
extern inline float32 draw_plotBezInitC_0(float32 fdd, float32 step);
extern inline float32 draw_plotBezInitC_1(float32 p_0, float32 p_1, float32 p_2, float32 step);
extern inline float32 draw_plotBezInitFDD(float32 p_0, float32 p_1, float32 p_2);
extern inline float32 draw_plotBezInitFD_0(float32 p_0, float32 p_1);
extern inline float32 draw_plotBezInitFD_1(float32 p_1, float32 p_2);
extern inline float32 draw_plotBezInitFDTimesT(float32 fd_0, float32 step);
extern inline float32 draw_fd(float32 c_2, float32 c_3, float32 t);
extern inline float32 draw_plotBezUpdateFXT(float32 f_x_t, float32 fd_times_t, float32 c_0);
extern inline float32 draw_plotBezUpdateFDTimesT(float32 fd_times_t, float32 c_1);
extern inline uint32 draw_toDir(const draw_vert *p_delta, const draw_globals *p_globals);
extern inline bool draw_toXDom(const draw_vert *p_delta);
extern inline void draw_setDir(const draw_vert *p_old, const draw_vert *p_new, uint32 *p_dir, const draw_globals *p_globals);
extern inline void draw_setDDir(const draw_vert *p_fd_times_t, const draw_vert *p_c_0, uint32 *p_dir, const draw_globals *p_globals);
extern inline float32 draw_manhattanDist(const draw_vert *p_0, const draw_vert *p_1);
extern inline float32 draw_euclideanDist(const draw_vert *p_0, const draw_vert *p_1);
extern inline uint32 draw_mem_inc_idx(const uint32 idx);
extern inline uint32 draw_largerPowerOf2(uint32 start, uint32 target);
extern inline uint32 draw_scanLogPos2ConstIdx(const draw_scanBrushLog *p_b, const float32 pos);
extern inline uint32 draw_gradPos2Idx(const draw_grad *p_grad, const draw_scanBrushLog *p_b, const float32 pos);
extern inline uint32 draw_gradIdx2ConstIdx(const draw_grad *p_grad, const uint32 grad_idx);
extern inline float32 draw_gradRowSideM(const draw_grad *p_grad, const draw_gradTranslated *p_grad_trans, const float32 y);
extern inline float32 draw_gradNewSideM(const draw_grad *p_grad, const draw_gradTranslated *p_grad_trans, const float32 x, const float32 y);
extern inline float32 draw_gradRow2NewSideM(const draw_grad *p_grad, const float32 row_side_m, const float32 x);
extern inline uint32 draw_antiAlias(float32 coord, uint32 col, bool next_pixel);
extern inline float32 draw_rangeDist(sint32 min, sint32 max, sint32 val);
extern inline draw_vert* draw_quadNextFD(draw_quad *p_bez);
/*
extern inline bool draw_polariseXDir(uint32 dir);
extern inline bool draw_polariseYDir(uint32 dir);
*/
extern inline float32 draw_mCs2X(float32 ma, float32 mb, float32 ca, float32 cb);
extern inline float32 draw_mXC2Y(float32 m, float32 x, float32 c);
extern inline float32 draw_ptSlope2C(const draw_vert *p_v, float32 m);
//extern inline void draw_scanLogShiftYs(draw_scanLog *p_log);
extern inline sint32 draw_capBoundaryCmp(const draw_capBoundary *p_fst, const draw_capBoundary *p_snd);
extern inline float32 draw_polynomial(uint32 n, uint32 i, float32 t);
extern inline float32 draw_bezierTerm(uint32 n, uint32 i, float32 t, float32 w, const draw_globals *p_globals);
extern inline draw_vert draw_bezierQuad(float32 t, const draw_vert *p_w[], const draw_globals *p_globals);
extern inline float32 draw_breadth2HalfWidth(const float32 breadth, const float32 blur_width);
extern inline draw_coordToIter *draw_blotCoordToIters(draw_blot *p_blot);
extern inline uint32 draw_blotHalfWidth2BBoxLen(const float32 half_width);
extern inline uint32 draw_blotBreadth2BBoxLen(const float32 breadth, const float32 blur_width);
extern inline uint32 draw_blotBBoxLen(const draw_blot *p_blot);
extern inline float32 draw_blotBreadth(const draw_blot *p_blot);
extern inline float32 draw_savedWidth2RenderedWidth(float32 saved_width, float32 blur_width);
extern inline uint32 draw_bboxLen(const draw_vert *p_box_dims);
extern inline draw_grads *draw_blotQuadrant2Grads(draw_blot *p_blot, const sint32 right, const sint32 down);
extern inline float32 draw_blotBreadth(const draw_blot *p_blot);
extern inline float32 draw_blotHalfWidth(const draw_blot *p_blot);
extern inline float32 draw_gradLongConst(const draw_grad *p_grad, const draw_vert *p_long_const);
extern inline draw_vert draw_blotBBoxDims(const draw_blot *p_blot);
extern inline bool draw_gradTranslatedFill(draw_gradTranslated *p_grad_trans, const draw_grad *p_grad, const draw_vert *p_mid);
extern inline void draw_gradTranslatedInit(draw_gradTranslated *p_grad_trans, const draw_grad *p_grad, const draw_vert *p_mid);
//extern inline bool draw_vertQuadLenientMatch(const draw_vert *p_fd, const sint32 horiz, const sint32 vert);
extern inline float32 draw_euclideanDistSquared(const draw_vert *p_0, const draw_vert *p_1);
extern inline sint32 draw_gradSideIncSgn4FD(const draw_grad *p_grad, const draw_vert *p_fd);
extern inline draw_vert draw_weightedMean(const draw_vert *p_one, const draw_vert *p_two, float32 t);
extern inline void draw_dotScanRowLeftBnd(float32 x, float32 y, draw_scanFillLog *p_f, const draw_gradsIf *p_grad_agg, const uint32 iter, draw_globals *p_globals);
extern inline void draw_dotScanRowRightBnd(float32 x, float32 y, draw_scanFillLog *p_f, const draw_gradsIf *p_grad_agg, const uint32 iter, draw_globals *p_globals);
extern inline draw_vert draw_vertRecip(const draw_vert *p_0);
extern inline bool draw_vertEq(const draw_vert *p_0, const draw_vert *p_1);
extern inline bool draw_vertSim(const draw_vert *p_0, const draw_vert *p_1, const float32 fuzz);
extern inline void draw_canvasMarkDirty(draw_canvas *p_canvas, const draw_vert *p_vert);
extern inline void draw_canvasMarkDirtyRect(draw_canvas *p_canvas, const draw_rect *p_new);
extern inline draw_rect draw_boundingBox(const draw_vert *p_0, const draw_vert *p_delta);
extern inline draw_rect draw_calcTerminalBox(const draw_vert *p_0, const draw_vert *p_fd, const float32 half_width);
//extern inline bool draw_vertQuadNumMatch(const draw_vert *p_fd, const uint32 q);
extern inline sint32 draw_quadrant2Horiz(const uint32 q);
extern inline sint32 draw_quadrant2Vert(const uint32 quadrant);
//extern inline bool draw_vertQuadMatch(const draw_vert *p_fd, const sint32 horiz, const sint32 vert);
extern inline draw_rect draw_calcTerminalBoxFromTan(const draw_vert *p_0, const draw_vert *p_tan);
extern inline uint32 draw_coordToIterVertY2Idx(const draw_coordToIter *p_c2I, const float32 y);
extern inline uint32 draw_coordToIterVert2Idx(const draw_coordToIter *p_c2I, const draw_vert *p_0);
extern inline draw_grads *draw_coordToIterGrads(draw_coordToIter *p_c2I);
extern inline uint32 draw_coordToIterMaxIter(draw_coordToIter *p_c2I);
extern inline draw_grad *draw_coordToIterGradPtr(draw_coordToIter *p_c2I);
extern inline draw_coordToIter *draw_blotQuadrant2CoordToIter(draw_blot *p_blot, const sint32 right, const sint32 down);
extern inline sint32 draw_horizVert2Quadrant(const sint32 horiz, const sint32 vert);
extern inline uint32 draw_gradReferenceIter(const draw_gradReference *p_grad_ref, const draw_vert *p_0);
extern inline void draw_gradReferenceSetLastPt(draw_gradReference *p_grad_ref, const draw_vert *p_0);
extern inline void draw_vertNullableSetPt(draw_vertNullable *p_inst, const draw_vert *p_0);
extern inline draw_gradTranslated *draw_gradReferenceTrans(draw_gradReference *p_grad_ref, const uint32 q, const uint32 iter);
extern inline draw_gradTranslated *draw_gradsTrans(draw_grads *p_grad_agg, const uint32 q, const uint32 iter);
extern inline uint32 draw_proxOffset(const draw_prox *p_prox, const float32 cen, const sint32 coord);
extern inline bool draw_proxLandmarkDistRanker(const draw_prox *p_prox, const uint32 x, const uint32 y_offset_idx, const uint32 landmark_row_dist_sq, const sint32 x_delta, const uint32 q, const uint32 iter);
extern inline uint32 draw_proxRowOffset(const draw_prox *p_prox, const draw_vert *p_center, const sint32 y);
extern inline bool draw_scanBrushLogLandmarkDistRanker(const draw_scanBrushLog *p_b, const uint32 x, const uint32 y_offset_idx, const uint32 landmark_row_dist_sq, const sint32 x_delta, const uint32 q, const uint32 iter);
extern inline uint32 draw_scanBrushLogRowOffset(const draw_scanBrushLog *p_b, const draw_vert *p_center, const sint32 y);
extern inline uint32 draw_proxProximity(const draw_prox *p_prox, const uint32 landmark_row_dist_sq, const sint32 x_delta);
extern inline uint32 draw_proxOffsetIdx(const draw_prox *p_prox, const float32 x_cen, const sint32 x, const uint32 y_offset_idx);
extern inline sint32 draw_landmarkXDelta(draw_landmark *p_landmark, const uint32 x, const uint32 q, const draw_vert *p_mid);
extern inline bool draw_scanBrushLogIsClosest(const draw_scanBrushLog *p_b, const float32 x_cen, const uint32 x, const uint32 y_offset_idx, const uint32 landmark_row_dist_sq, const sint32 x_delta);
extern inline bool draw_proxIsClosest(const draw_prox *p_prox, const float32 x_cen, const uint32 x, const uint32 y_offset_idx, const uint32 landmark_row_dist_sq, const sint32 x_delta);
extern inline float32 draw_scanBrushLogRelativise(const draw_scanBrushLog *p_b, float32 brush_center, uint32 coord);
extern inline uint32 draw_insertProximityVal(const uint32 old_nearest, const uint32 is_nearest, const uint32 val);
extern inline uint32 draw_gradReferenceQuadrant(const draw_gradReference *p_grad_ref);
extern inline uint32 draw_blotNumCoord2Iters(const draw_blot *p_blot);
extern inline uint32 draw_quadrant2CoordToIterIdx(const uint32 q);
extern inline uint32 draw_proxHalfSpan(const draw_prox *p_prox, const uint32 width, const bool long_half);
extern inline uint32 draw_extantDirtyNumRects(draw_globals *p_globals) ;
extern inline uint32 draw_extantDirtyTop(draw_globals *p_globals, const uint32 idx);
extern inline uint32 draw_extantDirtyBottom(draw_globals *p_globals, const uint32 idx);
extern inline uint32 draw_extantDirtyLeft(draw_globals *p_globals, const uint32 idx);
extern inline uint32 draw_extantDirtyRight(draw_globals *p_globals, const uint32 idx);
extern inline uint32 draw_dirtyNumRects(draw_globals *p_globals) ;
extern inline uint32 draw_dirtyTop(draw_globals *p_globals, const uint32 idx);
extern inline uint32 draw_dirtyBottom(draw_globals *p_globals, const uint32 idx);
extern inline uint32 draw_dirtyLeft(draw_globals *p_globals, const uint32 idx);
extern inline uint32 draw_dirtyRight(draw_globals *p_globals, const uint32 idx);
extern inline void draw_canvasMergeDirt(draw_canvas *p_canvas);
extern inline void draw_strokeContinue(draw_stroke *p_stroke, const draw_vert *p_start, draw_globals *p_globals);
extern inline float32 draw_gradATan(const draw_grad *p_grad, const draw_globals *p_globals);
extern inline float32 draw_scanLogCheckAngle(draw_scanLog *p_log, const draw_grad *p_last_grad, const draw_grad *p_grad, const draw_globals *p_globals);
extern inline draw_rectInt* draw_canvasDirty(draw_canvas *p_globals);
extern inline draw_rectInt* draw_canvasExtantDirty(draw_canvas *p_globals);
extern inline uint32 draw_renderCoreHeight(const draw_renderCore *p_render_core, const draw_scanFillLog *p_f);
extern inline void draw_brushInitInternal(draw_brush *p_brush, const uint32 col, const float32 breadth, const float32 blur_width, draw_globals *p_globals, const bool recalc_scaling);

draw_globals draw_globalsInitialFields(void) {
  draw_globals g={
    .canvas={
      .w=0,
      .h=0
    },
    .null_vert={ .x=0, .y=0 },
    .pp_y_dirs={{DRAW_DIR_UL, DRAW_DIR_U, DRAW_DIR_UR}, {DRAW_DIR_L, 0, DRAW_DIR_R}, {DRAW_DIR_DL, DRAW_DIR_D, DRAW_DIR_DR}},
    .draw_origin={ .x=0, .y=0 },
    .draw_pixels=0,
    .draw_iter_updates=0
  };
  return g;
}

draw_globals* draw_globalsInit(void) {
  draw_globals *p_globals=rtu_memAlloc(sizeof(draw_globals));
  if(p_globals) {
    rtu_memZero(p_globals, sizeof(draw_globals));
    *p_globals=draw_globalsInitialFields();
    rtu_globals* p_rtu_globals=rtu_globalsInit();
    p_globals->p_rtu=p_rtu_globals;
  } else {
    LOG_ASSERT(false, "rtu_globals not malloced");
  }
  return p_globals;
}

void draw_globalsDestroy(draw_globals *p_globals) {
  if(p_globals) {
    rtu_globalsDestroy(p_globals->p_rtu);
    rtu_memFree(p_globals);
  } else {
    LOG_ASSERT(false, "null draw_globals pointer");
  }
}

void draw_canvasInit(draw_canvas *p_canvas, const uint32 w, const uint32 h, const float32 devicePixelRatio) {
  /* devicePixelRatio indicates the display's pixel density. This
   * value is needed for scaling optimisation. A larger value
   * indicates a higher dpi display. PC monitors typically return a
   * devicePixelRatio of one whereas 'retina' displays are associated
   * with a devicePixelRatio of 2. A negative devicePixelRatio
   * indicates that scalingOptimisation is to be disabled.
   *
   */

  uint32 mag=1;
  for(uint32 i=0; i<DRAW_CANVAS_NUM_MAG_FACTORS; i++) {
    p_canvas->std_mag_factors[i]=mag;
    p_canvas->std_scaling_factors[i]=1/(float32)p_canvas->std_mag_factors[i];
    mag<<=1;
  }
  
  p_canvas->renderedW=p_canvas->w=w; //depending on the brush renderedW can differ from w if devicePixelRatio is <0 or is ==1.
  p_canvas->renderedH=p_canvas->h=h; //depending on the brush renderedH can differ from h if devicePixelRatio is <0 or is ==1.
  LOG_INFO("pix ratio: %f", devicePixelRatio);
  p_canvas->devicePixelRatio=devicePixelRatio;
  p_canvas->scaling_idx=UINT_MAX;
  draw_rectIntInit(&p_canvas->dirty);
  draw_rectIntInit(&p_canvas->extantDirty);
  p_canvas->stop=false;
}

uint32 draw_canvasFindStdScalingIdx(const draw_canvas *p_canvas, const float32 sought) {
  //returns an index into the p_canvas->std_mag_factors and p_canvas->std_scaling_factors arrays
  
  LOG_ASSERT(sought<=1, "sought: %f is too large", sought);
  LOG_ASSERT(sought>0, "sought: %f is too small", sought);
  uint32 start_idx=0, after_end_idx=DRAW_CANVAS_NUM_MAG_FACTORS, idx=after_end_idx>>1;
  while(p_canvas->std_scaling_factors[idx]!=sought) {
    if(sought>p_canvas->std_scaling_factors[idx] &&
       idx>start_idx) {
      after_end_idx=idx;
    } else if(sought<p_canvas->std_scaling_factors[idx] &&
	      idx+1<after_end_idx) {
      start_idx=idx+1;
    } else {
      break;
    }
    idx=(after_end_idx+start_idx)>>1;
  }

  uint32 too_small=(sought>p_canvas->std_scaling_factors[idx]);
  LOG_ASSERT(idx>=too_small, "attempt to return negative idx when searching scaling factors for %f", sought);
  uint32 not_smaller_than_idx=idx-too_small;
  LOG_ASSERT(not_smaller_than_idx<DRAW_CANVAS_NUM_MAG_FACTORS, "attempt to return out of range idx when searching scaling factors for %f", sought);
  return not_smaller_than_idx;
}

static sint8 *draw_test_canvasFindStdScalingFactor(void) {
  draw_canvas canvas;
  draw_canvasInit(&canvas, 100, 100, 2);
  float32 fuzz=0.0001;
  
  mu_assert("incorrect result when seeking 0.001", draw_canvasFindStdScalingIdx(&canvas, 0.001)+1==DRAW_CANVAS_NUM_MAG_FACTORS);
  mu_assert("failed to find scaling factor 1", IMPLIES(DRAW_CANVAS_NUM_MAG_FACTORS>=1, rtu_similar(canvas.std_scaling_factors[draw_canvasFindStdScalingIdx(&canvas, 1)], 1, fuzz)));
  mu_assert("failed to find scaling factor 0.5", IMPLIES(DRAW_CANVAS_NUM_MAG_FACTORS>=2, rtu_similar(canvas.std_scaling_factors[draw_canvasFindStdScalingIdx(&canvas, 0.5)],0.5, fuzz)));
  mu_assert("incorrect result when seeking 0.75", IMPLIES(DRAW_CANVAS_NUM_MAG_FACTORS>=2, rtu_similar(canvas.std_scaling_factors[draw_canvasFindStdScalingIdx(&canvas, 0.75)], 1, fuzz)));
  mu_assert("incorrect result when seeking 0.3", IMPLIES(DRAW_CANVAS_NUM_MAG_FACTORS>=2, rtu_similar(canvas.std_scaling_factors[draw_canvasFindStdScalingIdx(&canvas, 0.3)], 0.5, fuzz)));
    
  return 0;
}

float draw_canvasSetAndGetScalingIdx(draw_canvas *p_canvas, const uint32 opacity, const float32 blur_width) {
  //opacity ranges from 0-0xff
  //blur_width is the number of pixels it takes for a gradiant to go from opacity pixel opacity to zero opacity

  if(p_canvas->devicePixelRatio<0) {
    //LOG_INFO("no scaling allowed");
    return 0;
  }

  /*
  if(!recalc) {
    LOG_ASSERT(p_canvas->scaling_idx!=UINT_MAX, "unitialised scaling idx");
    return p_canvas->scaling_idx;
  }
  */
  
  float32 scaling_factor=MIN(1,opacity/(p_canvas->devicePixelRatio*blur_width*DRAW_SMALLEST_VISIBLE_OPACITY_CHANGE_PER_STD_PIXEL));
  //LOG_INFO("opacity: 0x%x blur: %f pix ratio: %f scaling: %f", opacity, blur_width, p_canvas->devicePixelRatio, scaling_factor);
  p_canvas->scaling_idx=draw_canvasFindStdScalingIdx(p_canvas, scaling_factor);
  float32 std_scaling_factor=p_canvas->std_scaling_factors[p_canvas->scaling_idx];

  LOG_ASSERT(p_canvas->std_mag_factors[p_canvas->scaling_idx]*(uint32)(p_canvas->renderedW*std_scaling_factor)==p_canvas->renderedW, "p_canvas->renderedW, %u must be divisible by all standard scalling factors", p_canvas->renderedW);
  LOG_ASSERT(p_canvas->std_mag_factors[p_canvas->scaling_idx]*(uint32)(p_canvas->renderedH*std_scaling_factor)==p_canvas->renderedH, "p_canvas->renderedH, %u must be divisible by all standard scalling factors", p_canvas->renderedH);

  p_canvas->w=((uint32)(p_canvas->renderedW*std_scaling_factor));
  p_canvas->h=((uint32)(p_canvas->renderedH*std_scaling_factor));
  return p_canvas->scaling_idx;
}

void draw_canvasCheckAndClear(draw_canvas *p_canvas, const draw_globals *p_globals) {
  uint32 *p_start=p_canvas->p_bitmap;
  uint32 renderedW=draw_canvasRenderedWidth(p_canvas);
  uint32 h=draw_canvasHeight(p_canvas);
  uint32 len=renderedW*h;
  uint32 *p_end=p_start+len;
  draw_rectInt rect;
  draw_rectIntInit(&rect);
  for(uint32 *p_i=p_start; p_i<p_end; p_i++){
    if(*p_i!=0) {
      uint32 offset=p_i-p_start;
      draw_vert pos={ .x=offset%renderedW, .y=offset/renderedW };
      LOG_INFO("coloured pixel at: %f, %f vals: 0x%x", pos.x, pos.y, *p_i);
      draw_rectIntFit(&rect, &pos);
    }
  }
  draw_rectIntReify(&rect);
  LOG_ASSERT(!draw_rectIntFilled(&rect), "originally wiped lt: %i, %i rb: %i, %i but still visible pixels at lt: %u, %u rb: %u, %u", p_canvas->dirty.lt.x, p_canvas->dirty.lt.y, p_canvas->dirty.rb.x, p_canvas->dirty.rb.y, rect.lt.x, rect.lt.y, rect.rb.x,rect.rb.y);

  if(draw_rectIntFilled(&rect)) {
    uint32 w=draw_canvasWidth(p_canvas);
    uint32 offset=0;
    for(uint32 y=0; y<h; y++) {
      draw_canvasClear(p_canvas, offset, w);
      offset+=renderedW;
    }
  }
}

void draw_canvasClearAll(draw_canvas *p_canvas) {
  draw_canvasClear(p_canvas, 0, draw_canvasRenderedWidth(p_canvas)*draw_canvasRenderedHeight(p_canvas));
  draw_rectIntInit(&p_canvas->extantDirty);
  p_canvas->extantDirty.lt.x=0;
  p_canvas->extantDirty.lt.y=0;
  p_canvas->extantDirty.rb.x=p_canvas->w-1;
  p_canvas->extantDirty.rb.y=p_canvas->h-1;
  draw_canvasMergeDirt(p_canvas);
}

void draw_canvasWipe(draw_canvas *p_canvas, const draw_globals *p_globals) {
  draw_rectIntReify(&p_canvas->dirty);
  if(draw_rectIntFilled(&p_canvas->dirty)) {
    //LOG_INFO("wiping dirty canvas");
    //uint32 w=draw_canvasWidth(p_canvas);
    //uint32 h=draw_canvasHeight(p_canvas);
    uint32 w=p_canvas->last_w;
    uint32 h=p_canvas->last_h;
    LOG_ASSERT(p_canvas->dirty.lt.y<=p_canvas->dirty.rb.y, "out of vertical range dirty: %u, %u. width: %u", p_canvas->dirty.lt.y, p_canvas->dirty.rb.y, h);
    sint32 y=MAX(p_canvas->dirty.lt.y, (sint32)0);
    sint32 start_x=MAX(p_canvas->dirty.lt.x, (sint32)0);
    sint32 end_x=MIN(p_canvas->dirty.rb.x+1+1, (sint32)w); //+1 because a vert at (8.5, 10) will affects pixels (8, 10) and (9, 10). another +1 because start and end bounds are inclusive, but we want to use the to calc length
    uint32 row_len=MAX(0, end_x-start_x);
    sint32 max_y=MIN(p_canvas->dirty.rb.y+1+1, (sint32)h); //see above for explanation of +1+1.
    uint32 renderedW=draw_canvasRenderedWidth(p_canvas);
    uint32 offset=y*renderedW+start_x;
    for(; y<max_y; y++) {
      draw_canvasClear(p_canvas, offset, row_len);
      offset+=renderedW;
    }

  } else {
    //LOG_INFO("wiping clean canvas");
  }
#ifdef NDK
  #ifdef EMSCRIPTEN
  LOG_ASSERT(false, "can't run draw_canasCheckAndClear in emscripten compiled code");
  #endif
  DO_ASSERT(draw_canvasCheckAndClear(p_canvas, p_globals));
#endif
  p_canvas->last_w=p_canvas->w;
  p_canvas->last_h=p_canvas->h;
  draw_rectIntInit(&p_canvas->extantDirty);
  draw_rectIntInit(&p_canvas->dirty);
}

void draw_canvasIgnoreDirt(draw_canvas *p_canvas) {
    draw_rectIntInit(&p_canvas->dirty);
}

float32 draw_test_blurWidth(float32 saved_width) {
  //uint32 width_int=ROUND32(saved_width);
  //returns total blur on each side (e.g.right and left) of stroke
  float32 some=saved_width*0.5;
  float32 log_width=LOG32(saved_width)*0.5;
  float32 lower_blur_width;
  if(some>log_width) {
    lower_blur_width=some-log_width;
  } else {
    lower_blur_width=0;
  }

  float32 max_blur_width=MAX(2,lower_blur_width*2); //<<1 is not in the java version, *2 so as to refer to both inner and outer blur
  return MIN(max_blur_width, saved_width*2); //without this the blur_width could be > than the total width
}

static sint8 *draw_test_blotBBoxDims(void) {
  draw_blot blot={ .breadth=40, .blur_width=draw_test_blurWidth(40) };
  draw_vert bbox=draw_blotBBoxDims(&blot);
  uint32 bbox_len=draw_bboxLen(&bbox);
  uint32 direct_bbox_len=draw_blotBreadth2BBoxLen(blot.breadth, blot.blur_width);
  mu_assert("bbox len mismatch", bbox_len==direct_bbox_len);
  return 0;
}

#if 0
static sint8 *draw_test_adhoc_type_casting(void) {
  /*
    we do a memset in draw_quadrilateralScan to initialise a series of uint32 fields to UINT_MAX
    since memset only accepts a byte initialisation value, we set all bytes to 0xff, and
    assume that when eventually we read the uint32 field value, a value of UINT_MAX means
    that the field has never been changed after initialisation.
    
    Here we check that assumption is correct.
  */

  unsigned char p_b_test[sizeof(uint32)];
  for(uint32 i=0; i<sizeof(uint32); i++) {
    p_b_test[i]=0xff;
  }
  mu_assert("memset failed for uint32 representation of 0xff, 0xff, 0xff, 0xff as UINT_MAX", *(uint32 *)p_b_test==UINT_MAX);
  return 0;
}
#endif

static sint8 *draw_test_vertAng(void) {

  draw_vert origin={ 150, -600 };
  
  draw_vert offsets[8]={
    { .x=0, .y=100 },
    { .x=100, .y=100 },
    { .x=100, .y=0 },
    { .x=100, .y=-100 },
    { .x=0, .y=-100 },
    { .x=-100, .y=-100 },
    { .x=-100, .y=0 },
    { .x=-100, .y=100 },
  };

  float32 angles[8]={
    0,
    M_PI_4,
    M_PI_2,
    3*M_PI_4,
    M_PI,
    5*M_PI_4,
    3*M_PI_2,
    7*M_PI_4
  };

  draw_vert pts[8];
  for(uint32 i=0; i<DIM(offsets); i++) {
    pts[i]=draw_add(&origin, &offsets[i]);
    mu_assert("failed angle calculation", rtu_similar(draw_vertAng(&origin, &pts[i]), angles[i], 0.01));
  }

  return 0;
}

inline uint32 draw_toDir(const draw_vert *p_delta, const draw_globals *p_globals) {
  float32 right=p_delta->x;
  float32 down=p_delta->y;
  LOG_ASSERT(down!=0 || right!=0, "down and left are zero, returning invalid direction");
  return p_globals->pp_y_dirs[SGN(down)+1][SGN(right)+1];
}

inline bool draw_toXDom(const draw_vert *p_delta) {
  return ABSF(p_delta->y)>ABSF(p_delta->x);
}

inline void draw_setDir(const draw_vert *p_old, const draw_vert *p_new, uint32 *p_dir, const draw_globals *p_globals) {
  //+ve if delta.y>0
  //+ve if delta.x>0
  draw_vert delta=draw_diff(p_new, p_old);
  if(delta.y==0 && delta.x==0) {
    return;
  }

  *p_dir=draw_toDir(&delta, p_globals);
}

inline void draw_setDDir(const draw_vert *p_fd_times_t, const draw_vert *p_c_0, uint32 *p_dir, const draw_globals *p_globals) {
  //+ve if delta.y>0
  //+ve if delta.x>0
  draw_vert delta=draw_add(p_fd_times_t, p_c_0);
  if(delta.y==0 && delta.x==0) {
    return;
  }

  *p_dir=draw_toDir(&delta, p_globals);
}

inline uint32 draw_antiAlias(float32 coord, uint32 col, bool next_pixel) {
  uint32 intVal=(uint32)coord;
  uint32 opacity=(col & DRAW_OPACITY_MASK);
  float32 discrepancy=next_pixel?intVal+1-coord:coord-intVal;

  LOG_ASSERT(opacity>0xff, "opacity too small %u", opacity);
  uint32 new_col=(col & DRAW_OPACITY_INVERSE_MASK)|(((uint32)(opacity*discrepancy)|DRAW_MIN_OPACITY)&DRAW_OPACITY_MASK);
  //LOG_INFO("old col 0x%x, \tnew col 0x%x \tcoord %f \t next_pixel: %u", col, new_col, coord, next_pixel);
  return new_col;
}

inline float32 draw_rangeDist(sint32 min, sint32 max, sint32 val) {
  //returns -ve if val<min, +ve if val>max, 0 otherwise
  sint32 delta=0;
  LOG_ASSERT(min<=max, "min>max: %f %f", min, max);
  if(MIN(val, min)==val) {
    //val<min
    delta=val-min;
  } else if(MAX(val, max)==val) {
    //val>max
    delta=val-max;
  }
  return delta;
}

void draw_gradInitConsts(draw_grad *p_grad, const draw_vert *p_delta) {
  p_grad->c5=p_delta->x;
  p_grad->c6=p_delta->y;
}

uint32 draw_findFirstIter4Row(draw_grad *p_iter_2_grad, draw_gradTranslated *p_iter_2_grad_trans, const uint32 x, const uint32 y, const uint32 start_iter, const uint32 stop_iter, float32 *p_new_side_m, const sint32 iter_inc) {
  //this function only returns next iter if we've moved past the old one.
  LOG_ASSERT(rtu_abs(SGN((sint32)stop_iter-(sint32)start_iter)-SGN(iter_inc))<2, "would never terminate %u, %u", start_iter, stop_iter);
  uint32 iter=start_iter;

  draw_grad *p_next_grad=&p_iter_2_grad[iter];
  draw_gradTranslated *p_next_grad_trans=&p_iter_2_grad_trans[iter];
  
  //When x is incremented the *p_new_side_m=*p_new_side_m-p_next_grad->c6.
  //Therefore if *p_new_side_m and p_next_grad->c6 have the same signs (neither of which is 0), the iter's draw_grad will be crossed in future by a left to right scan across the screen.
  //Else if the signs are different (and neither are 0), then the draw_grad is behind us in a left to right scan
  //Else if *p_new_side_m is 0, then we are on the line
  //Else if p_next_grad->c6 is 0, then the draw_grad is horizontal and the scan line runs parallel to it (but not on it)
  sint32 new_side_sign=SGN(*p_new_side_m);
  sint32 next_c6_sign=SGN(p_next_grad->c6);

  sint32 sign_delta=new_side_sign-next_c6_sign;
  bool iter_update_needed=(sign_delta==2 || sign_delta==-2);
  LOG_ASSERT(p_next_grad->initialised, "uninitialised grad: %u", start_iter);
  LOG_ASSERT(p_next_grad_trans->initialised, "uninitialised grad trans: %u", start_iter);
  while(iter_update_needed && iter!=stop_iter) {
    //last_iter=iter;
    iter+=iter_inc;
    p_next_grad=&p_iter_2_grad[iter];
    p_next_grad_trans=&p_iter_2_grad_trans[iter];
    *p_new_side_m=draw_gradNewSideM(p_next_grad, p_next_grad_trans, x, y);

    sign_delta=SGN(*p_new_side_m)-SGN(p_next_grad->c6);
    iter_update_needed=(sign_delta==2 || sign_delta==-2);
  }
  return iter;
}

static sint8 *draw_test_findFirstIter4Row(void) {
  draw_globals *p_globals=draw_globalsInit();
  rtu_initFastATan(DRAW_ATAN_DIVISORS, p_globals->p_rtu);
  draw_strokeWidth w={
    .width=100,
    .width_squared=10000,
    .width_recip=0.01
  };
  draw_grad grads[]=
    {
      {
	.fd_signed={.x=0, .y=100},
	.mid={.x=50, .y=0},
	.initialised=true
      },
      {
	.fd_signed={.x=100, .y=0},
	.mid={.x=0, .y=50},
	.initialised=true
      }
    };
  draw_gradTranslated grad_trans[2]={
    {
      .initialised=false
    },
    {
      .initialised=false
    }
  };
  DO_ASSERT(grads[0].idx=0);
  DO_ASSERT(grads[1].idx=1);
  DO_ASSERT(grad_trans[0].idx=0);
  DO_ASSERT(grad_trans[1].idx=1);

  draw_fillIter(&grads[0], &grad_trans[0], &w, p_globals);
  draw_fillIter(&grads[1], &grad_trans[1], &w, p_globals);
  
  float32 new_side_m=0; //point is on start_iter => new_side_m==0

  mu_assert("should remain on a draw_grad until we move past it", draw_findFirstIter4Row(grads, grad_trans, 0, 0, 0, 1, &new_side_m, 1)==0);
  rtu_destroyATan(p_globals->p_rtu);
  draw_globalsDestroy(p_globals);
  return 0;
}

void draw_iterRangeInit(draw_iterRange *p_it_range, const uint32 q, const uint32 iter) {
  p_it_range->q=q;
  p_it_range->iter=iter;
}

sint32 draw_scanSpanIterInc(const draw_scanSpan *p_span) {
  return BCMP(p_span->iters.end, p_span->iters.start);
}

sint32 draw_gradIterInc(
			const draw_grad *p_a, 
			const draw_gradTranslated *p_a_trans, 
			const draw_grad *p_b, 
			const draw_gradTranslated *p_b_trans, 
			const uint32 start_x, 
			const uint32 end_x, 
			const uint32 y
			) {
  LOG_ASSERT(p_a->idx<=p_b->idx, "draw_grad of smaller iter must be passed as first argument");
  //returns iter inc, assuming *p_a is the draw_grad of the smaller iter
  float32 row_sides[2]={
    draw_gradRowSideM(p_a, p_a_trans, y), 
    draw_gradRowSideM(p_b, p_b_trans, y)
  };

  float32 new_side_start_xs[2]={
    draw_gradRow2NewSideM(p_a, row_sides[0], start_x),
    draw_gradRow2NewSideM(p_b, row_sides[1], start_x)
  };

  bool a_is_first;
  sint32 a_sgn=SGN(p_a->c6);
  sint32 b_sgn=SGN(p_b->c6);
    /* At least one of the c6s==0 (ie that line is horizontal) -- the answer then depends on the other draw_grad
       Which point (start_x, y) or (end_x, y) is closer to that non-horizontal draw_gad
       If (start_x, y) is closer then we wish to move from non horizontal draw_grad to the horizontal one. Otherwise vice versa.
    */
    
    if(a_sgn!=0 || b_sgn!=0) {
      const draw_grad *p_grads[2]={p_a, p_b};
      uint32 idx=b_sgn!=0;
      float32 abs_new_side_start_x=ABSF(new_side_start_xs[idx]);
      float32 abs_new_side_end_x=ABSF(draw_gradRow2NewSideM(p_grads[idx], row_sides[idx], end_x));
      return (((abs_new_side_start_x>abs_new_side_end_x)^!idx)<<1)-1;
      
    } else {
      //doesn't matter what we return here
      return +1;
    }
  return ((a_is_first)<<1)-1;
}


static sint8 *draw_test_gradIterInc(void) {
  draw_globals *p_globals=draw_globalsInit();
  rtu_initFastATan(DRAW_ATAN_DIVISORS, p_globals->p_rtu);
  draw_strokeWidth w={
    .width=100,
    .width_squared=10000,
    .width_recip=0.01
  };
  draw_grad grads[]=
    {
      {
	.fd_signed={.x=0, .y=100},
	.mid={.x=50, .y=0},
	.initialised=false
      },
      {
	.fd_signed={.x=100, .y=0},
	.mid={.x=100, .y=50},
	.initialised=false
      }
    };
  draw_gradTranslated grad_trans[2]={
    {
      .initialised=false
    },
    {
      .initialised=false
    }
  };
  draw_grad *p_horizontal_left=&grads[0];
  draw_grad *p_vertical=&grads[1];
  DO_ASSERT(p_horizontal_left->idx=0);
  DO_ASSERT(p_vertical->idx=1);
  DO_ASSERT(grad_trans[0].idx=0);
  DO_ASSERT(grad_trans[1].idx=1);
  DO_ASSERT(grads[0].fd_recorded=true);
  DO_ASSERT(grads[1].fd_recorded=true);
  draw_fillIter(&grads[0], &grad_trans[0], &w, p_globals);
  draw_fillIter(&grads[1], &grad_trans[1], &w, p_globals);


  mu_assert("result of 1 required from draw_gradIterInc when point ((0, 0) for example) is on the first line in the argument list", draw_gradIterInc(p_horizontal_left, &grad_trans[0], p_vertical, &grad_trans[1], 0, 1, 0)==1);

  DO_ASSERT(p_horizontal_left->idx=1);
  DO_ASSERT(p_vertical->idx=0);
  DO_ASSERT(grad_trans[0].idx=1);
  DO_ASSERT(grad_trans[1].idx=0);
  mu_assert("result of -1 required from draw_gradIterInc when point ((0, 0) for example) is on the second line in the argument list", draw_gradIterInc(p_vertical, &grad_trans[1], p_horizontal_left, &grad_trans[0], 0, 1, 0)==-1);

  draw_grad grads_h[]=
    {
      {
	.fd_signed={.x=0, .y=100},
	.mid={.x=150, .y=100},
	.initialised=false
      },
      {
	.fd_signed={.x=100, .y=0},
	.mid={.x=100, .y=50},
	.initialised=false
      }
    };
  draw_grad *p_horizontal_right=&grads_h[0];
  p_vertical=&grads_h[1];
  
  draw_gradTranslated grad_trans_h[2]={
    {
      .initialised=false
    },
    {
      .initialised=false
    }
  };;
  DO_ASSERT(p_horizontal_right->idx=0);
  DO_ASSERT(p_vertical->idx=1);
  DO_ASSERT(grad_trans_h[0].idx=0);
  DO_ASSERT(grad_trans_h[1].idx=1);
  DO_ASSERT(grads_h[0].fd_recorded=true);
  DO_ASSERT(grads_h[1].fd_recorded=true);
  draw_fillIter(&grads_h[0], &grad_trans_h[0], &w, p_globals);
  draw_fillIter(&grads_h[1], &grad_trans_h[1], &w, p_globals);

  mu_assert("result of -1 required from draw_gradIterInc when point ((100, 0) for example) is on the first line in the argument list", draw_gradIterInc(p_horizontal_right, &grad_trans_h[0], p_vertical, &grad_trans_h[1], 100, 101, 0)==-1);

  DO_ASSERT(p_horizontal_right->idx=1);
  DO_ASSERT(p_vertical->idx=0);
  DO_ASSERT(grad_trans_h[0].idx=1);
  DO_ASSERT(grad_trans_h[1].idx=0);
  mu_assert("result of 1 required from draw_gradIterInc when point ((100, 0) for example) is on the second line in the argument list", draw_gradIterInc(p_vertical, &grad_trans_h[1], p_horizontal_right, &grad_trans_h[0], 100, 101, 0)==1);

  rtu_destroyATan(p_globals->p_rtu);
  draw_globalsDestroy(p_globals);
  return 0;
}

draw_range draw_orderedIters(const draw_range *p_start, const draw_range *p_end) {
  //LOG_ASSERT(p_start->start<=p_start->end && p_end->start<=p_end->end, "failed draw_orderedIters precondition");
  if(p_start==p_end) {
    /*
      This occurs when plotting the horizontal straight edge of a blot.
      We don't want to get the mean of start and end fields as then this would result
      in an abrupt transition from the row at above / below the straight edge the the
      edge itself.
    */
    draw_range ordered=
      {
	.start=p_start->start,
	.end=p_end->end
      };
    return ordered;
  } else {
    /*
      We set ordered.start to the mean of p_start->start and
      p_start->end in order to deal with the case (eg) where one side of
      an stroke curls up at the bottom to meet the end line of the
      stroke creating an island draw_scanSpan instance between two outer
      edges. Assume the start and end iters of the island are both
      116. Also assume that the edge draw_scanSpan instances have iter
      values (start and end) of 82 (on the left) and 80 (on the right).
    
      On the next row, at the base of the curl there are only two
      draw_scanSpan instances. The first has a start iter of 83 and end
      iter of 116. The second has a start and end iter of 81.

      If ordered.start was p_start->start then in draw_rowImmed the
      start iter would be 82 and end iter 81, effectively flattening
      away the iter of 116.

      However if ordered.start was p_start->end the result would be an
      abrupt change in the iter value at the edge from the first row
      (above), to be one below it. By using the mean of p_start->start
      and p_start->end, we soften this transition.
    */
    draw_range ordered=
      {
	.start=(p_start->start+p_start->end)>>1,
	.end=(p_end->start+p_end->end)>>1
      };
    return ordered;
  }
}

draw_grad *draw_gradReferenceGradPtr(draw_gradReference *p_grad_ref, const uint32 q) {
  return draw_coordToIterGradPtr(&p_grad_ref->p_coord_2_iters[draw_quadrant2CoordToIterIdx(q)]);
}

void draw_rowNewSegmentRangePerPix(const draw_scanBrushLog *p_b, const sint32 first_x, const sint32 last_x, const uint32 y, draw_globals *p_globals, draw_gradReference *p_ref) {
  draw_vert *p_center=&p_ref->center;
  uint32 render_start=MAX(first_x, 0);
  sint32 render_after_end=MIN(last_x, (sint32)draw_canvasWidth(&p_globals->canvas));
  for(sint32 x=render_start; x<render_after_end; x++) {
    draw_vert pt={ .x=x, .y=y };
    uint32 quad_iter=draw_scanBrushLogPt2Iter(p_b, p_ref, &pt);
    uint32 q=(quad_iter & DRAW_PROXIMITY_QUADRANT_MASK) >> DRAW_PROXIMITY_ITER_BITS;
    LOG_ASSERT(q<=3, "out of range q: %u",q);
    uint32 iter=(quad_iter & DRAW_PROXIMITY_ITER_MASK);
    LOG_ASSERT(iter<=p_ref->max_iter, "out of range iter: %u, max: %u", iter, p_ref->max_iter);

    draw_grad *p_iter_2_grad=draw_gradReferenceGradPtr(p_ref, q);
    draw_grad *p_grad=&p_iter_2_grad[iter];
    draw_gradTranslated *p_trans=draw_gradReferenceTrans(p_ref, q, iter);
    draw_gradTranslatedInit(p_trans, p_grad, p_center);
    
    draw_gradReferenceSetIterRange(p_ref, q, iter);
    if(p_grad->on_x) {
      draw_rowNewSegmentRangeOnX(p_b, x, x+1, y, p_globals, &p_ref->grads_if);
    } else {
      draw_rowNewSegmentRangeOnY(p_b, x, x+1, y, p_globals, &p_ref->grads_if);
    }
  }
}

bool draw_horizSegmentInit(draw_horizSegment *p_seg, const draw_scanBrushLog *p_b, const sint32 first_x, const sint32 last_x, const uint32 y, draw_globals *p_globals, draw_gradsIf *p_grads_if) {
  if(y>=draw_canvasHeight(&p_globals->canvas)) {
    //LOG_INFO("y: %u, height: %u", y, draw_canvasHeight(&p_globals->canvas));
    return false;
  }
  p_seg->iter_range=(p_grads_if->p_iter_range_cb)(p_grads_if);
  //LOG_ASSERT(p_seg->iter_range.iter_inc!=0, "looks like iter range is unitialised");
  p_seg->q=p_seg->iter_range.q;
  p_seg->iter=p_seg->iter_range.iter;
  sint32 rightmost=draw_canvasWidth(&p_globals->canvas)-1;
  p_seg->start_x=MAX(first_x, (sint32)0);
  p_seg->end_x=MIN(last_x, rightmost);
  if(p_seg->start_x>=p_seg->end_x) { 
    //LOG_INFO("start_x: %i, end_x: %i", p_seg->start_x, p_seg->end_x);
    return false; //without this we'd be altering p_state (eg setting p_state->p_last to NULL) even when no pixels were processed
  }
  p_seg->p_iter_2_grad=(p_grads_if->p_grad_ptr_cb)(p_grads_if, p_seg->q);
  p_seg->p_grad=&p_seg->p_iter_2_grad[p_seg->iter];
  draw_gradTranslated *p_grad_trans=(p_grads_if->p_trans_cb)(p_grads_if, p_seg->iter_range.q, p_seg->iter);
  //LOG_ASSERT(p_grad->num_bounds>1, "only %u bounds for y %u", p_grad->num_bounds, p_log->cur.p.y);
  p_seg->row_start_offset=y*draw_canvasRenderedWidth(&p_globals->canvas);
  p_seg->p_anchor=&p_grad_trans->mid;
  LOG_ASSERT(p_seg->p_grad->initialised, "iter not initialised %u", p_seg->iter);
  LOG_ASSERT(p_grad_trans->initialised, "grad trans not initialised %u", p_seg->iter);
  DO_INFO(p_globals->draw_iter_updates++);
  return true;
}

//bool G_track=false;
//uint32 G_not_rendered=0;
void draw_rowNewSegmentRangeOnX(const draw_scanBrushLog *p_b, const sint32 first_x, const sint32 last_x, const uint32 y, draw_globals *p_globals, draw_gradsIf *p_grads_if) {
  draw_horizSegment seg;
  if(!draw_horizSegmentInit(&seg, p_b, first_x, last_x, y, p_globals, p_grads_if)) {
    //LOG_INFO("tracking and aborting new segment range on x.");
    return;
  }
  LOG_ASSERT(seg.p_grad->on_x, "invalid grad: %u", seg.iter);
  sint32 x=seg.start_x;
  float32 bound_delta=(y-seg.p_anchor->y)*seg.p_grad->slope_recip;
  float32 cen_x=(seg.p_anchor->x+bound_delta);

  float32 pos=(x-cen_x)*seg.p_grad->default_pos_inc+p_b->w.half_width;  //since p_anchor is the mid point between draw_grad bounds (leading to a -ve pos for half of the pixels in this draw_grad) we need to ensure pos has the correct range (i.e. between 0 and p_b->w.width)
  uint32 idx=draw_gradPos2Idx(seg.p_grad, p_b, pos);
  sint32 opacity=p_b->grad_consts.start_opacity[0]+p_b->grad_consts.p_incs_per_pos[0]*(pos-(p_b->grad_consts.start_threshes[0]));
  float32 inc_per_x=p_b->grad_consts.p_incs_per_pos[0]*seg.p_grad->default_pos_inc;
  //LOG_INFO("onX start x: %i, end_x: %i y: %u", x, seg.end_x,y);
  for(; x<seg.end_x; x++) {
    DO_INFO(p_globals->draw_pixels++);
    //bounds are sorted by x
    
    if(idx!=0) {
      break;
    }
    
    draw_dot(x, y, p_b->rgb | ((((uint32)((opacity>0)*opacity*p_b->opacity_frac))<<(DRAW_OPACITY_SHIFT-DRAW_OPACITY_SCALED_SHIFT)) & DRAW_OPACITY_MASK), p_globals); //we deal with negative opacities here so that incrementally updating opacity accounts for negative values correctly
    pos+=seg.p_grad->default_pos_inc;
    idx+=pos>p_b->grad_consts.threshes[0];
    opacity+=inc_per_x;
  }
  idx+=pos>=p_b->grad_consts.threshes[1];
  for(; x<seg.end_x; x++) {
    DO_INFO(p_globals->draw_pixels++);
    //bounds are sorted by x
    
    if(idx!=1) {
      break;
    }

    /* Invoking draw_onOffset here provides us with a single
     * chokepoint (the draw_onOffset function) via which all pixels
     * are rendered. It reduces the number of breakpoints that need to
     * be set when debugging rendering issues.
     * 
     */
    #if LOG_LEVEL <= LOG_ASSERT_LEVEL
    draw_onOffset(seg.row_start_offset+x, p_b->col, p_globals);
    #else
    draw_atOffset(seg.row_start_offset+x, p_b->col, p_globals);
    #endif
    pos+=seg.p_grad->default_pos_inc;
    idx+=pos>=p_b->grad_consts.threshes[1];
  }
  opacity=p_b->grad_consts.start_opacity[2]+p_b->grad_consts.p_incs_per_pos[2]*(pos-(p_b->grad_consts.start_threshes[2]));
  for(; x<seg.end_x; x++) {
    LOG_ASSERT(draw_gradPos2Idx(seg.p_grad, p_b, pos)==2, "idx should be 2");
    DO_INFO(p_globals->draw_pixels++);
    //bounds are sorted by x
    
    draw_dot(x, y, p_b->rgb | ((((uint32)((opacity>0)*opacity*p_b->opacity_frac))<<(DRAW_OPACITY_SHIFT-DRAW_OPACITY_SCALED_SHIFT)) & DRAW_OPACITY_MASK), p_globals); //we deal with negative opacities here so that incrementally updating opacity accounts for negative values correctly
    DO_ASSERT(pos+=seg.p_grad->default_pos_inc);
    opacity-=inc_per_x;
  }
}

void draw_rowNewSegmentRangeOnY(const draw_scanBrushLog *p_b, const sint32 first_x, const sint32 last_x, const uint32 y, draw_globals *p_globals, draw_gradsIf *p_grads_if) {
  draw_horizSegment seg;
  if(!draw_horizSegmentInit(&seg, p_b, first_x, last_x, y, p_globals, p_grads_if)) {
    //LOG_INFO("tracking and aborting new segment range on x");
    return;
  }
  LOG_ASSERT(!seg.p_grad->on_x, "invalid grad: %u", seg.iter);
  sint32 x=seg.start_x;
  float32 bound_delta=(x-seg.p_anchor->x)*seg.p_grad->slope_recip;
  float32 cen_y=(seg.p_anchor->y+bound_delta);

  float32 pos=(y-cen_y)*seg.p_grad->default_pos_inc; //since this is for !on_x, we know that we are at the very start or end of and iter, so any difference between start_y and the adjusted bound is due to the pixel boundary not aligning exactly with the iter, rather than use being in the middle of the iter
  float32 pos_dec=seg.p_grad->slope_recip*seg.p_grad->default_pos_inc;
  if(pos_dec>0) {
    /* We must ensure that the pos starts low and increases because in
       the loops below we assume that idx does not decrease in value.
     */
    pos=-pos;
    pos_dec=-pos_dec;
  }
  pos+=p_b->w.half_width; //since p_anchor is the mid point between draw_grad bounds (leading to a -ve pos for half of the pixels in this draw_grad) we need to ensure pos has the correct range (i.e. between 0 and p_log->w.width)
  float32 idx=draw_gradPos2Idx(seg.p_grad,p_b, pos);
  sint32 opacity=p_b->grad_consts.start_opacity[0]+p_b->grad_consts.p_incs_per_pos[0]*(pos-(p_b->grad_consts.start_threshes[0]));
  float32 inc_per_x=p_b->grad_consts.p_incs_per_pos[0]*(-pos_dec);
  //LOG_INFO("onY start x: %i, end_x: %i y: %u", x, seg.end_x, y);
  for(; x<seg.end_x; x++) {
    DO_INFO(p_globals->draw_pixels++);
    
    if(idx!=0) {
      break;
    }

    draw_dot(x, y, p_b->rgb | ((((uint32)((opacity>0)*opacity*p_b->opacity_frac))<<(DRAW_OPACITY_SHIFT-DRAW_OPACITY_SCALED_SHIFT)) & DRAW_OPACITY_MASK), p_globals); //we deal with negative opacities here so that incrementally updating opacity accounts for negative values correctly
    pos-=pos_dec;
    idx+=pos>p_b->grad_consts.threshes[0];
    opacity+=inc_per_x;
  }
  idx+=pos>=p_b->grad_consts.threshes[1];
  for(; x<seg.end_x; x++) {
    DO_INFO(p_globals->draw_pixels++);

    if(idx!=1) {
      break;
    }
    #if LOG_LEVEL <= LOG_ASSERT_LEVEL
    draw_onOffset(seg.row_start_offset+x, p_b->col, p_globals);
    #else
    draw_atOffset(seg.row_start_offset+x, p_b->col, p_globals);
    #endif
    pos-=pos_dec;
    idx+=pos>=p_b->grad_consts.threshes[1];
  }
  opacity=p_b->grad_consts.start_opacity[2]+p_b->grad_consts.p_incs_per_pos[2]*(pos-(p_b->grad_consts.start_threshes[2]));
  for(; x<seg.end_x; x++) {
    LOG_ASSERT(draw_gradPos2Idx(seg.p_grad, p_b, pos)==2, "idx should be 2");
    DO_INFO(p_globals->draw_pixels++);
  
    draw_dot(x, y, p_b->rgb | ((((uint32)((opacity>0)*opacity*p_b->opacity_frac))<<(DRAW_OPACITY_SHIFT-DRAW_OPACITY_SCALED_SHIFT)) & DRAW_OPACITY_MASK), p_globals); //we deal with negative opacities here so that incrementally updating opacity accounts for negative values correctly
    DO_ASSERT(pos-=pos_dec);
    opacity-=inc_per_x;
  }
}

void draw_invalidIter(uint32 x, uint32 y, uint32 col) {
  LOG_INFO("not rendering");
}

void draw_segInit(draw_seg *p_seg, const draw_vert *p_0, const draw_vert *p_1) {
  p_seg->pt_diff=draw_diff(p_1, p_0);
  p_seg->x_dist=(sint32)p_seg->pt_diff.x;
  p_seg->y_dist=(sint32)p_seg->pt_diff.y;

  /*
    The neg_??_adjustement variable ensures that when there is a
    change in sign in one of the dimensions between p_0 and p_1 the
    calculation of x_steps / y_steps is correct.

    Consider eg p_1->y==-0.416459829 and p_0->y==2.67809486.  Without
    the adustment value the
    rtu_abs((sint32)p_1->y-(sint32)p_0->y)==rtu_abs(0-2)==2.

    With the adjustment neg_y1_adjustment==1 so the result of the calculation is
    rtu_abs(0-1-2)==3
   */
  sint32 neg_x1_adjustment=p_1->x<0;
  sint32 neg_x0_adjustment=p_0->x<0;
  sint32 neg_y1_adjustment=p_1->y<0;
  sint32 neg_y0_adjustment=p_0->y<0;
  p_seg->x_steps=rtu_abs((((sint32)p_1->x)-neg_x1_adjustment)-(((sint32)p_0->x)-neg_x0_adjustment));
  p_seg->y_steps=rtu_abs((((sint32)p_1->y)-neg_y1_adjustment)-(((sint32)p_0->y)-neg_y0_adjustment));

  p_seg->x_dom=ABSF(p_seg->pt_diff.x)>ABSF(p_seg->pt_diff.y);

  p_seg->p_0=p_0;
  p_seg->p_1=p_1;
}

uint32 draw_segFastRenderX(const draw_seg *p_seg, draw_onPtCb *p_callback, draw_scanFillLog *p_arg, const draw_gradsIf *p_grad_agg, const uint32 iter, draw_globals *p_globals) {
  uint32 result;
  const draw_vert *p_0=p_seg->p_0;
  const draw_vert *p_1=p_seg->p_1;
  //LOG_INFO("dominated by x. y diff %f x steps %u", pt_diff.y, x_steps);
  if(ABSF(p_seg->pt_diff.x)>1) {
    sint32 inc;
    if(p_1->x>p_0->x) {
      inc=1;
    } else {
      inc=-1;
    }
    float32 pt=p_0->x+inc;
    float32 o_inc=rtu_div(p_seg->pt_diff.y, p_seg->x_steps, p_globals->p_rtu);
    LOG_ASSERT(ABSF(o_inc)<=1, "o_inc %f too large when it should be dominated by x. y dist %f", o_inc, p_seg->pt_diff.y);
    float32 o_pt=p_0->y+o_inc;
    while(CMP(p_1->x, pt)==inc) {
      (*p_callback)(pt, o_pt, p_arg, p_grad_agg, iter, p_globals);
      pt+=inc;
      o_pt+=o_inc;
    }
    result=DRAW_INNER_SEG_X_DOM;
  } else {
    result=DRAW_INNER_SEG_X_DOM;
  }
  return result;
}

uint32 draw_segSlowRenderX(const draw_seg *p_seg, draw_onPtCb *p_callback, draw_scanFillLog *p_arg, const draw_gradsIf *p_grad_agg, const uint32 iter, draw_globals *p_globals) {
  if(p_seg->y_steps==0) {
    return DRAW_INNER_SEG_X_DOM;
  }
  //LOG_INFO("whoah. slow path 1");
  const draw_vert *p_0=p_seg->p_0;
  const draw_vert *p_1=p_seg->p_1;
  sint32 inc;
  if(p_1->x>p_0->x) {
    inc=1;
  } else {
    inc=-1;
  }
  sint32 o_recover_inc;
  if(p_1->y>p_0->y) {
    o_recover_inc=1;
  } else {
    o_recover_inc=-1;
  }
  float32 pt=p_0->x+inc;
  float32 o_inc=p_seg->pt_diff.y/p_seg->x_steps;
  float32 last_o_pt=p_0->y;
  float32 o_pt=p_0->y+o_inc;
  while(CMP(p_1->x, pt)==inc) {
    while(rtu_abs((sint32)((uint32)o_pt-(uint32)last_o_pt))>1 && CMP(p_1->y, last_o_pt)==o_recover_inc) {
      float32 this_o_pt=last_o_pt+o_recover_inc;
      //LOG_INFO("whoah. slow path 1. this would have been a mistake. %f, %f", pt, this_o_pt);
      (*p_callback)(pt, this_o_pt, p_arg, p_grad_agg, iter, p_globals); 	
      last_o_pt=this_o_pt;
    }
    //LOG_INFO("whoah. slow path 1. %f, %f", pt, o_pt);
    (*p_callback)(pt, o_pt, p_arg, p_grad_agg, iter, p_globals);
    pt+=inc;
    last_o_pt=o_pt;
    o_pt+=o_inc;
  }
  while(rtu_abs((sint32)((uint32)p_1->y-(uint32)last_o_pt))>1 && CMP(p_1->y, last_o_pt)==o_recover_inc) {
    float32 this_o_pt=last_o_pt+o_recover_inc;
    //LOG_INFO("whoah. slow path 1 at end. this would have been a mistake. %f, %f", pt, this_o_pt);
    (*p_callback)(pt, this_o_pt, p_arg, p_grad_agg, iter, p_globals);
    last_o_pt=this_o_pt;      
  }
  //LOG_INFO("whoah. slow path 1 last. %f, %f", p_1->x, p_1->y);
  return DRAW_INNER_SEG_X_DOM;
}

uint32 draw_segFastRenderY(const draw_seg *p_seg, draw_onPtCb *p_callback, draw_scanFillLog *p_arg, const draw_gradsIf *p_grad_agg, const uint32 iter, draw_globals *p_globals) {
  //LOG_INFO("dominated by y. x diff %f y steps %u", pt_diff.x, y_steps);
  uint32 result;
  const draw_vert *p_0=p_seg->p_0;
  const draw_vert *p_1=p_seg->p_1;
  if(ABSF(p_seg->pt_diff.y)>1) {
    sint32 inc;
    if(p_1->y>p_0->y) {
      inc=1;
    } else {
      inc=-1;
    }
    float32 pt=p_0->y+inc;
    float32 o_inc=rtu_div(p_seg->pt_diff.x, p_seg->y_steps, p_globals->p_rtu);
    LOG_ASSERT(ABSF(o_inc)<=1, "o_inc %f too large when it should be dominated by y. x dist %f", o_inc, p_seg->pt_diff.x);
    float32 o_pt=p_0->x+o_inc;
    while(CMP(p_1->y, pt)==inc) {
      (*p_callback)(o_pt, pt, p_arg, p_grad_agg, iter, p_globals);
      pt+=inc;
      o_pt+=o_inc;
    }
    result=DRAW_INNER_SEG_Y_DOM;
  } else {
    result=DRAW_INNER_SEG_Y_DOM;
  }
  return result;
}

uint32 draw_segSlowRenderY(const draw_seg *p_seg, draw_onPtCb *p_callback, draw_scanFillLog *p_arg, const draw_gradsIf *p_grad_agg, const uint32 iter, draw_globals *p_globals) {
  if(p_seg->y_steps==0) {
    return DRAW_INNER_SEG_Y_DOM;
  }
  const draw_vert *p_0=p_seg->p_0;
  const draw_vert *p_1=p_seg->p_1;
  //LOG_INFO("whoah. slow path 2");
  sint32 inc;
  if(p_1->y>p_0->y) {
    inc=1;
  } else {
    inc=-1;
  }
  sint32 o_recover_inc;
  if(p_1->x>p_0->x) {
    o_recover_inc=1;
  } else {
    o_recover_inc=-1;
  }
  float32 pt=p_0->y+inc;
  float32 o_inc=p_seg->pt_diff.x/p_seg->y_steps; 
  float32 last_o_pt=p_0->x;
  float32 o_pt=p_0->x+o_inc;
  while(CMP(p_1->y, pt)==inc) {
    while(rtu_abs((sint32)((uint32)o_pt-(uint32)last_o_pt))>1 && CMP(p_1->x, last_o_pt)==o_recover_inc) {
      float32 this_o_pt=last_o_pt+o_recover_inc;
      //LOG_INFO("whoah. slow path 2. this would have been a mistake. %f, %f", this_o_pt, pt);
      (*p_callback)(this_o_pt, pt, p_arg, p_grad_agg, iter, p_globals);
      last_o_pt=this_o_pt;
    }
    //LOG_INFO("whoah. slow path 2. %f, %f", o_pt, pt);
    (*p_callback)(o_pt, pt, p_arg, p_grad_agg, iter, p_globals); 	
    pt+=inc;
    last_o_pt=o_pt;
    o_pt+=o_inc;
  }
  while(rtu_abs((sint32)((uint32)o_pt-(uint32)last_o_pt))>1 && CMP(p_1->x, last_o_pt)==o_recover_inc) {
    float32 this_o_pt=last_o_pt+o_recover_inc;
    //LOG_INFO("whoah. slow path 2 at end. this would have been a mistake. %f, %f", this_o_pt, pt);
    (*p_callback)(this_o_pt, pt, p_arg, p_grad_agg, iter, p_globals);
    last_o_pt=this_o_pt;
  }
  //LOG_INFO("whoah. slow path 2 last. %f, %f", p_1->x, p_1->y);
  return DRAW_INNER_SEG_Y_DOM;
}

uint32 draw_innerSeg(const draw_vert *p_0, const draw_vert *p_1, draw_onPtCb *p_callback, draw_scanFillLog *p_arg, const draw_gradsIf *p_grad_agg, const uint32 iter, draw_globals *p_globals) {
  if(!p_callback) {
    return DRAW_INNER_SEG_NULL;
  }
  
  draw_seg seg;
  draw_segInit(&seg, p_0, p_1);

  uint32 result;
  if((seg.x_steps==1 && seg.y_steps==1) || seg.x_steps+seg.y_steps==1) {
    //LOG_INFO("single pixel each way");
    result=seg.x_dom;
  } else if(seg.x_dist==0 && seg.y_dist==0) {
    //LOG_INFO("no pixel either way");
    return DRAW_INNER_SEG_NULL;
  } else if(ABSF(seg.pt_diff.y)<seg.x_steps) {
    result=draw_segFastRenderX(&seg, p_callback, p_arg, p_grad_agg, iter, p_globals);
  } else if(ABSF(seg.pt_diff.x)<=seg.y_steps) {
    //y_steps used above instead of ay_dist as it tells us how many rows need to be plotted here.
    //Calculating o_inc below based on ay_dist will result in overlay large increments to x,
    //screwing up antialiasing.

    //However this is a problem as while drawing a line, the dominating dimension will change quuite frequently
    result=draw_segFastRenderY(&seg, p_callback, p_arg, p_grad_agg, iter, p_globals);
  } else {
    if(ABSF(seg.pt_diff.x)*seg.x_steps>ABSF(seg.pt_diff.y)*seg.y_steps) {
      result=draw_segSlowRenderX(&seg, p_callback, p_arg, p_grad_agg, iter, p_globals);
    } else {
      result=draw_segSlowRenderY(&seg, p_callback, p_arg, p_grad_agg, iter, p_globals);
    }
  }
  return result;
}

bool draw_headFaster(const draw_vert *p_0, const draw_vert *p_1, draw_onPtCb *p_callback, draw_scanFillLog *p_arg, const draw_gradsIf *p_grad_agg, const uint32 iter, draw_globals *p_globals) {
  //p_callbacks should be defined as in draw_tail
  if(!p_callback) {
    return false;
  }
  
  draw_seg seg;
  draw_segInit(&seg, p_0, p_1);

  if((seg.x_steps==1 && seg.y_steps==1) || seg.x_steps+seg.y_steps==1) {
    //LOG_INFO("single pixel");
    (*p_callback)(p_0->x, p_0->y, p_arg, p_grad_agg, iter, p_globals);
    return true;
  } else if(seg.x_dist==0 && seg.y_dist==0) {
    //LOG_INFO("no pixel either way");
    return false;
  } else if(ABSF(seg.pt_diff.y)<seg.x_steps) {
    (*p_callback)(p_0->x, p_0->y, p_arg, p_grad_agg, iter, p_globals);
    draw_segFastRenderX(&seg, p_callback, p_arg, p_grad_agg, iter, p_globals);
    return true;
  } else if(ABSF(seg.pt_diff.x)<=seg.y_steps) {
    //y_steps used above instead of ay_dist as it tells us how many rows need to be plotted here.
    //Calculating o_inc below based on ay_dist will result in overly large increments to x,
    //screwing up antialiasing.

    //However this is a problem as while drawing a line, the dominating dimension will change quuite frequently
    (*p_callback)(p_0->x, p_0->y, p_arg, p_grad_agg, iter, p_globals);
    draw_segFastRenderY(&seg, p_callback, p_arg, p_grad_agg, iter, p_globals);
    return true;
  } else {
    if(ABSF(seg.pt_diff.x)*seg.x_steps>ABSF(seg.pt_diff.y)*seg.y_steps) {
      (*p_callback)(p_0->x, p_0->y, p_arg, p_grad_agg, iter, p_globals);
      draw_segSlowRenderX(&seg, p_callback, p_arg, p_grad_agg, iter, p_globals);
    } else {
      (*p_callback)(p_0->x, p_0->y, p_arg, p_grad_agg, iter, p_globals);
      draw_segSlowRenderY(&seg, p_callback, p_arg, p_grad_agg, iter, p_globals);
    }
    return true;
  }
}

bool draw_tail(draw_vert *p_0, draw_vert *p_1, draw_onPtCb * p_callback, draw_scanFillLog *p_arg, const draw_gradsIf *p_grad_agg, const uint32 iter, draw_globals *p_globals) {
  if(p_callback) {
    uint32 result=draw_innerSeg(p_0, p_1, p_callback, p_arg, p_grad_agg, iter, p_globals);
    (*p_callback)(p_1->x, p_1->y, p_arg, p_grad_agg, iter, p_globals);
    return result!=DRAW_INNER_SEG_NULL;
  }
  return DRAW_INNER_SEG_NULL;
}

bool draw_tailFaster(const draw_vert *p_0, const draw_vert *p_1, draw_onPtCb * p_callback, draw_scanFillLog *p_arg, const draw_gradsIf *p_grad_agg, const uint32 iter, draw_globals *p_globals) {
  //p_callbacks should be defined as in draw_tail
  if(p_callback) {
    uint32 result=draw_innerSeg(p_0, p_1, p_callback, p_arg, p_grad_agg, iter, p_globals);
    (*p_callback)(p_1->x, p_1->y, p_arg, p_grad_agg, iter, p_globals);
    return result!=DRAW_INNER_SEG_NULL;
  } else {
    return DRAW_INNER_SEG_NULL;
  }
}

uint32 draw_unshiftDir(uint32 dir) {
  return dir>>DRAW_DIR_SHIFT;
}

void draw_gradInitFill(draw_grad *p_grad, const draw_strokeWidth *p_w, const draw_globals *p_globals) {
  if(!p_grad->initialised){
    LOG_ASSERT(p_grad->fd_recorded, "fd not recorded for iter: %", p_grad->idx);
    p_grad->p_landmark=NULL;
    
    //LOG_ASSERT(p_grad->num_bounds>1, "num bounds %u", p_grad->num_bounds);
    draw_vert *p_fd_signed=&p_grad->fd_signed;
    draw_vert fd={ .x=ABSF(p_grad->fd_signed.x), .y=ABSF(p_grad->fd_signed.y) };
    //LOG_INFO("iter: %u fd: %f, %f", p_grad->idx, fd.x, fd.y);
    draw_vert *p_fd=&fd;

    float32 denom;
    /*
      Attempt to calculate delta in feathering_angles.svg (A)
      (remember fd is perpendicular to m):

      where on_x==true
      line through origin: y=mx
      circle with origin at its centre (w is radius): w^2=x^2+y^2
      intersection between the two (sub in y=mx): x=sqrt(w^2/(1+m^2))

      where on_x==false
      line through origin: x=y/m
      circle with origin at its centre (w is radius): w^2=x^2+y^2
      intersection between the two (sub in x=y/m): y=sqrt(w^2/(1+(1/m)^2)).

      Calculating default_pos_inc (using denom):
      also see feathering_angles.svg (A) where on_x==true
      default_pos_inc = w/(delta.x + (delta.y * (fd.x/fd.y)))
                      = (w*fd.y)/(delta.x*fd.y+delta.y*fd.x)

		      
     */
    float32 row_width_recip[2]={ p_w->width_recip };
    draw_vert delta;
    if(fd.y>fd.x) {
      p_grad->on_x=true;
      if(p_grad->fd_signed.y!=0) {
	float32 m=-(p_grad->fd_signed.x/p_grad->fd_signed.y); //-ve reciprocal of fd (since it's for a perpendicular line)
	/*
	  calculation of delta assumes line is through origin ie y=mx
	  So with pythagoras: w*w=x*x+mx*mx
	  =>w*w=x*x(1+m*m)
	  =>x*x=(w*w)/(1+m*m)
	*/
	float32 delta_x=SQRT32(p_w->width_squared/(1+m*m));
	delta.x=delta_x; 
	delta.y=m*delta_x;
      } else {
	LOG_ASSERT(p_grad->fd_signed.x!=0, "null fd");
	//horizontal tangent => vertical draw_grad
	delta.x=0;
	delta.y=-(sint32)p_w->width*BSGN(p_grad->fd_signed.x);
      }
      draw_gradInitConsts(p_grad, &delta);
      denom=((delta.x*p_fd->y)+(ABSF(delta.y)*p_fd->x));

      p_grad->slope_recip=p_fd_signed->y!=0?p_fd_signed->x/p_fd_signed->y:0;

      row_width_recip[1]=p_fd->y/denom;
    } else {
      p_grad->on_x=false;
      if(p_grad->fd_signed.x!=0) {
	float32 m_recip=-(p_grad->fd_signed.y/p_grad->fd_signed.x);
	float32 delta_y=SQRT32(p_w->width_squared/(1+m_recip*m_recip));
	delta.x=delta_y*m_recip;
	delta.y=delta_y;
      } else {
	LOG_ASSERT(p_grad->fd_signed.y!=0, "null fd");
	//vertical tangent => horizontal draw_grad
	delta.x=-(sint32)p_w->width*BSGN(p_grad->fd_signed.y);
	delta.y=0;
      }
      draw_gradInitConsts(p_grad, &delta);
      denom=((ABSF(delta.x)*p_fd->y)+(delta.y*p_fd->x));

      p_grad->slope_recip=p_fd_signed->x!=0?p_fd_signed->y/p_fd_signed->x:0;
      row_width_recip[1]=p_fd->x/denom;
    }
    p_grad->half_delta=draw_by(&delta, 0.5);
    p_grad->default_pos_inc=p_w->width*row_width_recip[denom!=0];
    LOG_ASSERT(p_grad->default_pos_inc>=(0-DRAW_FUZZ_FACTOR) && p_grad->default_pos_inc<=(1+DRAW_FUZZ_FACTOR), "out of range default_pos_inc: %f", p_grad->default_pos_inc);

    /* Derivation of expression to calc draw_grad->opacity_adj field
     * (dim_thresh-bounds[0].x)*p_grad->default_pos_inc=thresh
     * dim_thresh-bounds[0].x=thresh/p_grad->default_pos_inc;
     * dim_thresh=thresh/p_grad->default_pos_inc+bounds[0].x

     * frac_inc=(((uint32)dim_thresh)+1-dim_thresh)*incs[idx]
     * opacity_adj=frac_inc-incs[idx]
     */
    p_grad->initialised=true;
  }
}

void draw_gradConstsInit(draw_gradConsts *p_consts, const draw_strokeWidth *p_w) {
  p_consts->opacity_scaled=DRAW_OPACITY_MAX_SCALED;
  uint32 opacity_lsbits=(1<<DRAW_OPACITY_SCALED_SHIFT)-1;
  p_consts->opacity_scaled|=opacity_lsbits; //opacity_scaled is set to the opacity shifted to correct position with all less significant bits set to 1
  sint32 opacity_grad_per_pos=(sint32)(p_consts->opacity_scaled*p_w->half_blur_width_recip);
  p_consts->p_incs_per_pos[0]=opacity_grad_per_pos;
  p_consts->p_incs_per_pos[1]=0;
  p_consts->p_incs_per_pos[2]=-p_consts->p_incs_per_pos[0];
  p_consts->p_incs_per_pos[3]=0;

  //the values of fst_thresh and snd_thresh as well as draw_grad.start_opacity[0] and draw_grad_start_opacity[3] are crucial in ensuring that there is no seam at the draw_grad.on_x and !draw_grad.on_x boundary
  float32 fst_thresh=p_w->half_blur_width;
  float32 snd_thresh=p_w->width-fst_thresh;
  LOG_ASSERT(fst_thresh<=snd_thresh, "invalid threshes: %f, %f", fst_thresh, snd_thresh)

  p_consts->threshes[0]=fst_thresh;
  p_consts->threshes[1]=snd_thresh;
  p_consts->threshes[2]=p_w->width;
  p_consts->threshes[3]=UINT_MAX;

  p_consts->start_threshes[0]=0;
  p_consts->start_threshes[1]=p_consts->threshes[0];
  p_consts->start_threshes[2]=p_consts->threshes[1];
  p_consts->start_threshes[3]=p_consts->threshes[2];

  p_consts->start_opacity[0]=0; //setting this field to zero breaks the seam where a draw_grad.on_x meets !draw_grad.on_x
  p_consts->start_opacity[1]=p_consts->opacity_scaled;
  p_consts->start_opacity[2]=p_consts->opacity_scaled;
  p_consts->start_opacity[3]=0; //setting this field to zero breaks the seam where a draw_grad.on_x meets !draw_grad.on_x

}

void draw_recordFD(
		       draw_scanLog *p_log, 
		       draw_grads *p_grad_agg,
		       const draw_vert *p_0, 
		       const draw_vert *p_fd, 
		       const uint32 iter,
		       draw_globals *p_globals
		   ) {
  if(!p_0) {
    return;
  }
  draw_grad *p_grad=&p_grad_agg->p_iter_2_grad[iter];
  p_grad->fd_signed=*p_fd;
  DO_ASSERT(p_grad->fd_recorded=true);
  return;
}

uint32 draw_gradReferenceMaxIter(draw_gradReference *p_grad_ref) {
  return draw_coordToIterMaxIter(&p_grad_ref->p_coord_2_iters[0]);
}

draw_range draw_vertsTopAndBottomIdxs(const draw_vert verts[], const uint32 num_elements) {
  LOG_ASSERT(num_elements>0, "invalid num elements: %u", num_elements);
  draw_range result={ .start=0, .end=0 };
  for(uint32 idx=1; idx<num_elements; idx++) {
    if(verts[idx].y<verts[result.start].y) {
      result.start=idx;
    }
  }
  
  for(uint32 idx=1; idx<num_elements; idx++) {
    if(verts[idx].y>verts[result.end].y) {
      result.end=idx;
    }
  }

  return result;
}

draw_range draw_scanFillLogStartScan(draw_scanFillLog *p_f, const draw_vert verts[], const uint32 num_verts) {
  draw_range y_bnds_idxs=draw_vertsTopAndBottomIdxs(verts, num_verts);
  //LOG_INFO("y bnds idxs: start: %i end: %i", y_bnds_idxs.start, y_bnds_idxs.end);
  uint32 min_y_idx=y_bnds_idxs.start;
  sint32 start_y=MAX(verts[min_y_idx].y-(p_f->min_y+1), 0);
  
  if(p_f->size>start_y) {
    LOG_ASSERT(start_y>=0 && start_y<=p_f->size, "out of range start_y: %i", start_y);
    rtu_memSet(p_f->xs_pairs.p_y_2_starts+start_y, INT_MAX, p_f->size-start_y);
    rtu_memSet(p_f->xs_pairs.p_y_2_ends+start_y, INT_MIN, p_f->size-start_y);
    //rtu_memSet(p_f->xs_pairs.p_y_2_starts, INT_MAX, p_f->size);
    //rtu_memSet(p_f->xs_pairs.p_y_2_ends, INT_MIN, p_f->size);
  }
  
  return y_bnds_idxs;
}

draw_range draw_scanFillLogScan(draw_scanFillLog *p_f, const draw_vert verts[], const uint32 num_verts, draw_globals *p_globals) {
  LOG_ASSERT(num_verts>0, "invalid num vert %u", num_verts);
  draw_range result=draw_scanFillLogStartScan(p_f, verts, num_verts);

  uint32 last_vert=num_verts-1;
  for(uint32 idx=0; idx<last_vert; idx++) {
    draw_tailFaster(&verts[idx], &verts[idx+1], draw_dotScanRowLeftBnd, p_f, NULL, 0, p_globals);
    draw_tailFaster(&verts[idx], &verts[idx+1], draw_dotScanRowRightBnd, p_f, NULL, 0, p_globals);
  }
  draw_tailFaster(&verts[last_vert], &verts[0], draw_dotScanRowLeftBnd, p_f, NULL, 0, p_globals);
  draw_tailFaster(&verts[last_vert], &verts[0], draw_dotScanRowRightBnd, p_f, NULL, 0, p_globals);
  return result;
}

void draw_scanLogFill(draw_scanLog *p_log, const draw_vert verts[], const uint32 num_verts, draw_globals *p_globals, draw_onIterRowCb *p_row_renderer, void *p_iter_info) {
  LOG_ASSERT(num_verts>0, "invalid number of verts: %u", num_verts);
  if(p_log->f.size==0) {
    //LOG_INFO("tracking and size==0");

    DO_ASSERT(draw_canvas *p_canvas=&p_globals->canvas);
    DO_ASSERT(uint32 h=draw_canvasHeight(p_canvas));
    DO_ASSERT(uint32 w=draw_canvasWidth(p_canvas));
    LOG_ASSERT((num_verts==3 && (
				 (verts[0].x<0 && verts[1].x<0 && verts[2].x<0) ||
				 (verts[0].x>=w && verts[1].x>=w && verts[2].x>=w) ||
				 (verts[0].y<0 && verts[1].y<0 && verts[2].y<0) ||
				 (verts[0].y>=h && verts[1].y>=h && verts[2].y>=h)
				 
				 )) || (num_verts==4 && (
				 (verts[0].x<0 && verts[1].x<0 && verts[2].x<0 && verts[3].x<0) ||
				 (verts[0].x>=w && verts[1].x>=w && verts[2].x>=w && verts[3].x>=w) ||
				 (verts[0].y<0 && verts[1].y<0 && verts[2].y<0 && verts[3].y<0) ||
				 (verts[0].y>=h && verts[1].y>=h && verts[2].y>=h && verts[3].y>=h)

							 )), "size should not be zero");
    return;
  }

  draw_range y_bnds_idxs=draw_scanFillLogScan(&p_log->f, verts, num_verts, p_globals);
  uint32 min_y_idx=y_bnds_idxs.start, max_y_idx=y_bnds_idxs.end;

  sint32 start_y=verts[min_y_idx].y-p_log->f.min_y;
  sint32 end_y=verts[max_y_idx].y-p_log->f.min_y;
  //LOG_INFO("ys: %f, %f, %f", verts[0].y, verts[1].y, verts[2].y);
  sint32 y=verts[min_y_idx].y-(verts[min_y_idx].y<0);
  uint32 bottommost=(draw_canvasHeight(&p_globals->canvas)-p_log->f.min_y)-1;
  //LOG_ASSERT(IMPLIES(p_log->f.size==0, end_y<=start_y), "the loop below should be incur any iterations. start_y: %i end_y: %i", start_y, end_y);
  //LOG_INFO("y: %u min_y_idx: %u, max_y_idx: %u start_y idx: %i, end_y idx: %i", y, min_y_idx, max_y_idx, start_y, end_y);
  for(sint32 y_idx=start_y; y_idx<=end_y; y_idx++) {
    if(y_idx<0 || y_idx>bottommost) {
      y++;
      //LOG_INFO("tracking and skipping row. y: %i bottommost %u", y_idx, bottommost);
      continue;
    }
    if(p_log->f.xs_pairs.p_y_2_starts[y_idx]<=p_log->f.xs_pairs.p_y_2_ends[y_idx]) {
      (p_row_renderer)(p_log->p_b, p_log->f.xs_pairs.p_y_2_starts[y_idx], p_log->f.xs_pairs.p_y_2_ends[y_idx]+1, y, p_globals, p_iter_info);
    } else {
      //LOG_INFO("tracking and bound isn't set.");
      //one of our bounds haven't been set; how is this possible; shouldn't draw_scanFillLogScan have set all bounds we encounter here? TODO
      if(p_log->f.xs_pairs.p_y_2_ends[y_idx]!=INT_MIN) {
	(p_row_renderer)(p_log->p_b, 
		       p_log->f.xs_pairs.p_y_2_ends[y_idx], 
		       p_log->f.xs_pairs.p_y_2_ends[y_idx]+1, 
			 y, p_globals, p_iter_info);
      } else if(p_log->f.xs_pairs.p_y_2_starts[y_idx]!=INT_MAX) {
	(p_row_renderer)(p_log->p_b, 
		       p_log->f.xs_pairs.p_y_2_starts[y_idx], 
		       p_log->f.xs_pairs.p_y_2_starts[y_idx]+1, 
			 y, p_globals, p_iter_info);
      }
    }
    y++;
  }
}

void draw_smallTriNow(
		      draw_scanLog *p_log, 
		      draw_gradReference *p_grad_ref,
		      const draw_vert *p_0,
		      uint32 quad_iter,
		      draw_globals *p_globals
		      ) {
  uint32 q=(quad_iter & DRAW_PROXIMITY_QUADRANT_MASK) >> DRAW_PROXIMITY_ITER_BITS;
  uint32 iter=(quad_iter & DRAW_PROXIMITY_ITER_MASK);
  
  draw_grad *p_iter_2_grad=draw_gradReferenceGradPtr(p_grad_ref, q);
  draw_vert *p_center=&p_grad_ref->center;
  draw_grad *p_grad=&p_iter_2_grad[iter];
  draw_gradTranslatedInit(draw_gradReferenceTrans(p_grad_ref, q, iter), p_grad, p_center);
  
  draw_vert verts[]={ *p_center, p_grad_ref->last_pt.pt, *p_0 };

  draw_onIterRowCb *p_row_renderer=DRAW_ON_ITER_ROW_CB(draw_rowNewSegmentRangePerPix,
						       p_grad_ref);
  
  draw_scanLogFill(p_log, verts, 3, p_globals, p_row_renderer, &p_grad_ref->grads_if);
    
  p_grad_ref->last_iter=iter;
  draw_gradReferenceSetLastPt(p_grad_ref, p_0);
}

void draw_triNow(		       
	      draw_scanLog *p_log, 
	      draw_gradReference *p_grad_ref,
	      const draw_vert *p_0, 
	      uint32 quad_iter,
	      draw_globals *p_globals
				       ) {
  uint32 q=(quad_iter & DRAW_PROXIMITY_QUADRANT_MASK) >> DRAW_PROXIMITY_ITER_BITS;
  uint32 iter=(quad_iter & DRAW_PROXIMITY_ITER_MASK);
  
  draw_grad *p_iter_2_grad=draw_gradReferenceGradPtr(p_grad_ref, q);
  draw_vert *p_center=&p_grad_ref->center;
  draw_grad *p_grad=&p_iter_2_grad[iter];

  /*
  if(G_trace) {
    LOG_ERROR("grad %u %u. pos inc: %f, half delta: %f,%f, slope_recip: %f, fd_signed: %f,%f", q, iter, p_grad->default_pos_inc, p_grad->half_delta.x, p_grad->half_delta.y, p_grad->slope_recip, p_grad->fd_signed.x, p_grad->fd_signed.y);
  }
  */
  
  draw_gradTranslatedInit(draw_gradReferenceTrans(p_grad_ref, q, iter), p_grad, p_center);
  
  draw_vert verts[]={ *p_center, p_grad_ref->last_pt.pt, *p_0 };
  //LOG_INFO("verts[0]: %f,%f. verts[1]: %f,%f. verts[2]: %f,%f", verts[0].x, verts[0].y, verts[1].x, verts[1].y, verts[2].x, verts[2].y);
  draw_gradReferenceSetIterRange(p_grad_ref, q, iter);
  draw_onIterRowCb *p_row_renderer;

  if(p_grad->on_x) {
    p_row_renderer=draw_rowNewSegmentRangeOnX;
  } else {
    p_row_renderer=draw_rowNewSegmentRangeOnY;
  }
  
  draw_scanLogFill(p_log, verts, 3, p_globals, p_row_renderer, &p_grad_ref->grads_if);
    
  p_grad_ref->last_iter=iter;
  draw_gradReferenceSetLastPt(p_grad_ref, p_0);
}

void draw_smallTriFill(
		       draw_scanLog *p_log, 
		       draw_gradReference *p_ref,
		       const draw_vert *p_0, 
		       draw_globals *p_globals
		   ) {
  uint32 last_iter=p_ref->last_iter;
  //uint32 iter=draw_gradReferenceIter(p_ref, p_0);
  uint32 quad_iter=draw_scanBrushLogPt2Iter(p_log->p_b, p_ref, p_0);
  if(last_iter==UINT_MAX) {
    p_ref->last_iter=quad_iter;
    draw_gradReferenceSetLastPt(p_ref, p_0);
  }
  if(quad_iter==last_iter || last_iter==UINT_MAX) {
    return;
  }
  draw_smallTriNow(p_log, p_ref, p_0, quad_iter, p_globals);
}

void draw_triFill(
		       draw_scanLog *p_log, 
		       draw_gradReference *p_grad_ref,
		       const draw_vert *p_0, 
		       draw_globals *p_globals
		   ) {
  /*
  if(G_trace) {
    LOG_ERROR("p_0: %f,%f", p_0->x, p_0->y);
  }
  */
  uint32 last_iter=p_grad_ref->last_iter;
  uint32 quad_iter=(draw_gradReferenceIter(p_grad_ref, p_0) | (draw_gradReferenceQuadrant(p_grad_ref) << DRAW_PROXIMITY_ITER_BITS));
  if(last_iter==UINT_MAX) {
    p_grad_ref->last_iter=quad_iter;
    draw_gradReferenceSetLastPt(p_grad_ref, p_0);
  }
  if(quad_iter==last_iter || last_iter==UINT_MAX) {
    return;
  }
  draw_triNow(p_log, p_grad_ref, p_0, quad_iter, p_globals);
}

void draw_smallTriFillCb(
		       draw_scanLog *p_log, 
		       draw_gradReference *p_grad_ref,
		       const draw_vert *p_0, 
		       const draw_vert *p_fd, 
		       const uint32 other_iter,
		       draw_globals *p_globals
		   ) {
  draw_smallTriFill(p_log, p_grad_ref, p_0, p_globals);
}

void draw_triFillCb(
		       draw_scanLog *p_log, 
		       draw_gradReference *p_grad_ref,
		       const draw_vert *p_0, 
		       const draw_vert *p_fd, 
		       const uint32 other_iter,
		       draw_globals *p_globals
		   ) {
  draw_triFill(p_log, p_grad_ref, p_0, p_globals);
}

bool draw_fillIter(draw_grad *p_grad, draw_gradTranslated *p_grad_trans, const draw_strokeWidth *p_w, const draw_globals *p_globals) {
  draw_gradInitFill(p_grad, p_w, p_globals);
  return draw_gradTranslatedFill(p_grad_trans, p_grad, &p_grad->mid);
}

void draw_coreRender(draw_scanLog *p_log, draw_grads *p_grad_agg, uint32 iter, draw_globals *p_globals) {
  draw_grad *p_grad=&p_grad_agg->p_iter_2_grad[iter];
  if(iter>0) {
    uint32 last_iter=iter-1;
    draw_grad *p_last_grad=&p_grad_agg->p_iter_2_grad[last_iter];
    draw_vert verts[4]={ 
      draw_diff(&p_last_grad->mid, &p_last_grad->half_delta),
      draw_add(&p_last_grad->mid, &p_last_grad->half_delta),
      draw_add(&p_grad->mid, &p_grad->half_delta),
      draw_diff(&p_grad->mid, &p_grad->half_delta),
    };
    /*
    if((p_grad->mid.y<30 || p_last_grad->mid.y<30) && (verts[0].y<0 || verts[1].y<0 || verts[2].y<0 || verts[3].y<0)) {
      G_track=true;
    }
    */

    //draw_dot(verts[2].x, verts[2].y, 0xffffffff, p_globals);
    //draw_dot(verts[3].x, verts[3].y, 0xffffffff, p_globals);

    if(p_log->f.highly_acute && draw_scanLogCheckAngle(p_log, p_last_grad, p_grad, p_globals)>PI_OVER_EIGHT && draw_euclideanDistSquared(&p_grad->mid, &p_last_grad->mid)<=1) {
      /* We filter out instance based on distance as where 2
       * successive draw_grads have a gap between their mid vertices,
       * as just rendering a blot at one mid (instead of a quad
       * between the two) will result in a gap at edges.
       *
       * Besides where there's a significant distance between
       * successive draw_grads it's usually indicative of a
       * straightish line and therefore near-perpendicular draw_grads
       * wouldn't cause a problem. However I would question how it is
       * possible that two successive draw_grads exists that are near
       * perpendicular, but are also on a straight line. TODO
       * 
       */
      
      draw_blotContinue(&p_grad->mid, &p_grad->fd_signed, p_log->p_b->w.breadth, p_log->p_b->w.blur_width, p_log->p_b->col, p_globals);
      //LOG_INFO("tracking and blotting.");
      return;
    }

    draw_vert ordered_verts[4]={ verts[0], verts[3], verts[2], verts[1] };
    if(p_last_grad->on_x!=p_grad->on_x && SGN(p_grad->fd_signed.x*p_grad->fd_signed.y)==1) {
      ordered_verts[1]=verts[2];
      ordered_verts[2]=verts[3];
      ordered_verts[3]=verts[1];
    }

    draw_onIterRowCb *p_row_renderer;
    if(p_grad->on_x) {
      p_row_renderer=draw_rowNewSegmentRangeOnX;
    } else {
      p_row_renderer=draw_rowNewSegmentRangeOnY;
    }
    draw_gradsSetIterRange(p_grad_agg, UINT_MAX, iter);
    
    draw_scanLogFill(p_log, ordered_verts, 4, p_globals, p_row_renderer, &p_grad_agg->grads_if);
  } else {
    p_log->f.last_ang=draw_gradATan(p_grad, p_globals);
  }
  
  draw_gradMarkDirty(p_grad, draw_gradsTrans(p_grad_agg, UINT_MAX, iter), &p_globals->canvas);
}

void draw_initSmallGrad(
		       draw_scanLog *p_log, 
		       draw_grads *p_grad_agg,
		       const draw_vert *p_0, 
		       const draw_vert *p_fd, 
		       const uint32 iter,
		       draw_globals *p_globals
		       ) {
  draw_recordFD(p_log, p_grad_agg, p_0, p_fd, iter, p_globals);
  draw_grad *p_grad=&p_grad_agg->p_iter_2_grad[iter];
  //p_grad->mid=*p_0;
  draw_gradInitFill(p_grad, &p_log->p_b->w, p_globals);
  p_grad->p_landmark=rtu_memAlloc(sizeof(draw_landmark));
  LOG_ASSERT(p_grad->p_landmark, "landmark not malloced");
  if(p_grad->p_landmark) {
    draw_landmarkInit(p_grad->p_landmark, p_0);
  }
}

void draw_initGrad(
		       draw_scanLog *p_log, 
		       draw_grads *p_grad_agg,
		       const draw_vert *p_0, 
		       const draw_vert *p_fd, 
		       const uint32 iter,
		       draw_globals *p_globals
		       ) {
  draw_recordFD(p_log, p_grad_agg, p_0, p_fd, iter, p_globals);
  draw_grad *p_grad=&p_grad_agg->p_iter_2_grad[iter];
  p_grad->mid=*p_0;
  draw_gradInitFill(p_grad, &p_log->p_b->w, p_globals);
}

void draw_coreInitGrad(
		       draw_scanLog *p_log, 
		       draw_grads *p_grad_agg,
		       const draw_vert *p_0, 
		       const draw_vert *p_fd, 
		       const uint32 iter,
		       draw_globals *p_globals
		       ) {
  draw_initGrad(p_log, p_grad_agg, p_0, p_fd, iter, p_globals);
  LOG_ASSERT(p_globals->null_vert.x==0 && p_globals->null_vert.y==0, "someone altered the null vert");
  draw_grad *p_grad=&p_grad_agg->p_iter_2_grad[iter];
  draw_gradTranslatedFill(draw_gradsTrans(p_grad_agg, UINT_MAX, iter), p_grad, &p_grad->mid);
  draw_coreRender(p_log, p_grad_agg, iter, p_globals);

}

draw_vert draw_bezNext(draw_bez *p_bez, const draw_globals *p_globals) {
    LOG_ASSERT(p_bez->f_x_t.x<INT_MAX, "f_x_t.x too large: %f", p_bez->f_x_t.x);
    LOG_ASSERT(p_bez->f_x_t.y<INT_MAX, "f_x_t.y too large: %f", p_bez->f_x_t.y);

    p_bez->f_x_t.x=draw_plotBezUpdateFXT(p_bez->f_x_t.x, p_bez->fd_times_t.x, p_bez->c_0.x);
    p_bez->f_x_t.y=draw_plotBezUpdateFXT(p_bez->f_x_t.y, p_bez->fd_times_t.y, p_bez->c_0.y);
    
    p_bez->fd_times_t.x=draw_plotBezUpdateFDTimesT(p_bez->fd_times_t.x, p_bez->c_1.x);
    p_bez->fd_times_t.y=draw_plotBezUpdateFDTimesT(p_bez->fd_times_t.y, p_bez->c_1.y);
    
    DO_ASSERT(p_bez->t=MIN(p_bez->t+p_bez->step, 1));
    DO_ASSERT(draw_vert real_f_x_t=draw_bezierQuad(p_bez->t, p_bez->p_points, p_globals));
    LOG_ASSERT(rtu_similar(real_f_x_t.x, p_bez->f_x_t.x, 0.25) && rtu_similar(real_f_x_t.y, p_bez->f_x_t.y, 0.25), "f_x_t mismatch at t: %f. f_x_t: %f, %f. real: %f, %f", p_bez->t, p_bez->f_x_t.x, p_bez->f_x_t.y, real_f_x_t.x, real_f_x_t.y);

    return p_bez->f_x_t;
}

draw_vert *draw_bezFD(draw_bez *p_bez) {
  return draw_quadNextFD(&p_bez->quad);
}

draw_vert *draw_bezFD_0(draw_bez *p_bez) {
  return &p_bez->fd_0;
}

draw_vert draw_bezFXT(draw_bez *p_bez) {
  return p_bez->f_x_t;
}

draw_bez draw_bezInterface(void) {
  draw_bez bez={
    .curve_if={
      .p_init_cb=DRAW_CURVE_IF_INIT_CB_ARG(draw_bezInit, &bez),
      .p_fxt_cb=DRAW_CURVE_IF_FXT_CB_ARG(draw_bezFXT, &bez),
      .p_fd_cb=DRAW_CURVE_IF_FD_CB_ARG(draw_bezFD, &bez),
      .p_next_cb=DRAW_CURVE_IF_NEXT_CB_ARG(draw_bezNext, &bez),
      .p_fd_0_cb=DRAW_CURVE_IF_FD_0_CB_ARG(draw_bezFD_0, &bez)
    }
  };
  return bez;
}

void draw_bezInit(draw_bez *p_bez, const float32 step, const draw_vert *p_0, const draw_vert *p_1, const draw_vert *p_2, const draw_globals *p_globals) {
  p_bez->p_points[0]=p_0;
  p_bez->p_points[1]=p_1;
  p_bez->p_points[2]=p_2;

  draw_quadInit(&p_bez->quad, p_bez->p_points);

  DO_ASSERT(p_bez->t=0);
  DO_ASSERT(p_bez->step=step);

  draw_vert f_0=*p_0;
  p_bez->fd_0.x=draw_plotBezInitFD_0(p_0->x, p_1->x);
  p_bez->fd_0.y=draw_plotBezInitFD_0(p_0->y, p_1->y);

  draw_vert fdd={
    .x=draw_plotBezInitFDD(p_0->x, p_1->x, p_2->x), 
    .y=draw_plotBezInitFDD(p_0->y, p_1->y, p_2->y)
  };
  p_bez->c_0.x=draw_plotBezInitC_0(fdd.x, step);
  p_bez->c_0.y=draw_plotBezInitC_0(fdd.y, step);
  p_bez->c_1.x=draw_plotBezInitC_1(p_0->x, p_1->x, p_2->x, step);
  p_bez->c_1.y=draw_plotBezInitC_1(p_0->y, p_1->y, p_2->y, step);
  
  p_bez->f_x_t=f_0;

  p_bez->fd_times_t.x=draw_plotBezInitFDTimesT(p_bez->fd_0.x, step);
  p_bez->fd_times_t.y=draw_plotBezInitFDTimesT(p_bez->fd_0.y, step);

  draw_quadStartFDGen(&p_bez->quad, step);
}

sint32 draw_curveIfPlot(
			draw_curveIf *p_curve, 
			draw_scanLog *p_log, 
			draw_gradsIf *p_grad_agg,
			const float32 step, 
			draw_vert *p_pts, 
			draw_onPtCb *p_on_pt_cb, 
			draw_onIterCb *p_on_iter_cb, 
			draw_globals *p_globals
			) {
  draw_onSegCb *p_on_seg_cb;
  draw_vert *p_0=&p_pts[0], *p_1=&p_pts[1], *p_2=&p_pts[2];
  p_0=&p_pts[0];
  p_2=&p_pts[2];
  
  p_on_seg_cb=draw_tailFaster;

  (p_curve->p_init_cb)(p_curve, step, p_0, p_1, p_2, p_globals);
  //LOG_INFO("args: p_0: %f, %f \tp_1: %f, %f \tp_2: %f, %f", p_0->x, p_0->y, p_1->x, p_1->y, p_2->x, p_2->y);
  draw_vert f_x_t=p_curve->p_fxt_cb(p_curve), last=p_curve->p_fxt_cb(p_curve);
  uint32 row_start_iter=0;

  //LOG_INFO("start iter %u", p_log->row_start_iter);
  if(p_on_iter_cb) {
    (*p_on_iter_cb)(p_log, p_grad_agg, &f_x_t, p_curve->p_fd_cb(p_curve), row_start_iter, p_globals);
  }
  
  uint32 cur_dir;
  draw_setDDir(&p_globals->draw_origin, p_curve->p_fd_0_cb(p_curve), &cur_dir, p_globals);

  sint32 max_y[2]={0, last.y}; //max will be stored in max[1], max[0] won't be read, ever
  uint32 next_iter=row_start_iter+1;
  uint32 max_iter=(p_grad_agg->p_max_iter_cb)(p_grad_agg);
  //LOG_INFO("draw_curveIfPlot. max_iter: %u", max_iter);
  
  for(; next_iter<max_iter;) {
    //LOG_INFO("next_iter: %u", next_iter);
    row_start_iter=next_iter++;
    LOG_ASSERT(row_start_iter<=max_iter, "invalid row_start_iter: %u", row_start_iter);
    //LOG_INFO("iter %u", p_log->row_start_iter);
    //LOG_INFO("dir: 0x%x fill dir: 0x%x pos: %f, %f", cur_dir, fill_dir, f_x_t.x, f_x_t.y);

    f_x_t=p_curve->p_next_cb(p_curve, p_globals);
    if(p_on_iter_cb) {
      (*p_on_iter_cb)(p_log, p_grad_agg, &f_x_t, p_curve->p_fd_cb(p_curve), row_start_iter, p_globals);
    }

    if((*p_on_seg_cb)(&last, &f_x_t, p_on_pt_cb, &p_log->f, p_grad_agg, row_start_iter, p_globals)) {
      draw_setDir(&last, &f_x_t, &cur_dir, p_globals);
      last=f_x_t;
    } else {
      draw_setDir(&last, &f_x_t, &cur_dir, p_globals);
    }

    max_y[f_x_t.y>max_y[1]]=f_x_t.y;
  }

  //next bit is not done inside loop as we wish to use p_2 as the final point
  row_start_iter=next_iter;
  LOG_ASSERT(row_start_iter==max_iter, "row start iter: %u max iter %u", row_start_iter, max_iter);

  f_x_t=p_curve->p_next_cb(p_curve, p_globals);
  if(p_on_iter_cb) {
    (*p_on_iter_cb)(p_log, p_grad_agg, p_2, p_curve->p_fd_cb(p_curve), row_start_iter, p_globals);
  }
  if((*p_on_seg_cb)(&last, p_2, p_on_pt_cb, &p_log->f, p_grad_agg, row_start_iter, p_globals)) {
    draw_setDir(&last, p_2, &cur_dir, p_globals);    
    last=*p_2;
  } else {
    draw_setDir(&last, p_2, &cur_dir, p_globals);    
  }

  max_y[f_x_t.y>max_y[1]]=f_x_t.y;
  return max_y[1];
}

sint32 draw_plotBez(const float32 step, draw_vert *p_pts, 
		    draw_onPtCb *p_on_pt_cb, 
		    draw_onIterCb *p_on_iter_cb, 
		    draw_scanLog *p_log, draw_gradsIf *p_grad_agg,
		    draw_globals *p_globals) {
  draw_bez curve=draw_bezInterface();
  LOG_ASSERT((void *)&curve==(void *)&curve.curve_if, "first field in draw_bez instance must be draw_curveIf");
  //LOG_INFO("verts: %f, %f - %f, %f - %f, %f", p_pts[0].x, p_pts[0].y, p_pts[1].x, p_pts[1].y, p_pts[2].x, p_pts[2].y);
  return draw_curveIfPlot(&curve.curve_if, p_log, p_grad_agg, step, p_pts, p_on_pt_cb, p_on_iter_cb, p_globals);
}

draw_vert draw_solveSimultaneous(float32 ma, float32 ca_lt, float32 mb, float32 cb_lt) {
  draw_vert result;
  LOG_ASSERT(mb-ma!=0, "slope diff of 0 %f, %f", mb, ma);
  result.x=draw_mCs2X(ma, mb, ca_lt, cb_lt);
  result.y=draw_mXC2Y(ma, result.x, ca_lt);
  return result;
}

void draw_solveSimultaneousRect(float32 ma, float32 ca_lt, float32 ca_rb, float32 mb, float32 cb_lt, float32 cb_rb, draw_rect *p_pts) {
  LOG_ASSERT(mb-ma!=0, "slope diff of 0 %f, %f", mb, ma);
  p_pts->lt.x=draw_mCs2X(ma, mb, ca_lt, cb_lt);
  p_pts->rb.x=draw_mCs2X(ma, mb, ca_rb, cb_rb);

  p_pts->lt.y=draw_mXC2Y(ma, p_pts->lt.x, ca_lt);
  p_pts->rb.y=draw_mXC2Y(ma, p_pts->rb.x, ca_rb);
}

sint32 draw_tailArr(const uint32 start_iter, const float32 step, draw_vert *p_pts, 
		    draw_onPtCb * p_on_pt_cb, 
		    draw_onIterCb *p_on_iter_cb,
		    draw_scanLog *p_log, draw_gradsIf *p_grad_agg,
		    draw_globals *p_globals) {
  //draw_scanLogStartLineEdge(p_log, start_iter, &p_pts[0], &p_pts[1], p_globals);
  //specifically added guard to reduce expense of cap rendering -- but removed it after it caused bug where only first pixel of bottom row in a vertical stroke was rendered
  draw_tail(&p_pts[0], &p_pts[1], p_on_pt_cb, &p_log->f, p_grad_agg, start_iter, p_globals);
  return MAX(p_pts[0].y, p_pts[1].y);
}

void draw_strokeWidthInit(draw_strokeWidth *p_w, const float32 breadth, const float32 blur_width) {
  p_w->breadth=breadth;
  float32 width=draw_savedWidth2RenderedWidth(breadth, blur_width);
  p_w->width=width;
  p_w->width_squared=p_w->width*p_w->width;
  p_w->half_width=width*0.5;
  p_w->width_recip=1/(float32)width;
  p_w->blur_width=blur_width;
  p_w->half_blur_width=blur_width * 0.5; //refers to blur on one side of stroke only
  //LOG_INFO("breadth: %u width: %u half_blur_width: %u", breadth, p_w->width, p_w->half_blur_width);
  LOG_ASSERT(2*p_w->half_blur_width<=p_w->width, "width too small: %u. half blur: %u", p_w->width, p_w->half_blur_width);
  p_w->half_blur_width_recip=1/p_w->half_blur_width;
  p_w->blurred_prop=p_w->half_blur_width/p_w->width;
  p_w->blurred_prop_recip=p_w->width/p_w->half_blur_width;
}

void draw_gradsDestroy(draw_grads *p_grad_agg) {
  draw_gradTranslatedsDestroy(&p_grad_agg->trans);
  p_grad_agg->max_iter=0;
}

void draw_proxInit(draw_prox *p_prox, float32 width_f) {
  uint32 width=CEIL32(width_f);
  uint32 end_adjustment=2; //+1 due to dotScanRowRightBnd adding 1 at end and +1 because (uint32)(0.99+1)=((uint32)0.99)+1
  p_prox->span=width+(DRAW_PROXIMITY_ORIGIN_OFFSET<<1)+end_adjustment; //+2 for rounding down down either at start or end
  DO_ASSERT(float32 c=p_prox->span*0.5);

  //(width+1+2)*0.5+(width>>1)+1<=(width+1+2)
  //(width+1)*0.5+2+(width>>1)<=width+1+2
  //(width+1)*0.5+(width>>1)<=width+1
  //width>>1+width>>1<=width+1 and width>>1+1+(width>>1)<=width-1
  //width-1<width+1 and width<=width-1

  //(width>>1)+1-(width+1+2)*0.5>=0
  //width>>1+2-(width+1)*0.5>=0
  //width>>1+2-width>>1>=0 or width>>1+2-width>>1+1>=0
  //2 >=0 or width>>1+2-width>>1+1>=0

  DO_ASSERT(draw_vert cen={ .x=c, .y=c });
  uint32 span_sq=p_prox->span*p_prox->span;
  uint32 size=span_sq*sizeof(uint32);
  p_prox->p_xy_2_nearest_landmark=rtu_memAlloc(size);
  p_prox->p_xy_2_iter=rtu_memAlloc(size);
  if(p_prox->p_xy_2_nearest_landmark && p_prox->p_xy_2_iter) {
    rtu_memZero(p_prox->p_xy_2_nearest_landmark, size);
    rtu_memZero(p_prox->p_xy_2_iter, size);
    p_prox->nearest_landmark_size=size;
    p_prox->rel_origin=(width>>1)+DRAW_PROXIMITY_ORIGIN_OFFSET;
    DO_ASSERT(0<p_prox->span?draw_proxOffset(p_prox, ((sint32)cen.y)+1, ((sint32)c)-((width>>1)+end_adjustment)):0);
    DO_ASSERT(0<p_prox->span?draw_proxOffset(p_prox, ((sint32)cen.y)+1, ((sint32)c)-((width>>1)+end_adjustment)):0);
    DO_ASSERT(p_prox->span-1>=0?draw_proxOffset(p_prox, ((sint32)cen.x), ((sint32)c)+1-(width>>1)):0);
    DO_ASSERT(p_prox->span-1>=0?draw_proxOffset(p_prox, ((sint32)cen.y), ((sint32)c)+1-(width>>1)):0);
    uint32 half_span=draw_proxHalfSpan(p_prox, width, true);
    p_prox->max_dist_squared=(half_span*half_span*DRAW_PROXIMITY_FIXED_POINT*DRAW_PROXIMITY_FIXED_POINT)<<1;
  } else {
    if(p_prox->p_xy_2_nearest_landmark) {
      rtu_memFree(p_prox->p_xy_2_nearest_landmark);
      p_prox->p_xy_2_nearest_landmark=NULL;
    }
    if(p_prox->p_xy_2_iter) {
      LOG_INFO("init (freeing due to failed malloc of p_xy_2_nearest_landmark). p_xy_2_iter: %p", p_prox->p_xy_2_iter);
      rtu_memFree(p_prox->p_xy_2_iter);
      p_prox->p_xy_2_iter=NULL;
    }
    LOG_ASSERT(p_prox->max_dist_squared<=DRAW_PROXIMITY_MASK, "out of range max_dist_squared: %u", p_prox->max_dist_squared);
    p_prox->max_dist_squared=0;
    p_prox->rel_origin=0;
  }
  DO_ASSERT(p_prox->initialised=true);
}

static sint8 *draw_test_proxInit(void) {
  draw_prox prox;
  DO_ASSERT(prox.initialised=false);
  draw_proxInit(&prox, 2);
  uint32 offset_0=((sint32)0-(sint32)1)+prox.rel_origin;
  mu_assert("out of range offset_0", offset_0<prox.span);
  uint32 offset_1=((sint32)1-(sint32)1)+prox.rel_origin;
  mu_assert("out of range offset_1", offset_1<prox.span);
  mu_assert("out of range offset_2", offset_0<prox.span);
  mu_assert("2 proxmity values wont' fit in draw_prox.p_xy_2_nearest_landmark element when each is squared", ((DRAW_TRI_THRESH*DRAW_TRI_THRESH*DRAW_PROXIMITY_FIXED_POINT*DRAW_PROXIMITY_FIXED_POINT*2)<(1<<16)));
  draw_proxDestroy(&prox);
  return 0;
}

void draw_proxDestroy(draw_prox *p_prox) {
  DO_ASSERT(p_prox->initialised=false);
  if(p_prox->p_xy_2_iter) {
    rtu_memFree(p_prox->p_xy_2_iter);
    p_prox->p_xy_2_iter=NULL;
  }
  if(p_prox->p_xy_2_nearest_landmark) {
    rtu_memFree(p_prox->p_xy_2_nearest_landmark);
    p_prox->p_xy_2_nearest_landmark=NULL;
  }
}

void draw_scanBrushLogInit(draw_scanBrushLog *p_b, const float32 breadth, const float32 blur_width, const uint32 col, draw_canvas *p_canvas, const bool recalc_scaling) {
  p_b->col=col;
  p_b->rgb=col & DRAW_OPACITY_INVERSE_MASK;
  p_b->opacity=col & DRAW_OPACITY_MASK;
  p_b->opacity_frac=((float32)(col>>DRAW_OPACITY_SHIFT))/255;
  DO_ASSERT(p_b->proximity.initialised=false);

  uint32 scaling_idx;
  if(recalc_scaling) {
    scaling_idx=draw_canvasSetAndGetScalingIdx(p_canvas, p_b->opacity>>DRAW_OPACITY_SHIFT, blur_width);
  } else {
    scaling_idx=0;
  }
  p_b->scaling_factor=p_canvas->std_scaling_factors[scaling_idx];
  p_b->mag_factor=p_canvas->std_mag_factors[scaling_idx];
  //LOG_INFO("mag: %u breadth: %f blur: %f", p_b->mag_factor, breadth, blur_width);
  draw_strokeWidthInit(&p_b->w, breadth*p_b->scaling_factor, blur_width*p_b->scaling_factor);
  
  if(p_b->w.width<DRAW_TRI_THRESH) {
    draw_proxInit(&p_b->proximity, p_b->w.width);
  }
  draw_gradConstsInit(&p_b->grad_consts, &p_b->w);
}

uint32 draw_scanBrushLogMagFactor(const draw_scanBrushLog *p_b) {
  return p_b->mag_factor;
}

float32 draw_scanBrushLogScaleFactor(const draw_scanBrushLog *p_b) {
  return p_b->scaling_factor;
}

uint32 draw_scanBrushLogPt2Iter(const draw_scanBrushLog *p_b, draw_gradReference *p_ref, const draw_vert *p_0) {
  const draw_prox *p_prox=&p_b->proximity;

  /* When *p_0 is very close to p_ref->center the dimension with the smaller
     distance between *p_0 and p_ref->center can still cross a pixel boundary
     while the other dimension with larger distance to the center can still
     share the same dimensional value as the center, resulting
     in an incorrect q / iter pair being chosen. Therefore we try to extend
     the line from p_ref->center to *p_0 to twice its length in order
     to give the per-pixel iter selection a reasonable chance of working.
  */
  uint32 half_span=draw_proxHalfSpan(p_prox, p_b->w.width, false);
  sint32 rounded_x_offset=p_0->x<(sint32)p_0->x, rounded_y_offset=p_0->y<(sint32)p_0->y; //casting to sint32 rounds towards 0, so if negative we subtract 1 to ensure cons
  //sint32 rounded_x_offset=0, rounded_y_offset=0; //casting to sint32 rounds towards 0, so if negative we subtract 1 to ensure cons
  draw_vert rounded={ .x=((sint32)p_0->x)-rounded_x_offset, .y=((sint32)p_0->y)-rounded_y_offset };
  draw_vert delta=draw_diff(&rounded, &p_ref->center);
  //draw_vert delta=draw_diff(&rounded, &p_prox->cen);
  draw_vert abs_delta={ .x=ABSF(delta.x), .y=ABSF(delta.y) };
  float32 largest_delta=MAX(abs_delta.x, abs_delta.y);
  draw_vert *p_final_delta;
  draw_vert final_delta;
  if(largest_delta*2<=half_span) {
    final_delta=draw_by(&delta, 2);
    p_final_delta=&final_delta;
  } else {
    p_final_delta=&delta;
  }
  draw_vert point=draw_add(&p_ref->center, p_final_delta);
  //draw_vert point=draw_add(&p_prox->cen, p_final_delta);

  rounded_x_offset=point.x<(sint32)point.x; //casting to sint32 rounds towards 0, so if negative we subtract 1 to ensure cons
  rounded_y_offset=point.y<(sint32)point.y;
  //LOG_ASSERT(IMPLIES(rounded_x_offset==1, point.x<0), "If the existing value of rounded_x_offset==1 so will an updated one");
  //LOG_ASSERT(IMPLIES(rounded_y_offset==1, point.y<0), "If the existing value of rounded_y_offset==1 so will an updated one");
  sint32 y_coord=((sint32)point.y)-rounded_y_offset;
  //LOG_INFO("point: %f, %f. final delta: %f, %f", point.x, point.y, p_final_delta->x, p_final_delta->y);
  uint32 y_offset_idx=draw_scanBrushLogRowOffset(p_b, &p_ref->center, y_coord);
  //uint32 y_offset_idx=draw_scanBrushLogRowOffset(p_b, &p_prox->cen, (uint32)point.y);
  uint32 offset_idx=draw_proxOffsetIdx(p_prox, p_ref->center.x, ((sint32)point.x)-rounded_x_offset, y_offset_idx);
  //uint32 offset_idx=draw_proxOffsetIdx(p_prox, p_prox->cen.x, (uint32)point.x, y_offset_idx);
  
  uint32 quad_iter=DRAW_PROXIMITY_EXTRACT(0, p_prox->p_xy_2_iter[offset_idx]);
  return quad_iter;
}

void draw_scanBrushLogDestroy(draw_scanBrushLog *p_b) {
  if(p_b->w.width<DRAW_TRI_THRESH) {
    draw_proxDestroy(&p_b->proximity);
  }    
}

void draw_scanFillLogDestroy(draw_scanFillLog *p_f) {
  if(p_f->xs_pairs.p_y_2_ends) {
    rtu_memFree(p_f->xs_pairs.p_y_2_ends);
    p_f->xs_pairs.p_y_2_ends=NULL;
  }
  if(p_f->xs_pairs.p_y_2_starts) {
    rtu_memFree(p_f->xs_pairs.p_y_2_starts);
    p_f->xs_pairs.p_y_2_starts=NULL;
  }
}

void draw_scanLogDestroy(draw_scanLog *p_log) {
  draw_scanFillLogDestroy(&p_log->f);
}

float32 draw_vertAng(const draw_vert *p_0, const draw_vert *p_1) {
  draw_vert diff=draw_diff(p_1, p_0);
  float32 ang;
  if(diff.x==0) {
    if(diff.y<0) {
      ang=M_PI;
    } else {
      ang=0;
    }
  } else {
    float32 slope=diff.y/diff.x;
    ang=M_PI_2-ATAN32(slope);
    LOG_ASSERT(0<=ang && ang<=M_PI, "out of range ang: %f", ang);
    if(diff.x<0) {
      ang+=M_PI;
    }
  }
  LOG_ASSERT(0<=ang && ang<=TWO_PI, "out of range final ang: %f", ang);
  return ang;
}

float32 draw_vertAngBetween(const draw_vert *p_0, const draw_vert *p_1, const draw_vert *p_2) {
  float32 angs[2]={ draw_vertAng(p_1, p_0), draw_vertAng(p_1, p_2) };
  uint32 larger_idx=angs[0]<angs[1];
  uint32 smaller_idx=1-larger_idx;
  float32 fst_ang=angs[larger_idx]-angs[smaller_idx];
  float32 snd_ang=TWO_PI+angs[smaller_idx]-angs[larger_idx];
  return MIN(fst_ang, snd_ang);
}

void draw_renderCoreInit(draw_renderCore *p_render_core, const draw_vert *p_0, const draw_vert *p_1, const draw_vert *p_2, draw_strokeWidth *p_w, const float32 ang, const uint32 max_iter, const draw_canvas *p_canvas) {
  p_render_core->spine[0]=*p_0;
  p_render_core->spine[1]=*p_1;
  p_render_core->spine[2]=*p_2;
  p_render_core->p_w=p_w;

  p_render_core->max_iter=max_iter;
  p_render_core->ang=ang;
  p_render_core->step=1/(float32)max_iter;
  float32 real_min_y=MIN(MIN(p_0->y, p_1->y), p_2->y)-p_render_core->p_w->half_width;
  real_min_y=MAX(real_min_y,0);
  p_render_core->min_y=((sint32)real_min_y)-1;  //sometimes due to rounding error we can get a y value less than real_min_y when rendering our bezier. So we give ourselves one more row to play with by making min_y smaller by -1 than it should really need to be
  float32 real_max_y=MAX(MAX(p_0->y, p_1->y), p_2->y)+p_render_core->p_w->half_width;
  real_max_y=MIN(real_max_y, p_canvas->h-1);
  if(real_min_y>real_max_y || real_min_y>=p_canvas->h) {
    //either the min and max are both less than 0 or are both >= canvas.h
    p_render_core->height=0;
  } else {
    p_render_core->height=(2+((sint32)real_max_y)-p_render_core->min_y);
  }
  
  //LOG_ASSERT(p_render_core->real_max_y<100000, "out of range real_max_y. REMOVE");
}

uint32 draw_renderCoreMaxIter(const draw_renderCore *p_render_core) {
  return p_render_core->max_iter;
}

void draw_renderCoreDraw(draw_renderCore *p_core, draw_scanLog *p_log, draw_grads *p_grad_agg, draw_globals *p_globals) {
  /*
  if(p_grad_agg->max_iter==8) {
    LOG_INFO("checking %p idx 8. %u", &(p_grad_agg->trans.p_iter_2_grad_trans[8]), p_grad_agg->trans.p_iter_2_grad_trans[8].idx);
  }
  */
  draw_plotBez(p_core->step, p_core->spine, NULL, DRAW_GRADS_IF_ON_ITER_CB_ARG(draw_coreInitGrad, p_grad_agg), p_log, &p_grad_agg->grads_if, p_globals);
  LOG_ASSERT(p_grad_agg->max_iter>0, "invalid no of iters");
  draw_coreRender(p_log, p_grad_agg, p_grad_agg->max_iter, p_globals);
}

uint32 draw_calcMaxIter(const float32 width, const float32 ang) {
  //ang is the angle in radians between p_0, p_1 and p_2 of the quadratic bezier
  LOG_ASSERT((ang<=M_PI || rtu_similar(ang, M_PI, 0.01)), "ang is too large: %f", ang);
  float32 ang_multiplier=1+M_PI-ang;
  float32 multiplier=LOG32(ang_multiplier*0.8)*LOG32(width*10)*15;
  uint32 result=MAX(multiplier, 3);
  //LOG_INFO("width: %u ang: %f max_iter: %u", width, ang, result);
  return result;
}

draw_grads draw_gradsInterface(void) {
  draw_grads grad_agg={
    .grads_if={
      .p_max_iter_cb=DRAW_GRADS_IF_MAX_ITER_CB_ARG(draw_gradsMaxIter, &grad_agg),
      .p_grad_ptr_cb=DRAW_GRADS_IF_GRAD_PTR_CB_ARG(draw_gradsGradPtr, &grad_agg),
      .p_iter_range_cb=DRAW_GRADS_IF_ITER_RANGE_CB_ARG(draw_gradsIterRange, &grad_agg),
      .p_trans_cb=DRAW_GRADS_IF_TRANS_CB_ARG(draw_gradsTrans, &grad_agg)
    }
  };
  return grad_agg;
}

void draw_gradsInit(draw_grads *p_grad_agg, draw_grad *p_iter_2_grad, const draw_renderCore *p_render_core) {
  p_grad_agg->max_iter=draw_renderCoreMaxIter(p_render_core);
  //DO_ASSERT(p_grad_agg->range.iter_inc=0);
  DO_ASSERT(p_grad_agg->trans.p_iter_2_grad_trans=NULL);
  p_grad_agg->p_iter_2_grad=p_iter_2_grad;
  draw_gradTranslatedsInit(&p_grad_agg->trans, p_grad_agg->max_iter, p_grad_agg->max_iter+1);
}

void draw_gradsSetIterRange(draw_grads *p_grad_agg, const uint32 q, const uint32 iter) {
  draw_iterRangeInit(&p_grad_agg->range, q, iter);
}

draw_iterRange draw_gradsIterRange(draw_grads *p_grad_agg) {
  return p_grad_agg->range;
}

uint32 draw_gradsMaxIter(draw_grads *p_grad_agg) {
  return p_grad_agg->max_iter;
}

draw_grad *draw_gradsGradPtr(draw_grads *p_grad_agg, const uint32 q) {
  LOG_ASSERT(q==UINT_MAX, "draw_grads instance only ever has draw_grads from one quadrant");
  return p_grad_agg->p_iter_2_grad;
}

void draw_gradsSetMids(draw_grads *p_grad_agg, const draw_vert *p_0) {
  for(uint32 idx=0; idx<=p_grad_agg->max_iter; idx++) {
    DO_ASSERT(p_grad_agg->p_iter_2_grad[idx].idx=idx);
    DO_ASSERT(p_grad_agg->p_iter_2_grad[idx].fd_recorded=false);
    p_grad_agg->p_iter_2_grad[idx].mid=*p_0;
  }  
}

void draw_gradMarkDirty(const draw_grad *p_grad, const draw_gradTranslated *p_grad_trans, draw_canvas *p_canvas) {
  LOG_ASSERT(p_grad->idx==p_grad_trans->idx, "iter mismatch: %u, %u", p_grad->idx, p_grad_trans->idx);
  draw_vert ends[2]=
    {
      draw_diff(&p_grad_trans->mid, &p_grad->half_delta),
      draw_add(&p_grad_trans->mid, &p_grad->half_delta),
    };
  draw_canvasMarkDirty(p_canvas, &ends[0]);
  draw_canvasMarkDirty(p_canvas, &ends[1]);
}

void draw_gradTranslatedsInit(draw_gradTranslateds *p_grad_trans, const uint32 max_iter, const uint32 num_iters) {
  DO_ASSERT(p_grad_trans->max_iter=max_iter);
  uint32 grad_trans_size=sizeof(draw_gradTranslated)*(num_iters);
  p_grad_trans->p_iter_2_grad_trans=rtu_memAlloc(grad_trans_size);
  //LOG_INFO("pointer: %p trans init. max_iter %u", p_grad_trans->p_iter_2_grad_trans, max_iter);
  if(p_grad_trans->p_iter_2_grad_trans) {
    //LOG_INFO("grad translateds malloced from: %p to %p", p_grad_trans->p_iter_2_grad_trans, p_grad_trans->p_iter_2_grad_trans+num_iters);
    p_grad_trans->num_iters=num_iters;
    draw_gradTranslatedsReset(p_grad_trans);
  } else {
    LOG_ASSERT(false, "malloc of %u grad_trans bytes failed", grad_trans_size);
    p_grad_trans->num_iters=0;
  }
}

void draw_gradTranslatedsReset(draw_gradTranslateds *p_grad_trans) {
  DO_ASSERT(uint32 iter=0);
  for(uint32 i=0; i<p_grad_trans->num_iters; i++) {
    DO_ASSERT(p_grad_trans->p_iter_2_grad_trans[i].idx=iter);
    if(i==8 && p_grad_trans->num_iters==8) {
      LOG_INFO("setting idx for trans (%p). %u", &(p_grad_trans->p_iter_2_grad_trans), p_grad_trans->p_iter_2_grad_trans[i].idx);
    }
    p_grad_trans->p_iter_2_grad_trans[i].initialised=false;
    DO_ASSERT(iter=iter>=p_grad_trans->max_iter?0:iter+1);
  }
}

void draw_gradTranslatedsDestroy(draw_gradTranslateds *p_grad_trans) {
  if(p_grad_trans->p_iter_2_grad_trans) {
    rtu_memFree(p_grad_trans->p_iter_2_grad_trans);
    p_grad_trans->p_iter_2_grad_trans=NULL;
  } else {
    LOG_ASSERT(false, "failed to free gradTranslateds as they were never malloced");
  }
  p_grad_trans->num_iters=0;
}

void draw_scanFillLogInit(draw_scanFillLog *p_f, const draw_renderCore *p_render_core) {
  p_f->highly_acute=(p_render_core->ang<=PI_OVER_EIGHT);
  p_f->min_y=p_render_core->min_y;
  const uint32 rows=draw_renderCoreHeight(p_render_core, p_f);
  p_f->size=rows;

  //LOG_INFO("rows: %u real_max_y: %u. min_y: %i.", rows, p_render_core->real_max_y, p_f->min_y);
  p_f->xs_pairs.p_y_2_starts=rtu_memAlloc(sizeof(uint32)*rows);
  if(p_f->xs_pairs.p_y_2_starts) {
    p_f->xs_pairs.p_y_2_ends=rtu_memAlloc(sizeof(uint32)*rows);
    if(!p_f->xs_pairs.p_y_2_ends) {
      LOG_ASSERT(false, "failed to malloc for p_y_2_ends: %u", sizeof(uint32)*rows);
      return;
    }
  } else {
    LOG_ASSERT(false, "failed to malloc for p_y_2_starts: %u", sizeof(uint32)*rows);
    return;
  }
}

float32 draw_scanFillLogCheckAngle(draw_scanFillLog *p_f, const draw_grad *p_last_grad, const draw_grad *p_grad, const draw_globals *p_globals) {
  float32 ang=draw_gradATan(p_grad, p_globals);
  
  float ang_between=ABSF(ang-p_f->last_ang);
  p_f->last_ang=ang;
  return ang_between;
}

void draw_scanLogInit(draw_scanLog *p_log, 
		      const draw_renderCore *p_render_core, 
		      draw_scanBrushLog *p_b,
		      const draw_globals *p_globals) {
  rtu_memZero(p_log, sizeof(draw_scanLog));
  p_log->p_b=p_b;
  draw_scanFillLogInit(&p_log->f, p_render_core);
}

void draw_gradsArrayInit(draw_gradsArray *p_grads_arr, const uint32 num_iters) {
  p_grads_arr->num_iters=num_iters;
  uint32 iter_2_grad_size=sizeof(draw_grad)*num_iters;
  p_grads_arr->p_iter_2_grad=rtu_memAlloc(iter_2_grad_size);
  if(p_grads_arr->p_iter_2_grad) {
    //LOG_INFO("malloced p_iter_2_grad (size %u) of draw_gradsArray: %p - %p", num_iters, p_grads_arr->p_iter_2_grad, p_grads_arr->p_iter_2_grad+num_iters);    rtu_memZero(p_grads_arr->p_iter_2_grad, iter_2_grad_size);
    for(uint32 idx=0; idx<num_iters; idx++) {
      DO_ASSERT(p_grads_arr->p_iter_2_grad[idx].idx=idx);
      DO_ASSERT(p_grads_arr->p_iter_2_grad[idx].fd_recorded=false);
      p_grads_arr->p_iter_2_grad[idx].initialised=false;
    }
  } else {
    LOG_ERROR("failed to malloc for p_iter_2_grad: %u bytes", iter_2_grad_size);
    exit(-1);
  }
}

void draw_gradsArrayDestroy(draw_gradsArray *p_grads_arr) {
  if(p_grads_arr->p_iter_2_grad) {
    //LOG_INFO("freeing p_iter_2_grad: 0x%p",p_grad_agg->p_iter_2_grad);
    rtu_memFree(p_grads_arr->p_iter_2_grad);
    p_grads_arr->p_iter_2_grad=NULL;
  }
  p_grads_arr->num_iters=0;
}

void draw_thickQuad(const draw_vert *p_0, const draw_vert *p_1, const draw_vert *p_2, draw_scanBrushLog *p_b, draw_globals *p_globals) {
  bool first_matches=draw_vertEq(p_0, p_1);
  bool snd_matches=draw_vertEq(p_1, p_2);
  if(first_matches && snd_matches) {
    return;
  }

  draw_vert pt_1=*p_1;
  if(first_matches || snd_matches) {
    draw_vert sum=draw_add(p_0, p_2);
    pt_1=draw_by(&sum, 0.5);
  }
  float32 ang=draw_vertAngBetween(p_0, p_1, p_2);
  draw_renderCore core;
  draw_renderCoreInit(&core, p_0, &pt_1, p_2, &p_b->w, ang, draw_calcMaxIter(p_b->w.width, ang), &p_globals->canvas);
  if(core.p_w->half_width==0) {
    return;
  }
  draw_gradsArray grads_arr;
  uint32 num_iters=draw_renderCoreMaxIter(&core)+1;
  draw_gradsArrayInit(&grads_arr, num_iters);
  draw_grads grad_agg=draw_gradsInterface();
  draw_gradsInit(&grad_agg, &grads_arr.p_iter_2_grad[0], &core);

  draw_scanLog log;
  draw_scanLogInit(&log, &core, p_b, p_globals);
  draw_renderCoreDraw(&core, &log, &grad_agg, p_globals);
  
  draw_scanLogDestroy(&log);
  draw_gradsDestroy(&grad_agg);

  draw_gradsArrayDestroy(&grads_arr);
  return;
}

draw_vert draw_determineCtrlPt(const draw_vert *p_0, const draw_vert *p_fd_0, const draw_vert *p_2, const draw_vert *p_fd_2) {
  draw_vert ctrl;

  if(p_fd_0->x==0 || p_fd_2->x==0) {
    if(p_fd_0->x==0 && p_fd_2->x==0) {
      ctrl.x=p_0->x;
      ctrl.y=p_0->y;
    } else if(p_fd_0->x==0) {
      LOG_ASSERT(p_fd_2->x!=0, "invalid value for p_fd_2->x %f", p_fd_2->x);

      ctrl.x=p_0->x;
      float32 mb=p_fd_2->y/p_fd_2->x;
      float32 cb_lt=draw_ptSlope2C(p_2, mb);
      ctrl.y=mb*ctrl.x+cb_lt;
    } else {
      LOG_ASSERT(p_fd_2->x==0 && p_fd_0->x!=0, "invalid value for p_fd_2->x %f", p_fd_2->x);
      ctrl.x=p_2->x;

      float32 ma=p_fd_0->y/p_fd_0->x;
      float32 ca_lt=draw_ptSlope2C(p_0, ma);
      ctrl.y=ma*ctrl.x+ca_lt;
    }
  } else {

    float32 ma=p_fd_0->y/p_fd_0->x;
    float32 ca_lt=draw_ptSlope2C(p_0, ma);

    float32 mb=p_fd_2->y/p_fd_2->x;
    float32 cb_lt=draw_ptSlope2C(p_2, mb);
  
    if(mb-ma==0) {
      ctrl=*p_0;
    } else {
      LOG_ASSERT(mb-ma!=0, "slope diff of 0 %f, %f. division by zero will occur", mb, ma);
      ctrl=draw_solveSimultaneous(ma, ca_lt, mb, cb_lt);
    }
  }
  return ctrl;
}

uint32 draw_blotMaxIter(const draw_blot *p_blot) {
  //returns max_iter per quadrant at a cap
  return LOG32(draw_savedWidth2RenderedWidth(p_blot->breadth, p_blot->blur_width))*5+3;
}

void draw_landmarkInit(draw_landmark *p_landmark, const draw_vert *p_corner) {
  //DO_ASSERT(p_landmark->reified=false);
  p_landmark->end=*p_corner;
  draw_vertNullableInit(&p_landmark->cached, NULL);
}

draw_vert draw_landmarkPt(draw_landmark *p_landmark, const uint32 q, const draw_vert *p_mid) {

  if(p_landmark->cached.p_pt!=NULL && p_landmark->cached_q==q) {
    return p_landmark->cached.pt;
  }
  
  draw_vert rel=draw_diff(&p_landmark->end, p_mid);
  sint32 horiz=-draw_quadrant2Horiz(q);
  sint32 vert=draw_quadrant2Vert(q);
  draw_vert *p_result;
  sint32 x_sgn=SGN(rel.x);
  sint32 y_sgn=SGN(rel.y);
  if((horiz==x_sgn || x_sgn==0) && (vert==y_sgn || y_sgn==0)) {
    draw_vert opposite_end=draw_diff(p_mid, &rel);
    p_result=&opposite_end;
  } else {
    LOG_ASSERT(horiz!=x_sgn && vert!=y_sgn, "if we're not in the right quadrant we must be in the opposite quadrant");
    p_result=&p_landmark->end;
  }
  draw_vertNullableSetPt(&p_landmark->cached, p_result);
  p_landmark->cached_q=q;
  return *p_result;
}

static sint8 *draw_test_landmarkPt(void) {
  draw_vert mid={
    .x=1,
    .y=1
  };
  const float32 fuzz=0.1;

  draw_landmark top_left={
    .end={
      .x=0,
      .y=0
    },
    .cached={
      .p_pt=NULL
    }
  };
  
  draw_landmark bottom_right={
    .end={
      .x=2,
      .y=2
    },
    .cached={
      .p_pt=NULL
    }
  };

  draw_vert pt=draw_landmarkPt(&bottom_right, 0, &mid);
  mu_assert("wrong end point for q 0: 2,2", draw_vertSim(&pt, &top_left.end, fuzz));
  draw_vertNullableSetPt(&bottom_right.cached, NULL); //clear cache
  pt=draw_landmarkPt(&bottom_right, 2, &mid);
  mu_assert("wrong end point for q 2: 2,2", draw_vertSim(&pt, &bottom_right.end, fuzz));


  draw_landmark bottom_left={
    .end={
      .x=0,
      .y=2
    },
    .cached={
      .p_pt=NULL
    }
  };

  draw_landmark top_right={
    .end={
      .x=2,
      .y=0
    },
    .cached={
      .p_pt=NULL
    }
  };

  pt=draw_landmarkPt(&top_right, 1, &mid);
  mu_assert("wrong end point for q 1: 2,0", draw_vertSim(&pt, &bottom_left.end, fuzz));
  draw_vertNullableSetPt(&top_right.cached, NULL); //clear cache
  pt=draw_landmarkPt(&top_right, 3, &mid);
  mu_assert("wrong end point for q 3: 2,0", draw_vertSim(&pt, &top_right.end, fuzz));
  return 0;
}

void draw_scanBrushLogGradDistRanker(draw_scanBrushLog *p_b, const draw_grad *p_grad, const uint32 q, const uint32 iter) {
  LOG_ASSERT(p_grad->p_landmark, "no landmark");
  draw_landmark *p_landmark=p_grad->p_landmark;
  uint32 span=p_b->proximity.span;
  sint32 start_x_delta=draw_landmarkXDelta(p_landmark, 0, q, &p_grad->mid);
  uint32 y_offset_idx=0;
  for(sint32 y=0; y<span; y++) {
    //uint32 y_offset_idx=draw_proxOffset(&p_b->proximity, p_b->proximity.cen.y, y)*p_b->proximity.span;//draw_scanBrushLogRowOffset(p_b, &p_grad->mid, y);
    sint32 landmark_row_dist=(y-(draw_landmarkPt(p_landmark, q, &p_grad->mid).y+DRAW_PROXIMITY_ORIGIN_OFFSET))*DRAW_PROXIMITY_FIXED_POINT;
    uint32 landmark_row_dist_sq=landmark_row_dist*landmark_row_dist;
    sint32 x_delta=start_x_delta;
    for(uint32 x=0; x<span; x_delta-=DRAW_PROXIMITY_FIXED_POINT, x++) {
      //+1 is to account for us added 1 pixel boundary to top and left of span array
      draw_scanBrushLogLandmarkDistRanker(p_b, x, y_offset_idx, landmark_row_dist_sq, x_delta, q, iter);
    }
    y_offset_idx+=span;
  }
}

void draw_blotInit(draw_blot *p_blot, draw_scanBrushLog *p_b, draw_globals *p_globals) {
  p_blot->breadth=p_b->w.breadth; //setting this before guard ensures subsequent blotRenders to do anything
  p_blot->blur_width=p_b->w.blur_width;
  
  float32 half_width=draw_breadth2HalfWidth(p_b->w.breadth, p_blot->blur_width);
  if(p_b->w.breadth==0) { //ensure a corresponding guard exists in draw_blotDestroy
    return;
  }
  draw_vert pt_0={ .x=half_width, .y=half_width };
  p_blot->origin=pt_0;

  draw_vert pts[3]={ pt_0 };
  pts[0].y+=half_width;
  sint32 horiz=draw_quadrant2Horiz(2), next_horiz;
  sint32 vert=draw_quadrant2Vert(2), next_vert;
  uint32 max_iter=draw_blotMaxIter(p_blot);
  draw_gradTranslatedsInit(&p_blot->trans, max_iter, 4*(max_iter+1));
  
  uint32 num_grads=(max_iter+1)<<1;
  draw_gradsArrayInit(&p_blot->grads_arr, num_grads);
  
  for(uint32 q=2; q<=3; q++) {
    uint32 next_q=(q+1) & 0x3;
    next_horiz=draw_quadrant2Horiz(next_q);
    next_vert=draw_quadrant2Vert(next_q);
    
    draw_vert ctrl_delta={ .x=half_width*horiz,.y=-half_width*vert };
    pts[1]=draw_add(&pt_0, &ctrl_delta);
    draw_vert end_delta={ .x=half_width*next_horiz*(next_horiz==horiz),.y=-half_width*next_vert*(next_vert==vert) };
    pts[2]=draw_add(&pt_0, &end_delta);

    draw_renderCore render_core;
    draw_renderCoreInit(&render_core, &pts[0], &pts[1], &pts[2], &p_b->w, M_PI_2, max_iter, &p_globals->canvas);

    draw_grads *p_grad_agg=draw_blotQuadrant2Grads(p_blot, horiz, vert);
    *p_grad_agg=draw_gradsInterface();
    draw_gradsInit(p_grad_agg, &p_blot->grads_arr.p_iter_2_grad[(q==3)*(max_iter+1)], &render_core);
    draw_gradsSetMids(p_grad_agg, &pt_0);

    draw_scanLog log;
    draw_scanLogInit(&log, &render_core, p_b, p_globals);
    draw_plotBez(render_core.step, pts, NULL, DRAW_GRADS_IF_ON_ITER_CB_ARG(p_b->w.width<DRAW_TRI_THRESH?draw_initSmallGrad:draw_initGrad, p_grad_agg), &log, &p_grad_agg->grads_if, p_globals);
    draw_scanLogDestroy(&log);

    draw_coordToIter *p_coord_2_iter=draw_blotQuadrant2CoordToIter(p_blot, horiz, vert);

    draw_coordToIterInit(p_coord_2_iter, p_grad_agg, &p_blot->origin, q, &p_b->w, 0, p_grad_agg->max_iter);

    horiz=next_horiz;
    vert=next_vert;
    
    pts[0]=pts[2];
  }

  
  if(p_b->w.width<DRAW_TRI_THRESH) {
    /* Calculate the distances squared between each point
     * and its two nearest draw_grad landmarks
     */
    for(uint32 q=0; q<=3; q++) {
      horiz=draw_quadrant2Horiz(q);
      vert=draw_quadrant2Vert(q);
      draw_grads *p_grad_agg=draw_blotQuadrant2Grads(p_blot, horiz, vert);
      draw_grad *p_iter_2_grad=draw_gradsGradPtr(p_grad_agg, UINT_MAX);
      for(uint32 iter=0; iter<=p_grad_agg->max_iter; iter++) {
	draw_grad *p_grad=&p_iter_2_grad[iter];
	draw_scanBrushLogGradDistRanker(p_b, p_grad, q, iter);
      }
    }   
  }
}

void draw_gradDestroy(draw_grad *p_grad) {
  if(p_grad->p_landmark) {
    rtu_memFree(p_grad->p_landmark);
  }
  DO_ASSERT(p_grad->fd_recorded=false);
  p_grad->initialised=false;
}

void draw_blotDestroy(draw_blot *p_blot) {
  if(p_blot->breadth==0) { //ensure a corresponding guard exists in draw_blotInit
    return;
  }

  for(uint32 q=2; q<=3; q++) {
    sint32 right=draw_quadrant2Horiz(q);
    sint32 down=draw_quadrant2Vert(q);
    draw_grads *p_grad_agg=draw_blotQuadrant2Grads(p_blot, right, down);
    for(uint32 iter=0; iter<=p_grad_agg->max_iter; iter++) {
      draw_grad *p_grad=&p_grad_agg->p_iter_2_grad[iter];
      draw_gradDestroy(p_grad);
    }

    if(p_grad_agg->p_iter_2_grad) {
      draw_gradsDestroy(p_grad_agg);
    }

  }
  LOG_ASSERT(DIM(p_blot->coord_2_iters)==2, "freeing incorrect number of draw_coordToIter instances");
  for(uint32 i=0; i<2; i++) {
    draw_coordToIterDestroy(&p_blot->coord_2_iters[i]);
  }

  draw_gradsArrayDestroy(&p_blot->grads_arr);
  draw_gradTranslatedsDestroy(&p_blot->trans);
}

void draw_blotContinue(const draw_vert *p_0, const draw_vert *p_fd, const float32 breadth, const float32 blur_width, const uint32 col, draw_globals *p_globals) {
  draw_brush brush;
  draw_brushInitInternal(&brush, col, breadth, blur_width, p_globals, false);
  draw_stroke stroke;
  draw_strokeInit(&stroke, &brush, p_globals);
  stroke.last_fd=draw_neg(p_fd);
  //stroke.last_fd=*p_fd;
  stroke.state=DRAW_STROKE_IN_STROKE;
  stroke.last=*p_0;
  draw_strokeRender(&stroke, p_globals);

  draw_brushDestroy(&brush);
}

void draw_blotHere(const draw_vert *p_0, const float32 breadth, const float32 blur_width, const uint32 col, draw_globals *p_globals) {
  draw_brush brush;
  draw_brushInit(&brush, col, breadth, blur_width, p_globals);
  draw_stroke stroke;
  draw_strokeInit(&stroke, &brush, p_globals);
  draw_strokeMoveTo(&stroke, p_0, p_globals);
  draw_strokeRender(&stroke, p_globals);
}

void draw_quadInit(draw_quad *p_bez, const draw_vert **pp_verts) {
  p_bez->pts[0]=*pp_verts[0];
  p_bez->pts[1]=*pp_verts[1];
  p_bez->pts[2]=*pp_verts[2];
  draw_vert pt_0_by_minus_2=draw_by(&p_bez->pts[0], -2);
  draw_vert pt_1_by_2=draw_by(&p_bez->pts[1], 2);
  p_bez->fd_init_val=draw_add(&pt_0_by_minus_2, &pt_1_by_2);

  draw_vert pt_0_by_2=draw_by(&p_bez->pts[0], 2);
  draw_vert pt_1_by_minus_4=draw_by(&p_bez->pts[1], -4);
  draw_vert pt_2_by_2=draw_by(&p_bez->pts[2], 2);
  draw_vert sum=draw_add(&pt_0_by_2, &pt_1_by_minus_4);
  p_bez->fd_delta=draw_add(&sum, &pt_2_by_2);
}

void draw_quadStartFDGen(draw_quad *p_bez, float32 step) {
  draw_vert two_p1=draw_by(&p_bez->pts[1], 2);
  draw_vert two_p2=draw_by(&p_bez->pts[2], 2);

  draw_vert init_fd={.x=0, .y=0};
  p_bez->fds[0]=init_fd;
  p_bez->fds[1]=draw_diff(&two_p2, &two_p1);

  p_bez->step=step;
  p_bez->step_sum=0;

  p_bez->fd_delta_by_step=draw_by(&p_bez->fd_delta, step);
  p_bez->fd_t=p_bez->fd_init_val;
}

void draw_coordToIterCalcCoord2IterSize(draw_coordToIter *p_c2I, const draw_grads *p_grad_agg, const draw_strokeWidth *p_w) {
  uint32 num_iters=1+p_c2I->end_iter-p_c2I->start_iter;
  uint32 iter=(num_iters>>1)+p_c2I->start_iter;
  uint32 end_iter=p_c2I->end_iter;
  sint32 iter_inc=-p_c2I->right*p_c2I->down;
  for(; iter>=p_c2I->start_iter && iter<=end_iter;) {
    draw_grad *p_grad=&p_grad_agg->p_iter_2_grad[iter];
    if(p_grad->on_x) {
      iter-=iter_inc;
      continue;
    }
    break;
  }
  LOG_ASSERT(!p_grad_agg->p_iter_2_grad[iter].on_x, "iter %u should not be on x",iter);
  uint32 last_iter=iter;
  for(; iter<=end_iter;) {
    draw_grad *p_grad=&p_grad_agg->p_iter_2_grad[iter];
    if(!p_grad->on_x) {
      last_iter=iter;
      iter+=iter_inc;
      continue;
    }
    break;
  }
  LOG_ASSERT(p_grad_agg->p_iter_2_grad[iter].on_x && iter==last_iter+iter_inc && !p_grad_agg->p_iter_2_grad[last_iter].on_x, "draw_grad at %u should be on_x", iter, last_iter);

  draw_vert max_y=p_grad_agg->p_iter_2_grad[last_iter].half_delta;
  p_c2I->abs_max_y_x=ABSF(max_y.x);
  p_c2I->max_on_y_x_coord=p_c2I->abs_max_y_x*2+1;
  p_c2I->min_on_y_y_coord=ABSF(max_y.y);
  p_c2I->num_coords=(p_c2I->min_on_y_y_coord*2)+p_c2I->max_on_y_x_coord+1;
}

void draw_coordToIterInit(draw_coordToIter *p_c2I, draw_grads *p_grad_agg, const draw_vert *p_origin, const sint32 q, const draw_strokeWidth *p_w, const uint32 start_iter, const uint32 end_iter) {
  LOG_ASSERT(start_iter>=0 && start_iter<=p_grad_agg->max_iter, "out of range start: %u vs max %u", start_iter, p_grad_agg->max_iter);
  LOG_ASSERT(end_iter>=start_iter && end_iter<=p_grad_agg->max_iter, "out of range end: %u start: %u vs max %u", end_iter, start_iter, p_grad_agg->max_iter);
  LOG_ASSERT(q>=0 && q<=3, "out of range q: %i", q); 

  p_c2I->start_iter=start_iter;
  p_c2I->end_iter=end_iter;
  p_c2I->p_grad_agg=p_grad_agg;

  p_c2I->right=draw_quadrant2Horiz(q);
  p_c2I->down=draw_quadrant2Vert(q);

  p_c2I->origin=*p_origin;
  draw_coordToIterCalcCoord2IterSize(p_c2I, p_grad_agg, p_w);
  //LOG_INFO("mallocing %u uint32s", p_c2I->num_coords);
  p_c2I->p_coord_2_iter=rtu_memAlloc(sizeof(uint32)*p_c2I->num_coords);
  if(p_c2I->p_coord_2_iter) {
    uint32 coord_idx=0;
    for(uint32 iter=start_iter; iter<=end_iter; iter++) {
      if(coord_idx>=p_c2I->num_coords) {
	return;
      }
      draw_grad *p_grad=&p_grad_agg->p_iter_2_grad[iter];
      draw_vert half_delta=p_grad->half_delta;
      if(half_delta.y<0) {
	half_delta=draw_neg(&half_delta);
      }

      uint32 terminal=draw_coordToIterVert2Idx(p_c2I, &half_delta);
      if(BSGN(p_c2I->down)*BSGN(p_c2I->right)==1) {
	terminal=(p_c2I->num_coords-terminal)-1;
      }
      LOG_ASSERT(terminal<p_c2I->num_coords, "going to overflow");
      for(;coord_idx<=terminal; coord_idx++) {
	p_c2I->p_coord_2_iter[coord_idx]=iter;
      }
    }
  } else {
    LOG_ASSERT(false, "failed to alloc %u elements", p_c2I->num_coords);
  }
}

sint32 draw_coordToIterVert(const draw_coordToIter *p_c2I) {
  return p_c2I->down;
}

uint32 draw_coordToIterStartIter(const draw_coordToIter *p_c2I) {
  return p_c2I->start_iter;
}

uint32 draw_coordToIterGetIter(const draw_coordToIter *p_c2I, const draw_vert *p_0) {
  //assumes *p_0 is an endpoint of a draw_grad instance relative to its draw_grad_tran.mid
  draw_vert delta;
  if(p_0->y<=0) {
    delta=draw_neg(p_0);
  }  else {
    delta=*p_0;
  }
  if((BSGN(delta.x)*BSGN(delta.y))!=-p_c2I->right*p_c2I->down) {
    /*
      Apparently incorrect quadrant. Assume its due to cumulative
      error in our taylor-series inspired draw_curveIfPlot and round
      off p_0.
    */
    delta.x=ROUND32(delta.x);
    delta.y=ROUND32(delta.y);
  }
  LOG_ASSERT(SGN(delta.x)==0 || SGN(delta.y)==0 || (BSGN(delta.x)*BSGN(delta.y))==-p_c2I->right*p_c2I->down, "this function can't deal with this point as it's in the wrong quadrant: %f,%f", delta.x, delta.y);
  uint32 iter=draw_coordToIterVert2Idx(p_c2I, &delta);
  if(BSGN(p_c2I->down)*BSGN(p_c2I->right)==1) {
    iter=(p_c2I->num_coords-iter)-1;
  }
  LOG_ASSERT(iter<p_c2I->num_coords, "going to overflow");
  return p_c2I->p_coord_2_iter[iter];
}

void draw_coordToIterDestroy(draw_coordToIter *p_c2I) {
  if(p_c2I->p_coord_2_iter) {
    rtu_memFree(p_c2I->p_coord_2_iter);
    p_c2I->p_coord_2_iter=NULL;
    p_c2I->num_coords=0;
  }
}

void draw_vertNullableInit(draw_vertNullable *p_vertInstPtr, const draw_vert *p_0) {
  draw_vertNullableSetPt(p_vertInstPtr, p_0);
}

draw_gradReference draw_gradReferenceInterface(void) {
  draw_gradReference grad_ref={
    .grads_if={
      .p_max_iter_cb=DRAW_GRADS_IF_MAX_ITER_CB_ARG(draw_gradReferenceMaxIter, &grad_ref),
      .p_grad_ptr_cb=DRAW_GRADS_IF_GRAD_PTR_CB_ARG(draw_gradReferenceGradPtr, &grad_ref),
      .p_iter_range_cb=DRAW_GRADS_IF_ITER_RANGE_CB_ARG(draw_gradReferenceIterRange, &grad_ref),
      .p_trans_cb=DRAW_GRADS_IF_TRANS_CB_ARG(draw_gradReferenceTrans, &grad_ref)
    }
  };
  return grad_ref;
}

void draw_gradReferenceInit(draw_gradReference *p_grad_ref,
			    uint32 num_coord_2_iters,
			    draw_coordToIter *p_coord_2_iters,
			    const draw_vert *p_center,
			    const uint32 last_iter,
			    const uint32 start_quadrant,
			    const uint32 max_iter,
			    const draw_vertNullable *p_last_pt,
			    draw_gradTranslated *p_iter_2_grad_trans) {
  LOG_ASSERT(num_coord_2_iters==2, "expecting two coord_2_iters: %u", num_coord_2_iters);
  for(uint32 i=1; i<num_coord_2_iters; i++) {
    LOG_ASSERT(max_iter==draw_coordToIterMaxIter(&p_coord_2_iters[i]), "not all coord_2_iters have same max_iter: %u", max_iter);
  }
  p_grad_ref->p_coord_2_iters=p_coord_2_iters;
  p_grad_ref->center=*p_center;
  p_grad_ref->last_iter=last_iter;
  p_grad_ref->last_pt=*p_last_pt;
  //DO_ASSERT(p_grad_ref->range.iter_inc=0);
  p_grad_ref->p_iter_2_grad_trans=p_iter_2_grad_trans;
  p_grad_ref->start_quadrant=start_quadrant;
  p_grad_ref->max_iter=max_iter;
}

void draw_gradReferenceDestroy(draw_gradReference *p_grad_ref) {
}

draw_iterRange draw_gradReferenceIterRange(draw_gradReference *p_grad_ref) {
  return p_grad_ref->range;
}

void draw_gradReferenceSetIterRange(draw_gradReference *p_grad_ref, const uint32 q, const uint32 iter) {
  draw_iterRangeInit(&p_grad_ref->range, q, iter);
}

void draw_brushInit(draw_brush *p_brush, const uint32 col, const float32 breadth, const float32 blur_width, draw_globals *p_globals) {
  draw_brushInitInternal(p_brush, col, breadth, blur_width, p_globals, true);
}

uint32 draw_brushMagFactor(const draw_brush *p_brush) {
  return draw_scanBrushLogMagFactor(&p_brush->b);
}

float32 draw_brushScaleFactor(const draw_brush *p_brush) {
  return draw_scanBrushLogScaleFactor(&p_brush->b);
}

bool draw_renderable(const float32 breadth, const uint32 col) {
	return !(breadth==0 || (col & 0xff000000)==0);
}

bool draw_brushRenderable(const draw_brush *p_brush) {
  return draw_renderable(p_brush->b.w.breadth, p_brush->b.col);
}

void draw_brushDestroy(draw_brush *p_brush) {
  draw_blotDestroy(&p_brush->blot);
  draw_scanBrushLogDestroy(&p_brush->b);
}

float32 draw_brushHalfWidth(const draw_brush *p_brush) {
  return draw_blotHalfWidth(&p_brush->blot);
}

float32 draw_brushBreadth(const draw_brush *p_brush) {
  return p_brush->b.w.breadth;
}

void draw_strokeInit(draw_stroke *p_stroke, const draw_brush *p_brush, draw_globals *p_globals) {
  LOG_ASSERT(p_brush->b.w.breadth>=0, "invalid brush width: %f", p_brush->b.w.breadth);
  p_stroke->brush=*p_brush;

  draw_vert start={ .x=0, .y=0 };
  p_stroke->last=start;
  draw_vert any_fd={ .x=1, .y=1 };
  p_stroke->last_fd=any_fd;

  p_stroke->state=DRAW_STROKE_RENDERED;
  //draw_strokeReset(p_stroke, p_globals);
}

draw_vert draw_strokeEndFD(const draw_stroke *p_stroke, const draw_vert *p_last, const draw_vert *p_ctrl, const draw_vert *p_end, const draw_brush *p_brush) {
  draw_vert fd_1;
  if(p_end->x!=p_ctrl->x || p_end->y!=p_ctrl->y) {
    fd_1.x=draw_plotBezInitFD_1(p_ctrl->x, p_end->x);
    fd_1.y=draw_plotBezInitFD_1(p_ctrl->y, p_end->y);
  } else if(p_last->x!=p_ctrl->x || p_last->y!=p_ctrl->y) {
    fd_1.x=draw_plotBezInitFD_1(p_last->x, p_ctrl->x);
    fd_1.y=draw_plotBezInitFD_1(p_last->y, p_ctrl->y);
  } else {
    float32 half_width=draw_brushHalfWidth(p_brush);
    fd_1.x=half_width;
    fd_1.y=0;
  }
  return fd_1;
}

draw_vert draw_strokeStartFD(const draw_stroke *p_stroke, const draw_vert *p_last, const draw_vert *p_ctrl, const draw_vert *p_end, const draw_brush *p_brush) {
  draw_vert fd;
  if(p_last->x!=p_ctrl->x || p_last->y!=p_ctrl->y) {
    fd.x=draw_plotBezInitFD_0(p_last->x, p_ctrl->x);
    fd.y=draw_plotBezInitFD_0(p_last->y, p_ctrl->y);
  } else {
    fd=draw_strokeEndFD(p_stroke, p_last, p_ctrl, p_end, p_brush);
  }
  return fd;
}

void draw_canvasMarkDirtyRadius(draw_canvas *p_canvas, const draw_vert *p_0, const float32 breadth, const float32 blur_width) {
  float32 half_width=draw_breadth2HalfWidth(breadth, blur_width);
  draw_rect bnds=
    {
      .lt={
	.x=p_0->x-half_width,
	.y=p_0->y-half_width
      },
      .rb={
	.x=p_0->x+half_width,
	.y=p_0->y+half_width
      }
    };
  draw_canvasMarkDirtyRect(p_canvas, &bnds);
}

sint32 draw_vertNonReflexAngNormalised(const draw_vert *p_fst, const draw_vert *p_snd) {
  /*
    Assumes p_fst and p_snd are directional vectors.  Then are two
    angle formed between the vector position of p_fst, the origin and
    p_snd -- a reflex angle (around the outside) or the
    non-reflex angle (ie acute or obtuse on the inside). This function indicates
    which angle is which. 

    If +1 is returned the acute/obtuse angle is found by travelling
    clockwise from p_fst, around the origin to p_snd. Travelling
    anticlockwise from p_fst to p_snd is consequently the reflex
    angle.

    If -1 is returned the acute/obtuse angle is found by travelling
    anticlockwise from p_fst, around the origin to p_snd. Travelling
    clockwise from p_fst to p_snd is consequently the reflex angle.

    In summary a result of +1 means travelling clockwise from *p_fst
    to *p_snd is traversing the acute/obtuse angle. A result of -1
    means travelling anticlockwise from *p_fst to *p_snd is traversing
    the acute/obtuse angle.

    Finally a 0 is returned where the answer is undefined (ie where
    an angle of 0 or PI radians exists between the vectors).
  */

  DO_ASSERT(draw_vert snd_180=draw_by(p_snd, -1));
  DO_ASSERT(draw_vert *p_snd_180=&snd_180);

  sint32 fst_x_sgn=BSGN(p_fst->x);
  sint32 snd_x_sgn=BSGN(p_snd->x);

  sint32 fst_y_sgn=BSGN(p_fst->y);
  sint32 snd_y_sgn=BSGN(p_snd->y);

  sint32 fst_sgn=fst_x_sgn*fst_y_sgn;
  sint32 snd_sgn=snd_x_sgn*snd_y_sgn;

  draw_vert fst;

  if(fst_sgn==snd_sgn) {
    //fst and snd either in same or opposite quadrants
    sint32 quadrant_multiplier=((fst_x_sgn==snd_x_sgn)<<1)-1; //modifies result if p_fst and p_snd are in opposite quadrants.

    //both are fst and snd are in the same quadrant.
    LOG_ASSERT((fst_x_sgn==snd_x_sgn)==(fst_y_sgn==snd_y_sgn), "not in the same quadrant");
    /*
      This table assumes that *p_fst and *p_snd are in the same
      quadrant. If they are in opposite quadrants then result, and the
      args corresponding to one of the quadrants in the input table
      colums must all be multiplied by -1.

      Assuming that p_fst->x can be compared with p_snd->x and
      p_fst->y can be compared with p_snd->y.

      fst x = BSGN(p_fst->x). fst y defined similarly.  fst y<snd y
      is p_fst->y<p_snd->y. fst x<snd x defined similarly.

      +-----+-----+---------+-----+------+
      |fst x|fst y|fst x <  |fst y|result|
      |     |     |snd x    |< snd|      |
      |     |     |         |y    |      |
      +-----+-----+---------+-----+------+
      | -1  |-1   | false   |true |+1    |
      +-----+-----+---------+-----+------+
      | -1  |+1   |true     |true |+1    |
      +-----+-----+---------+-----+------+
      | +1  |-1   |false    |false|+1    |
      +-----+-----+---------+-----+------+
      | +1  |+1   |true     |false|+1    |
      +-----+-----+---------+-----+------+
      | -1  |-1   | true    |false|-1    |
      +-----+-----+---------+-----+------+
      | -1  |+1   | false   |false|-1    |
      +-----+-----+---------+-----+------+
      | +1  |-1   |true     |true |-1    |
      +-----+-----+---------+-----+------+
      | +1  |+1   |false    |true |-1    |
      +-----+-----+---------+-----+------+
      | -1  |-1   | false   |false| na   |
      +-----+-----+---------+-----+------+
      | -1  |-1   | true    |true |na    |
      +-----+-----+---------+-----+------+
      | -1  |+1   |false    |true |na    |
      +-----+-----+---------+-----+------+
      | -1  |+1   |true     |false|na    |
      +-----+-----+---------+-----+------+
      | +1  |-1   |false    |true |na    |
      +-----+-----+---------+-----+------+
      | +1  |-1   |true     |false|na    |
      +-----+-----+---------+-----+------+
      | +1  |+1   |false    |false|na    |
      +-----+-----+---------+-----+------+
      | +1  |+1   |true     |true |na    |
      +-----+-----+---------+-----+------+

      ie either compare fst y and fst x<snd x or fst x and fst y<snd y
    */

    DO_ASSERT(fst_x_sgn*=quadrant_multiplier);
    fst_y_sgn*=quadrant_multiplier;
    fst=draw_by(p_fst, quadrant_multiplier);
    p_fst=&fst;

    sint32 result=fst_y_sgn*(((p_fst->x<p_snd->x)<<1)-1);
    LOG_ASSERT(IMPLIES((-fst_x_sgn*(((p_fst->y<p_snd->y)<<1)-1))!=result, (rtu_similarToZero(draw_euclideanDistSquared(p_fst, p_snd), 0.01) || rtu_similarToZero(draw_euclideanDistSquared(p_fst, p_snd_180), 0.01))), "inconsistent result: fst x: %i, fst y: %i, fst x < snd x: %u, fst y<snd y: %u", fst_x_sgn, snd_x_sgn, p_fst->x<p_snd->x, p_fst->y<p_snd->y);
    return result*quadrant_multiplier;
  } else {
    /*
      Any input combinations not listed in the table should be handled
      by the earlier 'then' part of the conditional statement.

      +-----+-----+-----+-----+------+
      |fst x|fst y|snd x|snd y|result|
      +-----+-----+-----+-----+------+
      |-1   |-1   |-1   |+1   |+1    | 
      +-----+-----+-----+-----+------+
      |-1   |+1   |+1   |+1   |+1    | 
      +-----+-----+-----+-----+------+
      |+1   |+1   |+1   |-1   |+1    | 
      +-----+-----+-----+-----+------+
      |+1   |-1   |-1   |-1   |+1    | 
      +-----+-----+-----+-----+------+
      |-1   |-1   |+1   |-1   |-1    | 
      +-----+-----+-----+-----+------+
      |-1   |+1   |-1   |-1   |-1    | 
      +-----+-----+-----+-----+------+
      |+1   |+1   |-1   |+1   |-1    | 
      +-----+-----+-----+-----+------+
      |+1   |-1   |+1   |+1   |-1    | 
      +-----+-----+-----+-----+------+

    */
    return -fst_x_sgn*snd_y_sgn;
  }
}

sint32 draw_vertNonReflexAng(const draw_vert *p_not_normalised_fst, const draw_vert *p_not_normalised_snd) {
  
  float32 mag_fst=draw_mag(p_not_normalised_fst);

  float mag_snd=draw_mag(p_not_normalised_snd);
  draw_vert fst=draw_divide(p_not_normalised_fst, mag_fst);
  draw_vert *p_fst=&fst;
  draw_vert snd=draw_divide(p_not_normalised_snd, mag_snd);
  draw_vert *p_snd=&snd;

  return draw_vertNonReflexAngNormalised(p_fst, p_snd);
}

static sint8 *draw_test_horizVert2Quadrant(void) {
  uint32 q;

  q=0;
  mu_assert("quadrant mismatch 0", draw_horizVert2Quadrant(draw_quadrant2Horiz(q), draw_quadrant2Vert(q))==q);
  
  q=1;
  mu_assert("quadrant mismatch 1", draw_horizVert2Quadrant(draw_quadrant2Horiz(q), draw_quadrant2Vert(q))==q);
  
  q=2;
  mu_assert("quadrant mismatch 2", draw_horizVert2Quadrant(draw_quadrant2Horiz(q), draw_quadrant2Vert(q))==q);
  
    q=3;
    mu_assert("quadrant mismatch 3", draw_horizVert2Quadrant(draw_quadrant2Horiz(q), draw_quadrant2Vert(q))==q);
  return 0;
}

static sint8 *draw_test_vertNonReflexAng(void) {
  draw_vert fst={ .x=0, .y=110 };
  draw_vert neg_fst=draw_neg(&fst);
  draw_vert snd={ .x=-284, .y=-30 };
  draw_vert neg_snd=draw_neg(&snd);
  //q0 is the bottom-left quadrant, q1 is the top-left quadrant etc
  mu_assert("reflex angle in direction indicated by result (up vertex - q0 vertex)", draw_vertNonReflexAng(&fst, &snd)==-1);
  mu_assert("reflex angle in direction indicated by result (down vertex - q0 vertex)", draw_vertNonReflexAng(&neg_fst, &snd)==1);
  mu_assert("reflex angle in direction indicated by result (down vertex - q2 vertex)", draw_vertNonReflexAng(&neg_fst, &neg_snd)==-1);
  mu_assert("reflex angle in direction indicated by result (up vertex - q2 vertex)", draw_vertNonReflexAng(&fst, &neg_snd)==1);
  mu_assert("result should indicate +1 or -1 direction (up vertex - down vertex)", rtu_abs(draw_vertNonReflexAng(&fst, &neg_fst))==1);
  mu_assert("result should indicate +1 or -1 direction (q0 vertex - q2 vertex)", rtu_abs(draw_vertNonReflexAng(&snd, &neg_snd))==1);
  return 0;
}

void draw_completeQuadrant(
			   draw_scanLog *p_log,
			   draw_gradReference *p_ref,
			   const draw_vert *p_0,
			   const float32 width,
			   draw_globals *p_globals
) {
  uint32 quad_iter;
  if(width>=DRAW_TRI_THRESH) {
    quad_iter=(draw_gradReferenceIter(p_ref, p_0) | (draw_gradReferenceQuadrant(p_ref) << DRAW_PROXIMITY_ITER_BITS));
    draw_triNow(p_log, p_ref, p_0, quad_iter, p_globals);
  } else {
    quad_iter=draw_scanBrushLogPt2Iter(p_log->p_b, p_ref, p_0);
    draw_smallTriNow(p_log, p_ref, p_0, quad_iter, p_globals);
  }
}

void draw_strokeCap(draw_stroke *p_stroke, const draw_vert *p_0, draw_vert *p_last_fd, draw_vert *p_next_fd, draw_globals *p_globals) {
#if 1
  draw_canvasMarkDirtyRadius(&p_globals->canvas, p_0, p_stroke->brush.b.w.breadth, p_stroke->brush.b.w.blur_width);

  draw_vert neg_last_fd=draw_neg(p_last_fd);
  draw_vert neg_next_fd=draw_neg(p_next_fd);
  float32 width=draw_savedWidth2RenderedWidth(p_stroke->brush.b.w.breadth, p_stroke->brush.b.w.blur_width);
  float32 half_width=width*0.5;

  draw_vert *p_stroke_end_pts[2], *p_stroke_start_pts[2];

  draw_rect end_pts=draw_calcTerminalBox(p_0, p_last_fd, half_width);
  p_stroke_end_pts[0]=&end_pts.lt;
  p_stroke_end_pts[1]=&end_pts.rb;
  draw_rect start_pts=draw_calcTerminalBox(p_0, &neg_next_fd, half_width);
  p_stroke_start_pts[0]=&start_pts.lt;
  p_stroke_start_pts[1]=&start_pts.rb;
  /* We wish to draw a curve around the outside of the stroke from
     either p_stroke_start_pts[0] to p_stroke_end_pts[1] or from
     p_stroke_start_pts[1] to p_stroke_end_pts[0].

     See feathering_angles.svg (E).
  */
  draw_vert join_offsets[2];
  join_offsets[0]=draw_diff(p_stroke_end_pts[0], p_0);
  join_offsets[1]=draw_diff(p_stroke_start_pts[0], p_0);
  sint32 zeroth_order=draw_vertNonReflexAngNormalised(&join_offsets[0], &join_offsets[1]);
  /*
    If zeroth_order is +1, ie moving clockwise from
    p_stroke_end_pts[0] to p_stroke_start_pts[0] around the fulcrum
    p_0 is an non reflex angle, we should draw the curve around the
    outside of the stroke from p_stroke_start_pts[0] to
    p_stroke_end_pts[1]. Otherwise the rendered curve should be from
    p_stroke_start_pts[1] to p_stroke_end_pts[0].
  */

  draw_vert terminal_start, terminal_end, *p_start_fd, *p_end_fd;
  if(zeroth_order==+1) {
    terminal_start=*p_stroke_start_pts[0];
    terminal_end=*p_stroke_end_pts[1];

    p_start_fd=p_next_fd;
    p_end_fd=&neg_last_fd;
  } else {
    LOG_ASSERT(zeroth_order==-1, "invalid zeroth order %i", zeroth_order);
    terminal_end=*p_stroke_start_pts[1];
    terminal_start=*p_stroke_end_pts[0];

    p_start_fd=&neg_last_fd;
    p_end_fd=p_next_fd;
  }

  if((sint32)terminal_start.x==(sint32)terminal_end.x && (sint32)terminal_start.y==(sint32)terminal_end.y) {
    return;
  }

  draw_vert term_start_delta=draw_diff(&terminal_start, p_0);
  draw_vert term_end_delta=draw_diff(&terminal_end, p_0);
  uint32 start_quadrant=draw_horizVert2Quadrant(BSGN(term_start_delta.x), -BSGN(term_start_delta.y)), 
    end_quadrant=draw_horizVert2Quadrant(BSGN(term_end_delta.x), -BSGN(term_end_delta.y));
  if(end_quadrant<start_quadrant) {
    end_quadrant+=4;
  }

  const uint32 H_IDX=0;
  const uint32 V_IDX=1;
  draw_vert incoming_fd=*p_start_fd, outgoing_fd;
  draw_vert start=terminal_start, ctrl, end;
  float step;

  draw_vertNullable initial_pt;
  draw_vertNullableInit(&initial_pt, NULL);
  const uint32 initial_iter=UINT_MAX;

  draw_coordToIter *p_coord_2_iters=draw_blotCoordToIters(&p_stroke->brush.blot);
  uint32 max_iter=draw_blotMaxIter(&p_stroke->brush.blot);
  draw_gradTranslateds *p_trans=&p_stroke->brush.blot.trans;
  draw_gradTranslatedsReset(p_trans);
  uint32 cur_quadrant=start_quadrant;
  for(; cur_quadrant<end_quadrant; cur_quadrant++) {
    uint32 q=cur_quadrant & 3;

    sint32 horiz=draw_quadrant2Horiz(q);
    sint32 vert=draw_quadrant2Vert(q);

    sint32 q_idxes[2]=
      {
	horiz,
	-vert
      };

    /*
      When rotating clockwise we leave a quadrant with the x dimension
      zeroed if the quadrant is an odd number and with the y dimension
      zeroed otherwise. This assumes that quadrant 0 is the bottom
      left quadrant.
    */
    bool odd=q & 1;
    outgoing_fd.x=half_width*q_idxes[H_IDX]*odd;
    outgoing_fd.y=half_width*q_idxes[V_IDX]*!odd;

    end.x=half_width*q_idxes[H_IDX]*!odd+p_0->x;
    end.y=half_width*q_idxes[V_IDX]*odd+p_0->y;
    if(end.x==start.x) {
      continue;
    }

    ctrl=draw_determineCtrlPt(&start, &incoming_fd, &end, &outgoing_fd);
    /* One of the dimensions of the ctrl point should be equal to the
       same dimensions of the end point. If it's not there's some sort
       of floating point drift occurring which could cause the
       condition in a subsequent draw_determineCtrlPt that performs a
       floating point comparison to fail when it really
       shouldn't. Remember on of the fd args into that function is
       generated from ctrl and end.
    */
    if(ABSF(ctrl.x-end.x)>ABSF(ctrl.y-end.y)) {
      ctrl.y=end.y;
    } else {
      ctrl.x=end.x;
    }

    draw_vert p_pts[3]={
      start, ctrl, end
    };

    step=rtu_div(1, max_iter, p_globals->p_rtu);

    draw_renderCore core;
    draw_renderCoreInit(&core, &start, &ctrl, &end, &p_stroke->brush.b.w, draw_vertAngBetween(&start, &ctrl, &end), max_iter, &p_globals->canvas);
    draw_scanLog log;
    draw_scanLogInit(&log, &core, &p_stroke->brush.b, p_globals);
    draw_gradReference ref=draw_gradReferenceInterface();
    draw_gradReferenceInit(&ref, draw_blotNumCoord2Iters(&p_stroke->brush.blot), p_coord_2_iters, p_0, initial_iter, q, max_iter, &initial_pt, p_trans->p_iter_2_grad_trans);
    //render bezier here from start -> ctrl -> end
    draw_plotBez(step, p_pts, NULL, DRAW_GRADS_IF_ON_ITER_CB_ARG((width>=DRAW_TRI_THRESH?draw_triFillCb:draw_smallTriFillCb), &ref), &log, &ref.grads_if, p_globals);
    draw_completeQuadrant(&log, &ref, &p_pts[2], width, p_globals);

    draw_gradReferenceDestroy(&ref);
    draw_scanLogDestroy(&log);
    start=end;

    incoming_fd.x=draw_plotBezInitFD_1(ctrl.x, end.x);
    incoming_fd.y=draw_plotBezInitFD_1(ctrl.y, end.y);
  }
  uint32 q=cur_quadrant & 3;
  ctrl=draw_determineCtrlPt(&start, &incoming_fd, &terminal_end, p_end_fd);
  
  draw_vert p_pts[3]={
    start, ctrl, terminal_end
  };
  step=rtu_div(1, max_iter, p_globals->p_rtu);

  LOG_ASSERT(max_iter==draw_blotQuadrant2Grads(&p_stroke->brush.blot, draw_quadrant2Horiz(q), draw_quadrant2Vert(q))->max_iter, "inconsistent max iters");
  draw_renderCore core;
  draw_renderCoreInit(&core, &p_pts[0], &p_pts[1], &p_pts[2], &p_stroke->brush.b.w, draw_vertAngBetween(&p_pts[0], &p_pts[1], &p_pts[2]), max_iter, &p_globals->canvas);
  draw_scanLog log;
  draw_scanLogInit(&log, &core, &p_stroke->brush.b, p_globals);
  draw_gradReference ref=draw_gradReferenceInterface();
  draw_gradReferenceInit(&ref, draw_blotNumCoord2Iters(&p_stroke->brush.blot), p_coord_2_iters, p_0, initial_iter, q, max_iter, &initial_pt, p_trans->p_iter_2_grad_trans);
  //render bezier here from start -> ctrl -> end
  draw_plotBez(step, p_pts, NULL, DRAW_GRADS_IF_ON_ITER_CB_ARG((width>=DRAW_TRI_THRESH?draw_triFillCb:draw_smallTriFillCb), &ref), &log, &ref.grads_if, p_globals);
  draw_completeQuadrant(&log, &ref, &p_pts[2], width, p_globals);
  draw_gradReferenceDestroy(&ref);
  draw_scanLogDestroy(&log);
#endif
}

void draw_strokeStartCap(draw_stroke *p_stroke, const draw_vert *p_ctrl, const draw_vert *p_end, draw_globals *p_globals) {
  draw_vert fd_0=draw_strokeStartFD(p_stroke, &p_stroke->last, p_ctrl, p_end, &p_stroke->brush);
  draw_vert neg_fd=draw_neg(&fd_0);
  draw_strokeCap(p_stroke, &p_stroke->last, &neg_fd, &fd_0, p_globals);
}

void draw_strokeEndCap(draw_stroke *p_stroke, const draw_vert *p_end, draw_vert *p_fd_1, draw_globals *p_globals) {
  draw_vert neg_fd=draw_neg(p_fd_1);
  draw_strokeCap(p_stroke, p_end, p_fd_1, &neg_fd, p_globals);
}

void draw_strokeMoveTo(draw_stroke *p_stroke, const draw_vert *p_start, draw_globals *p_globals) {
  //see tiles/stroke.dot for state transition diagram
  //LOG_INFO("start pt: %f, %f", p_start->x, p_start->y);
  LOG_ASSERT(p_stroke->state=DRAW_STROKE_RENDERED, "can only move on rendered state");
  draw_canvasWipe(&p_globals->canvas, p_globals);
  draw_strokeContinue(p_stroke, p_start, p_globals);
}

void draw_strokeQuadTo(draw_stroke *p_stroke, const draw_vert *p_ctrl, const draw_vert *p_end, draw_globals *p_globals) {
  if(!draw_strokeRenderable(p_stroke)) {
    return;
  }

  //LOG_INFO("end pt: %f, %f", p_end->x, p_end->y);
  float32 scale=draw_brushScaleFactor(&p_stroke->brush);
  draw_vert scaled_ctrl=draw_by(p_ctrl, scale);
  draw_vert scaled_end=draw_by(p_end, scale);
  draw_vert fd_0;
  //see tiles/stroke.dot for state transition diagram
  switch(p_stroke->state) {
  case DRAW_STROKE_MOVED:
  case DRAW_STROKE_RENDERED:
    /*
    if(G_trace) {
    LOG_ERROR("start cap start");
    }
    */
    draw_strokeStartCap(p_stroke, &scaled_ctrl, &scaled_end, p_globals);
    /*
    if(G_trace) {
      LOG_ERROR("start cap end");
    }
    */
    break;
  case DRAW_STROKE_IN_STROKE:
    //render join
    fd_0=draw_strokeStartFD(p_stroke, &p_stroke->last, &scaled_ctrl, &scaled_end, &p_stroke->brush);
    draw_strokeCap(p_stroke, &p_stroke->last, &p_stroke->last_fd, &fd_0, p_globals);
    break;
  }

  draw_thickQuad(&p_stroke->last, &scaled_ctrl, &scaled_end, &p_stroke->brush.b, p_globals);

  p_stroke->last_fd=draw_strokeEndFD(p_stroke, &p_stroke->last, &scaled_ctrl, &scaled_end, &p_stroke->brush);
  p_stroke->last=scaled_end;
  p_stroke->state=DRAW_STROKE_IN_STROKE;
  draw_canvasMergeDirt(&p_globals->canvas);
}

void draw_strokeRender(draw_stroke *p_stroke, draw_globals *p_globals) {
  if(!draw_strokeRenderable(p_stroke)) {
    LOG_INFO("stroke not renderable. col: 0x%x w: %f", p_stroke->brush.b.col, p_stroke->brush.b.w.breadth);
    return;
  }

  draw_vert neg_fd;
  //see tiles/stroke.dot for state transition diagram
  switch(p_stroke->state) {
  case DRAW_STROKE_MOVED:
    //start cap
    neg_fd=draw_neg(&p_stroke->last_fd);
    draw_strokeCap(p_stroke, &p_stroke->last, &neg_fd, &p_stroke->last_fd, p_globals);
    //fall through
  case DRAW_STROKE_IN_STROKE:
    draw_strokeEndCap(p_stroke, &p_stroke->last, &p_stroke->last_fd, p_globals);
    break;
  }
  p_stroke->state=DRAW_STROKE_RENDERED;
  draw_canvasMergeDirt(&p_globals->canvas);
  //LOG_INFO("stroke renderable. col: 0x%x w: %f extant: %i, %i - %i, %i", p_stroke->brush.b.col, p_stroke->brush.b.w.breadth, p_globals->canvas.extantDirty.lt.x, p_globals->canvas.extantDirty.lt.y, p_globals->canvas.extantDirty.rb.x, p_globals->canvas.extantDirty.rb.y);
}

bool draw_strokeRenderable(const draw_stroke *p_stroke) {
  return draw_brushRenderable(&p_stroke->brush);
}

void draw_quadPtsInit(draw_quadPts *p_pts, const draw_vert *p_0, const draw_vert *p_1, const draw_vert *p_2) {
  p_pts->pts[0]=*p_0;
  p_pts->pts[1]=*p_1;
  p_pts->pts[2]=*p_2;
}

void draw_quadPtsSplit(const draw_quadPts *p_pts, float32 t, draw_quadPts *p_fst, draw_quadPts *p_snd) {
  LOG_ASSERT(t>=0 && t<=1, "out of range t: %f", t);
  draw_vert mids[2];
  for(uint32 i=0; i<2; i++) {
    mids[i]=draw_weightedMean(&p_pts->pts[i], &p_pts->pts[i+1], t);
  }
  draw_vert join=draw_weightedMean(&mids[0], &mids[1], t);

  draw_quadPtsInit(p_fst, &p_pts->pts[0], &mids[0], &join);
  draw_quadPtsInit(p_snd, &join, &mids[1], &p_pts->pts[2]);
}

draw_vert draw_vertRotate(const draw_vert *p_vert, const float32 ang, const draw_vert *p_origin) {
  draw_vert rel_vert=draw_diff(p_vert, p_origin);
  draw_vert rotated={ 
    .x=rel_vert.x*COS32(ang)-rel_vert.y*SIN32(ang), 
    .y=rel_vert.y*COS32(ang)+rel_vert.x*SIN32(ang) 
  };
  
  return draw_add(&rotated, p_origin);
}

void draw_rectIntInit(draw_rectInt *p_bnds) {
  p_bnds->lt.x=INT_MAX;
  p_bnds->rb.x=INT_MIN;
  p_bnds->lt.y=INT_MAX;
  p_bnds->rb.y=INT_MIN;

  DO_ASSERT(p_bnds->reified=false);
}

void draw_rectIntFitRectInt(draw_rectInt *p_bnds, const draw_rectInt *p_new) {
  LOG_ASSERT(!p_bnds->reified, "don't go fiddling with rect after reifying it");
  LOG_ASSERT(p_new->lt.x<=p_new->rb.x && p_new->lt.y<=p_new->rb.y, "incorrectly ordered p_new");
  if(p_new->lt.x<p_bnds->lt.x) {
    p_bnds->lt.x=p_new->lt.x;
  }
  if(p_new->rb.x>p_bnds->rb.x) {
    p_bnds->rb.x=p_new->rb.x;
  }
  if(p_new->lt.y<p_bnds->lt.y) {
    p_bnds->lt.y=p_new->lt.y;
  }
  if(p_new->rb.y>p_bnds->rb.y) {
    p_bnds->rb.y=p_new->rb.y;
  }
}

void draw_rectIntFitRect(draw_rectInt *p_bnds, const draw_rect *p_new) {
  LOG_ASSERT(!p_bnds->reified, "don't go fiddling with rect after reifying it");
  LOG_ASSERT(p_new->lt.x<=p_new->rb.x && p_new->lt.y<=p_new->rb.y, "incorrectly ordered p_new");
  if(p_new->lt.x<p_bnds->lt.x) {
    p_bnds->lt.x=p_new->lt.x;
  }
  if(p_new->rb.x>p_bnds->rb.x) {
    p_bnds->rb.x=p_new->rb.x;
  }
  if(p_new->lt.y<p_bnds->lt.y) {
    p_bnds->lt.y=p_new->lt.y;
  }
  if(p_new->rb.y>p_bnds->rb.y) {
    p_bnds->rb.y=p_new->rb.y;
  }
}

void draw_rectIntFit(draw_rectInt *p_orig, const draw_vert *p_new) {
  LOG_ASSERT(!p_orig->reified, "don't go fiddling with rect after reifying it");
  if(p_new->x<p_orig->lt.x) {
    p_orig->lt.x=p_new->x;
  } else if(p_new->x>p_orig->rb.x) {
    p_orig->rb.x=p_new->x;
  }
  if(p_new->y<p_orig->lt.y) {
    p_orig->lt.y=p_new->y;
  } else if(p_new->y>p_orig->rb.y) {
    p_orig->rb.y=p_new->y;
  }
}

void draw_rectIntShrink(draw_rectInt *p_orig, const draw_vertInt *p_boundSize) {
  LOG_ASSERT(p_boundSize->x>=0 && p_boundSize->y>=0, "p_boundSize components must not be negative");
  if(p_orig->lt.x<0) {
    p_orig->lt.x=0;
    p_orig->rb.x=MAX(p_orig->lt.x, p_orig->rb.x);
  }
  if(p_orig->lt.y<0) {
    p_orig->lt.y=0;
    p_orig->rb.y=MAX(p_orig->lt.y, p_orig->rb.y);
  }

  if(p_orig->rb.x>=p_boundSize->x) {
    p_orig->rb.x=p_boundSize->x;
    p_orig->lt.x=MIN(p_orig->lt.x, p_orig->rb.x);
  }
  if(p_orig->rb.y>p_boundSize->y) {
    p_orig->rb.y=p_boundSize->y;
    p_orig->lt.y=MIN(p_orig->lt.y, p_orig->rb.y);
  }
}

bool draw_rectIntFilled(const draw_rectInt *p_rect) {
  LOG_ASSERT(p_rect->reified, "don't check that rect has been filled without reifying it first");
  return p_rect->lt.x<=p_rect->rb.x;
}

void draw_rectIntReify(draw_rectInt *p_orig) {
  /* If one of the elements in p_orig, in either dimension has been
   *  modified since inialisation then we ensure than the max>=min for
   *  both dimensions.  
   * 
   * Also checks some invariants eg if elements in one dimension have
   * not changed since initialisation neither should the elements of
   * the other dimension, .
   */
  DO_ASSERT(p_orig->reified=true);
  sint32 *p_bounds[2][2]={
    {
      &p_orig->lt.x, 
      &p_orig->rb.x
    },
    {
      &p_orig->lt.y, 
      &p_orig->rb.y
    }
  };

  uint32 dim=0;
  sint32 *p_min=p_bounds[dim][0];
  sint32 *p_max=p_bounds[dim][1];
  if(*p_min==INT_MAX && *p_max==INT_MIN) {
    LOG_ASSERT(*p_bounds[1-dim][0]==INT_MAX && *p_bounds[1-dim][1]==INT_MIN, "if one dim is freshly initialised so must the other");
    return;
  }
  if(*p_min>*p_max) {
    LOG_ASSERT(*p_min==INT_MAX || *p_max==INT_MIN, "invalid rect - if the dimension isn't in ascending order then one of the values in that dimension must not have been assigned a value: dim %u, min %i, max %i", dim, *p_min, *p_max);
    sint32 real_val_idx=*p_min==INT_MAX; //idx to an initialised value for this dim, if one exists
    *p_bounds[dim][1-real_val_idx]=*p_bounds[dim][real_val_idx];
    uint32 other_dim=1-dim;
    LOG_ASSERT(*p_bounds[other_dim][0]>*p_bounds[other_dim][1] && (*p_bounds[other_dim][0]==INT_MAX)==real_val_idx, "if one of the dimensions' values aren't in ascending order then neither can the other dimensions', since we never fit one dimension's value on its own. Also the same values in the both dimensions must be not have been assigned values. dim %u, min %i, max %i, other min %i, other max %i", dim, *p_min, *p_max, *p_bounds[other_dim][0], *p_bounds[other_dim][1]);
    *p_bounds[other_dim][1-real_val_idx]=*p_bounds[other_dim][real_val_idx];
  }
}

sint8 *draw_test(void) {
  mu_run_test(draw_test_proxInit);
  mu_run_test(draw_test_blotBBoxDims);
  mu_run_test(draw_test_findFirstIter4Row);
  mu_run_test(draw_test_gradIterInc);
  mu_run_test(draw_test_vertAng);
  mu_run_test(draw_test_vertNonReflexAng);
  mu_run_test(draw_test_horizVert2Quadrant);
  mu_run_test(draw_test_landmarkPt);
  mu_run_test(draw_test_canvasFindStdScalingFactor);
  return 0;
}
