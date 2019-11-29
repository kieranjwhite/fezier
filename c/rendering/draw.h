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

#ifndef DRAW_H
#define DRAW_H

#include <limits.h>
#include <math.h>

#include "../rtu.h"
#include "../types.h"

//due to rounding errors some asserts can fail, so we allow some leeway by including a fuzz factor in the condition
#define DRAW_FUZZ_FACTOR 0.4

#define DRAW_INNER_SEG_NULL 0
#define DRAW_INNER_SEG_X_DOM 1
#define DRAW_INNER_SEG_Y_DOM 2

#define DRAW_MIN_OPACITY 0x01000000
#define DRAW_OPACITY_MASK 0xff000000
#define DRAW_OPACITY_INVERSE_MASK (~0xff000000)
#define DRAW_OPACITY_SHIFT 0x18
#define DRAW_OPACITY_SCALED_SHIFT 0x8
#define DRAW_OPACITY_MAX_SCALED ((0x1<<(DRAW_OPACITY_SCALED_SHIFT+8))-1)

#define DRAW_DIR_SHIFT (((sizeof(uint32)-1)*8)+5)
#define DRAW_DIR_INC (1<<DRAW_DIR_SHIFT)

#define DRAW_DIR_U 0
#define DRAW_DIR_UR ((uint32)(DRAW_DIR_U+DRAW_DIR_INC))
#define DRAW_DIR_R ((uint32)(DRAW_DIR_UR+DRAW_DIR_INC))
#define DRAW_DIR_DR ((uint32)(DRAW_DIR_R+DRAW_DIR_INC))
#define DRAW_DIR_D ((uint32)(DRAW_DIR_DR+DRAW_DIR_INC))
#define DRAW_DIR_DL ((uint32)(DRAW_DIR_D+DRAW_DIR_INC))
#define DRAW_DIR_L ((uint32)(DRAW_DIR_DL+DRAW_DIR_INC))
#define DRAW_DIR_UL ((uint32)(DRAW_DIR_L+DRAW_DIR_INC))

#define DRAW_DIV_LIMIT 0x1000
#define DRAW_ATAN_DIVISORS 8

//fixed point arithmetic multiplies vals by this value before casting
#define DRAW_PROXIMITY_FIXED_POINT 16
#define DRAW_PROXIMITY_BITS 16
#define DRAW_PROXIMITY_MASK ((1<<DRAW_PROXIMITY_BITS)-1)
#define DRAW_PROXIMITY_SHIFT(RANK) (DRAW_PROXIMITY_BITS*RANK)
#define DRAW_PROXIMITY_EXTRACT(RANK, ELEMENT) ((ELEMENT >> DRAW_PROXIMITY_SHIFT(RANK)) & DRAW_PROXIMITY_MASK)

#define DRAW_PROXIMITY_QUADRANT_BITS 2
#define DRAW_PROXIMITY_ITER_BITS (DRAW_PROXIMITY_BITS-DRAW_PROXIMITY_QUADRANT_BITS)
#define DRAW_PROXIMITY_QUADRANT_MASK (((1<<DRAW_PROXIMITY_QUADRANT_BITS)-1)<<DRAW_PROXIMITY_ITER_BITS)
#define DRAW_PROXIMITY_ITER_MASK ((1<<DRAW_PROXIMITY_ITER_BITS)-1)
#define DRAW_PROXIMITY_ORIGIN_OFFSET 3
#define DRAW_BLOT_NUM_COORD_TO_ITERS 2

#define DRAW_SCAN_SPAN_ITER_BOUND_START 0x01
#define DRAW_SCAN_SPAN_ITER_BOUND_END 0x10

#define DRAW_SMALLEST_VISIBLE_OPACITY_CHANGE_PER_STD_PIXEL 6

typedef struct {
  float32 x;
  float32 y;
} draw_vert;

typedef struct {
  draw_vert *p_pt;
  draw_vert pt;
} draw_vertNullable; //the only reason this struct exists is so that I can define a NULL draw_vert and not be reliant on a pointer to an external, possibly transient (i.e. on the heap) draw_vert instance.

typedef struct {
  draw_vert end; //vertex at end of draw_grad, if y>0, if y==0 then we look at the x value
  
  //DO_ASSERT(bool reified);

  draw_vertNullable cached;
  uint32 cached_q;
} draw_landmark;

typedef struct {
  draw_landmark *p_cur;
  draw_landmark *p_last;
  draw_landmark *p_first;
} draw_landmarkSequence;

typedef struct {
  DO_ASSERT(uint32 idx); //iter of this grad and its index within draw_scanLog.p_iter_2_grad

  float32 default_pos_inc; //how much pos increases for every change of +1 in x where draw_grad.on_x is true or y othersize
  draw_vert last_mid;
  draw_vert mid;
  draw_vert half_delta;
  float32 slope_recip; //actually this is only the slope reciprocal where x is dominant, otherwise x is the denominator and then it's the slope
  float32 c5; //(bx-ax) in link's top answer - see https://stackoverflow.com/questions/3461453/determine-which-side-of-a-line-a-point-lies
  float32 c6; //(by-ay) in link's top answer - see https://stackoverflow.com/questions/3461453/determine-which-side-of-a-line-a-point-lies
  draw_vert fd_signed;
  
  DO_ASSERT(bool fd_recorded);
  bool initialised;
  bool on_x;
  bool has_last_mid;

  bool bisect_initialised;
  draw_vert bisect_fd_signed; //required in draw_coreRender when deciding when to reorder vertices
  draw_vert bisect_half_delta;
  bool bisect_on_x;
  float32 bisect_slope_recip; //actually this is only the slope reciprocal where x is dominant, otherwise x is the denominator and then it's the slope
  float32 bisect_default_pos_inc; //how much pos increases for every change of +1 in x where draw_grad.on_x is true or y othersize
} draw_grad;

typedef struct {
  sint32 x;
  sint32 y;
} draw_vertInt;

typedef struct {
  draw_vertInt lt;
  draw_vertInt rb;

  DO_ASSERT(bool reified);
} draw_rectInt;

typedef struct {
#define DRAW_CANVAS_NUM_MAG_FACTORS 5
  uint32 std_mag_factors[DRAW_CANVAS_NUM_MAG_FACTORS];
  float32 std_scaling_factors[DRAW_CANVAS_NUM_MAG_FACTORS]; //each element in this should be the reciprocal of its corresponding element in G_draw_std_mag_factors
  uint32 scaling_idx;
  

  uint32 renderedW; //this should remain constant for a given device (we also change it when user switches to a monitor with a different resolution, but we do so by reinitialising everything - so that doesn't really count)
  uint32 renderedH; //this should remain constant for a given device (same proviso as for renderedW though)
  uint32 w; //this will change depending on the blur_width of current brush
  uint32 h; //this will change depending on the blur_width of current brush
  float32 devicePixelRatio; //pixel density of a screen, e.g. 1 for most desktop monitors, 2 for 'retina' type displays. A negative value means that scaling optimisation is disabled for this canvas and dependent brushes.
  draw_rectInt dirty;
  draw_rectInt extantDirty;
  uint32 *p_bitmap;

  uint32 last_w; //the w value associated with the previous brush. Needed for draw_cavasWipe
  uint32 last_h; //the h value associated with the previous brush. Needed for draw_cavasWipe
  
  bool stop;
} draw_canvas;

typedef struct {
  draw_vert lt;
  draw_vert rb;
} draw_rect;

typedef struct {
  draw_vert pts[3];
  
  draw_vert fd_init_val;
  draw_vert fd_delta;
  draw_vert fd_t;
  draw_vert fd_delta_by_step;
  draw_vert fds[2]; //0th is the derivative of the current iter in the quadratic bezier during FDGen, 1st is fd_1

  float32 step;
  float64 step_sum;
} draw_quad;

typedef struct {
  DO_ASSERT(uint32 idx); //iter of this grad and its index within draw_scanLog.p_iter_2_grad

  draw_vert mid; //mid point between translated bounds
  //cross-product formula constants - see https://stackoverflow.com/questions/3461453/determine-which-side-of-a-line-a-point-lies
  float32 c6_ax_less_c5_ay;
  draw_vert offset;
  
  bool initialised;
} draw_gradTranslated;

typedef struct {
  float32 breadth;
  float32 width; //total rendered width of stroke (ie the diameter, saved_width + (blur width>>1))
  float32 width_squared;
  float32 width_recip; //1/draw_strokeWidth.width
  float32 half_width; //draw_strokeWidth.width*0.5
  float32 blur_width; //number of pixels between brush's opacity and complete transparancy. We optimise by scaling on hi-dpi screens so this value can be less than rendered_blur_width when scaling is in effet
  float32 rendered_blur_width; //number of pixels on screen between brush's opacity and complete transparancy
  float32 half_blur_width; //blur width * 0.5
  float32 blurred_prop; //draw_strokeWidth.half_blur_width/draw_strokeWidth.width
  float32 blurred_prop_recip; //1/draw_strokeWidth.blurred_prop
  float32 half_blur_width_recip; //1/draw_strokeWidth.half_blur_width
} draw_strokeWidth;

typedef struct {
  uint32 opacity_scaled;
  sint32 p_incs_per_pos[4]; //how much a change of 1 along the iter hypothenuse changes the opacity

  //compare threshes to positional sum
  float32 threshes[4]; //pos values. 0th full opacity start, 1st full opacity end, 2nd no opacity start, 3rd UINT_MAX, but if any region has a width of zero then element will not be provided and any later elements will be shifted down to fill the hole
  float32 start_threshes[4];
  uint32 start_opacity[4]; //the 4 elements here will always be populated and are as follows 0th opacity from start, 1st opacity at start of full opacity region, 2nd opacity at end of full opacity region, 3rd opacity at end

  float32 bisect_threshes[4]; //pos values. 0th full opacity start, 1st full opacity end, 2nd no opacity start, 3rd UINT_MAX, but if any region has a width of zero then element will not be provided and any later elements will be shifted down to fill the hole
  float32 bisect_start_threshes[4];
  uint32 bisect_start_opacity[4]; //the 4 elements here will always be populated and are as follows 0th opacity from start, 1st opacity at start of full opacity region, 2nd opacity at end of full opacity region, 3rd opacity at end
  sint32 bisect_incs_per_pos[4]; //how much a change of 1 along the iter hypothenuse changes the opacity
  
} draw_gradConsts;

typedef struct {
  DO_ASSERT(uint32 initialised);
  uint32 *p_xy_2_iter;
  uint32 nearest_landmark_size;
  //draw_vert cen;
  uint32 span;
  uint32 rel_origin;
  uint32 max_dist_squared;
} draw_prox;

typedef struct {
  float32 scaling_factor; // 1>=scaling_factor>0, smaller on high dpi screens and larger, blurrier brushes. canvas width and height and brush blur_width and breadth are multiplied by this amount for scaling optimisation
  uint32 mag_factor; //reciprocal of scaling_factor
  uint32 col;
  uint32 rgb; //col & DRAW_OPACITY_INVERSE_MASK
  uint32 opacity; //col & DRAW_OPACITY_MASK
  float32 opacity_frac; // 1/(col >> DRAW_OPACITY_SHIFT)
  draw_strokeWidth w;
  draw_gradConsts grad_consts;
  draw_prox proximity;
} draw_scanBrushLog; //fields here don't change if the brush doesn't

 typedef struct {
  draw_vert pts[3];
} draw_quadPts;

typedef struct {
  sint32 start;
  sint32 end;
} draw_range;

typedef struct {
  uint32 q;
  uint32 iter;
} draw_iterRange;

typedef struct {
  draw_iterRange iter_range;
  uint32 q;
  uint32 iter;
  sint32 start_x;
  sint32 end_x;
  draw_grad *p_iter_2_grad;
  draw_grad *p_grad;
  uint32 row_start_offset;
  draw_vert *p_anchor;
} draw_horizSegment;

typedef struct draw_gradsIf draw_gradsIf;

typedef uint32 (draw_onGradsIfMaxIter) (draw_gradsIf *);
typedef draw_grad *(draw_onGradsIfGradPtr) (draw_gradsIf *, uint32 q);
typedef draw_iterRange (draw_onGradsIfIterRange) (draw_gradsIf *);
typedef draw_gradTranslated *(draw_onGradsIfTranslated) (draw_gradsIf *, const uint32 q, const uint32 iter);

				       struct draw_gradsIf {
					 draw_onGradsIfMaxIter * p_max_iter_cb;
					 draw_onGradsIfGradPtr * p_grad_ptr_cb;
					 draw_onGradsIfIterRange * p_iter_range_cb;
					 draw_onGradsIfTranslated * p_trans_cb;
				       };

typedef struct {
  draw_gradTranslated *p_iter_2_grad_trans;  //fields defining a translation of a particular draw_grad (ie one with a matching idx field).
  uint32 num_iters; //space was allocated in p_iter_2_grad_trans for this many elements
  DO_ASSERT(uint32 max_iter;);
} draw_gradTranslateds;

typedef struct {
  draw_gradsIf grads_if;
  draw_grad *p_iter_2_grad;
  uint32 max_iter;
  draw_iterRange range;
  draw_gradTranslateds trans; //used by draw_renderCore

  draw_iterRange bisect_range; //needs to be distinct from range because a bisector could be in a different quadrant to its corresponding grad
} draw_grads;

typedef struct {
  draw_grad *p_iter_2_grad;
  uint32 num_iters;
} draw_gradsArray;

typedef struct {
  rtu_globals *p_rtu;
  /*
  uint32* p_bitmap;
  draw_vertInt draw_dim;
  */
  draw_canvas canvas;
  draw_vert null_vert;
  uint32 pp_y_dirs[3][3];
  draw_vert draw_origin;
  uint64 draw_pixels;
  uint64 draw_iter_updates;
} draw_globals;

typedef void (draw_onIterRowCb)(const draw_scanBrushLog *p_log, const sint32 start_x, const sint32 end_x, const uint32 y, draw_globals *p_globals, draw_gradsIf *p_grads_if);

#define DRAW_ON_ITER_ROW_CB(cb, arg) typesafe_cb_cast(void (*)(const draw_scanBrushLog *p_log, const sint32 start_x, const sint32 end_x, const uint32 y, draw_globals *p_globals, draw_gradsIf *p_grads_if), void (*)(const draw_scanBrushLog *p_log, const sint32 start_x, const sint32 end_x, const uint32 y, draw_globals *p_globals, __typeof__((arg))), (cb))

typedef struct draw_curveIf draw_curveIf;

typedef void (draw_onCurveIfInit) (draw_curveIf *, const float32, const draw_vert *p_0, const draw_vert *p_1, const draw_vert *p_2, const draw_globals *p_globals);
typedef draw_vert (draw_onCurveIfFXT) (draw_curveIf *);
typedef draw_vert *(draw_onCurveIfFD)(draw_curveIf *);
typedef draw_vert (draw_onCurveIfNext)(draw_curveIf *, const draw_globals *p_globals);
typedef draw_vert *(draw_onCurveIfFD0)(draw_curveIf *);

				      struct draw_curveIf {
					draw_onCurveIfInit *const p_init_cb;
					draw_onCurveIfFXT *const p_fxt_cb;
					draw_onCurveIfFD *const p_fd_cb;
					draw_onCurveIfFD0 *const p_fd_0_cb;
					draw_onCurveIfNext *const p_next_cb;

				      };

#define DRAW_CURVE_IF_INIT_CB_ARG(cb, arg)					\
  typesafe_cb_postargs(void, draw_curveIf *, (cb), (arg), const float32 step, const draw_vert *p_0, const draw_vert *p_1, const draw_vert *p_2, const draw_globals *p_globals)
#define DRAW_CURVE_IF_FXT_CB_ARG(cb, arg)				\
  typesafe_cb_cast(draw_vert (*)(draw_curveIf *), draw_vert (*)(__typeof__((arg))), (cb))
#define DRAW_CURVE_IF_FD_CB_ARG(cb, arg)					\
  typesafe_cb_cast(draw_vert *(*)(draw_curveIf *), draw_vert *(*)(__typeof__((arg))), (cb))
#define DRAW_CURVE_IF_NEXT_CB_ARG(cb, arg)					\
  typesafe_cb_cast(draw_vert (*)(draw_curveIf *, const draw_globals *p_globals), draw_vert (*)(__typeof__((arg)), const draw_globals *p_globals), (cb))
#define DRAW_CURVE_IF_FD_0_CB_ARG(cb, arg)					\
  typesafe_cb_cast(draw_vert *(*)(draw_curveIf *), draw_vert *(*)(__typeof__((arg))), (cb))

typedef struct {
  draw_curveIf curve_if;

  draw_quad quad;
  draw_vert f_x_t;
  draw_vert fd_times_t;
  draw_vert c_0;
  draw_vert c_1;
  draw_vert fd_0;

  draw_grad *p_iter_2_grad;
  uint32 max_iter;

  DO_ASSERT(float32 t);
  DO_ASSERT(float32 step);
  const draw_vert *p_points[3];
} draw_bez;

#define DRAW_GRADS_IF_MAX_ITER_CB_ARG(cb, arg) typesafe_cb_cast(uint32 (*)(draw_gradsIf *), uint32 (*)(__typeof__((arg))), (cb))
#define DRAW_GRADS_IF_GRAD_PTR_CB_ARG(cb, arg) typesafe_cb_cast(draw_grad *(*)(draw_gradsIf *, uint32 q), draw_grad *(*)(__typeof__((arg)), uint32 q), (cb))

#define DRAW_GRADS_IF_ON_ITER_CB_ARG(cb, arg) typesafe_cb_cast(void (*)(draw_scanLog *p_log, draw_gradsIf *, const draw_vert *p_last, const draw_vert *p_0,  const draw_vert *p_fd_times_t, const uint32, draw_globals *p_globals), void (*)(draw_scanLog *p_log, __typeof__((arg)), const draw_vert *p_last, const draw_vert *p_0,  const draw_vert *p_fd_times_t, const uint32, draw_globals *p_globals), (cb))

#define DRAW_GRADS_IF_ITER_RANGE_CB_ARG(cb, arg) typesafe_cb_cast(draw_iterRange (*)(draw_gradsIf *), draw_iterRange (*)(__typeof__((arg))), (cb))

#define DRAW_GRADS_IF_TRANS_CB_ARG(cb, arg) typesafe_cb_cast(draw_gradTranslated *(*)(draw_gradsIf *, const uint32 q, const uint32 iter), draw_gradTranslated *(*)(__typeof__((arg)), const uint32 q, const uint32 iter), (cb))

typedef struct {
  float32 start;
  float32 end;
} draw_rangeFlt;

typedef struct {
  uint32 q; //quadrant
  uint32 iter; //iter (idx within quadrant)
} draw_iterKey;

typedef struct {
  draw_grads *p_grad_agg;

  float32 max_on_y_x_coord;
  float32 min_on_y_y_coord;
  uint32 num_coords; //num elements of p_coord_2_iter
  uint32 *p_coord_2_iter;
  draw_vert origin;

  sint32 right;
  sint32 down;
  float32 abs_max_y_x;

  /* p_grad_agg range of iters can subsume the range of iters returned
     by this instance. The range of distinct iters that can be
     returned by this instance is 1+end_iter-start_iter
   */
  uint32 start_iter; 
  uint32 end_iter;
} draw_coordToIter;

typedef struct {
  draw_gradsIf grads_if;

  draw_coordToIter *p_coord_2_iters;
  draw_vert center;
  draw_iterRange range;

  uint32 last_iter; //used to ensure we don't plot triangle for same iter twice
  draw_vertNullable last_pt;
  draw_gradTranslated *p_iter_2_grad_trans;
  uint32 start_quadrant; //first element in p_iter_2_grad_trans should have iter==0 and belong to quadrant 'start_quadrant'
  uint32 max_iter; //minimum valid range of p_iter_2_grad_trans index,
		   //iter is iter>=0 && iter<=max_iter. However not
		   //all *p_iter_2_grad_trans[iter] instances will
		   //necessarily have been initialised
} draw_gradReference;

typedef struct {
  draw_grads slope_sign_2_iter_2_grads[2];
  draw_gradsArray grads_arr;

  float32 breadth;
  float32 blur_width;
  
  draw_vert origin; //the point at which our reference blot is assumed to exist. Blots that are rendered as stroke caps will be defined partially in terms of translations from our draw_blot.left_2_down_2_iter_2_grads elements. Our draw_grads defined in this array all have draw_blot.origin as their mid point.
  draw_coordToIter coord_2_iters[DRAW_BLOT_NUM_COORD_TO_ITERS];
  draw_gradTranslateds trans; //draw_gradReference.p_iter_2_grand_trans points to this, used when rendering caps
} draw_blot;

typedef struct {
  draw_scanBrushLog b;
  draw_blot blot;
} draw_brush;

typedef struct {
  draw_vert last;

  draw_brush brush;
  uint32 buf_width;
  uint32 buf_height;

  draw_vert last_fd;
#define DRAW_STROKE_IN_STROKE 0x00
#define DRAW_STROKE_MOVED 0x01
#define DRAW_STROKE_RENDERED 0x02
  uint32 state; //if this is true, the next segment (rendered in draw_strokeQuadTo) will include a full start-cap. Otherwise only the join will be generated
} draw_stroke;

typedef struct {
  float32 pos; //position on iter line. 0<=pos<p_log->w.width
  sint32 opacity; //opacity=(val<<0x18), 0<=val<=0xff
  uint32 idx; //index of within pos threshold or opacity inc arrays

#define DRAW_GRAD_PT_MAX_START 0
#define DRAW_GRAD_PT_MIN_END(W) ((W))
  DO_ASSERT(draw_rangeFlt pos_range); //max and min pos associated with this column
} draw_gradPt;

typedef struct {
  draw_vert pt_diff;
  sint32 x_dist;
  sint32 y_dist;
  uint32 x_steps;
  uint32 y_steps;

  const draw_vert *p_0;
  const draw_vert *p_1;
} draw_seg;

typedef struct {
  draw_range xs;
  draw_range iters;
} draw_scanSpan;

typedef struct {
  DO_ASSERT(uint32 idx);

  uint32 size; //num elements in xs
  draw_scanSpan span[4]; //all elements are in order of x, span[i].iters.start is the smallest iter of the ith range, span[i].iters.end is the largest iter of the ith range
} draw_scanEnd;

typedef struct {
  float32 inc;
  float32 pos;
} draw_end;

typedef struct {
  draw_vertInt p;
  uint32 fill_dir;
} draw_posState;

typedef struct {
  sint32 *p_y_2_starts;
  sint32 *p_y_2_ends;
} draw_decomposedBounds;

typedef struct {
  sint32 min_y;

  draw_decomposedBounds xs_pairs;
  uint32 size; //number of elements in each of the two xs_pair fields

  bool highly_acute; //If a curve if very acute then there is the possibility that the angle between two successive draw_grads is sufficiently large at to lead to rendering artifacts. We special case this scenario in draw_coreRender(), checking this flag first.
  float32 last_ang;
  
} draw_scanFillLog; //fields here are required by our most recent draw_grad quadrilateral filling algorithm. The fields' values change between renderings.

typedef struct {
  draw_scanBrushLog *p_b;
  draw_scanFillLog f;
} draw_scanLog;

#define DRAW_CAP_BOUNDARY_CMP_CB_ARG(cb)					\
  typesafe_cb_cast(sint32 (*)(const void *, const void *), sint32 (*)(const draw_capBoundary *, const draw_capBoundary *), (cb))

typedef struct {
  float32 x;
  bool start_cap; //true means this x is the left-most x coord of a cap, false means it' the right-most x coord of a cap
} draw_capBoundary;

typedef struct {
  sint32 not_checking; //if not_checking is 0, we're checking, if it's +ve we're not checking. It should never be -ve
  draw_capBoundary bnds[4];
} draw_capBoundaries;

typedef void (draw_onPtCb)(const float32, const float32, draw_scanFillLog *, const draw_gradsIf *, const uint32, draw_globals *p_globals);
typedef void (draw_onIterCb)(draw_scanLog *p_log, draw_gradsIf *, const draw_vert *p_last, const draw_vert *p_0,  const draw_vert *p_fd_times_t, const uint32, draw_globals *p_globals);
typedef bool (draw_onSegCb)(const draw_vert *p_0, const draw_vert *p_1, draw_onPtCb *p_callback, draw_scanFillLog *p_arg, const draw_gradsIf *p_grad_agg, const uint32 iter, draw_globals *p_globals);
			   //typedef sint32 (draw_onFeatherCb)(const uint32 start_iter, const float32, uint32, draw_vert *, draw_onPtCb *p_on_pt_cb, draw_onIterCb *p_on_iter_cb, draw_scanLog *p_arg, draw_grads *p_grad_agg, const bool renderable, draw_globals *p_globals);

typedef struct {
  draw_onPtCb *p_on_pt_cb;
  draw_onIterCb *p_on_iter_cb;
} draw_featherCbs;

typedef struct {
  draw_vert spine[3];
  draw_strokeWidth *p_w;
  float32 ang; //needed for draw_calcMaxIter
  uint32 max_iter;
  float32 step;
  //uint32 real_max_y;
  //uint32 real_min_y;
  sint32 min_y;
  uint32 height;
} draw_renderCore;   //needed by new renderer

typedef struct {
  draw_renderCore core;
} draw_render;


void draw_init(const uint32 w, const uint32 h, const float32 devicePixelRatio, uint32 *p_pixels, draw_globals *p_globals);
void draw_canvasResetDirty(draw_canvas *p_canvas);
bool draw_canvasSetup(draw_canvas *p_canvas, uint32 w, uint32 h, uint32* p_pixels);
bool draw_horizSegmentInit(draw_horizSegment *p_seg, const draw_scanBrushLog *p_b, const sint32 first_x, const sint32 last_x, const uint32 y, draw_globals *p_globals, draw_gradsIf *p_grads_if);

inline void draw_atOffset(uint32 offset, uint32 col, const draw_globals *p_globals) {
  *(p_globals->canvas.p_bitmap+offset)=col;
}

inline uint32 draw_get(uint32 offset, const draw_globals *p_globals) {
  return *(p_globals->canvas.p_bitmap+offset);
}

inline void draw_canvasClear(draw_canvas *p_canvas, uint32 offset, uint32 len) {
  rtu_memZero(p_canvas->p_bitmap+offset, len*sizeof(uint32));
}

uint32 draw_unshiftDir(uint32 dir);
void draw_gradAddBound(draw_grad *p_grad, const bool bound_idx, const draw_vert *p_0);

inline uint32 draw_canvasRenderedWidth(const draw_canvas *p_canvas) {
  return p_canvas->renderedW;
}

inline uint32 draw_canvasWidth(const draw_canvas *p_canvas) {
  return p_canvas->w;
}

inline uint32 draw_canvasRenderedHeight(const draw_canvas *p_canvas) {
  return p_canvas->renderedH;
}

inline uint32 draw_canvasHeight(const draw_canvas *p_canvas) {
  return p_canvas->h;
}

void draw_rectIntFit(draw_rectInt *p_orig, const draw_vert *p_new);
inline void draw_canvasMarkDirty(draw_canvas *p_canvas, const draw_vert *p_vert) {
  //LOG_INFO("marking dirty vert. orig: %i, %i - %i, %i new: %f, %f", p_canvas->dirty.lt.x, p_canvas->dirty.lt.y, p_canvas->dirty.rb.x, p_canvas->dirty.rb.y, p_vert->x, p_vert->y);
  draw_rectIntFit(&p_canvas->extantDirty, p_vert);
  //LOG_INFO("marking dirty vert (0x%p). after: %i, %i - %i, %i", &p_canvas->dirty, p_canvas->dirty.lt.x, p_canvas->dirty.lt.y, p_canvas->dirty.rb.x, p_canvas->dirty.rb.y);
}

void draw_rectIntFitRectInt(draw_rectInt *p_bnds, const draw_rectInt *p_new);
void draw_rectIntShrink(draw_rectInt *p_orig, const draw_vertInt *p_boundSize);
inline void draw_canvasMergeDirt(draw_canvas *p_canvas) {
  //LOG_INFO("merge dirty rect. orig: %i, %i - %i, %i", p_canvas->dirty.lt.x, p_canvas->dirty.lt.y, p_canvas->dirty.rb.x, p_canvas->dirty.rb.y);
  draw_vertInt bounds={ .x=p_canvas->w-1, .y=p_canvas->h-1 };
  draw_rectIntShrink(&p_canvas->extantDirty, &bounds);
  draw_rectIntFitRectInt(&p_canvas->dirty, &p_canvas->extantDirty);
  //LOG_INFO("merge dirty rect (0x%p). after: %i, %i - %i, %i", &p_canvas->dirty, p_canvas->dirty.lt.x, p_canvas->dirty.lt.y, p_canvas->dirty.rb.x, p_canvas->dirty.rb.y);
}

void draw_rectIntFitRect(draw_rectInt *p_bnds, const draw_rect *p_new);
inline void draw_canvasMarkDirtyRect(draw_canvas *p_canvas, const draw_rect *p_new) {
  //LOG_INFO("marking dirty rect. orig: %i, %i - %i, %i new: %f, %f - %f, %f", p_canvas->dirty.lt.x, p_canvas->dirty.lt.y, p_canvas->dirty.rb.x, p_canvas->dirty.rb.y, p_new->lt.x, p_new->lt.y, p_new->rb.x, p_new->rb.y);
  draw_rectIntFitRect(&p_canvas->extantDirty, p_new);
  //LOG_INFO("marking dirty rect (0x%p). after: %i, %i - %i, %i", &p_canvas->dirty, p_canvas->dirty.lt.x, p_canvas->dirty.lt.y, p_canvas->dirty.rb.x, p_canvas->dirty.rb.y);
}

inline uint32 draw_blotHalfWidth2BBoxLen(const float32 half_width) {
  return CEIL32(half_width);
}

inline uint32 draw_bboxLen(const draw_vert *p_box_dims) {
  return draw_blotHalfWidth2BBoxLen((p_box_dims->x+p_box_dims->y)*0.5);
}

inline float32 draw_polynomial(uint32 n, uint32 i, float32 t) {
  LOG_ASSERT(t>=0 && t<=1, "out of range t: %f", t);
  LOG_ASSERT(n>=i, "invalid args n %u, i %u", n, i);
  return POW32(1-t, n-i)*POW32(t, i);
}

inline float32 draw_bezierTerm(uint32 n, uint32 i, float32 t, float32 w, const draw_globals *p_globals) {
  LOG_ASSERT(t>=0 && t<=1, "out of range t: %f", t);
  LOG_ASSERT(n>=i, "invalid args n %u, i %u", n, i);
  
  return rtu_nCr(n, i, p_globals->p_rtu)*draw_polynomial(n,i,t)*w;
}

inline draw_vert draw_bezierQuad(float32 t, const draw_vert *p_ws[], const draw_globals *p_globals) {
  LOG_ASSERT(t>=0 && t<=1, "out of range t: %f", t);
  draw_vert result={
    .x=draw_bezierTerm(2, 0, t, p_ws[0]->x, p_globals)+draw_bezierTerm(2, 1, t, p_ws[1]->x, p_globals)+draw_bezierTerm(2, 2, t, p_ws[2]->x, p_globals), 
    .y=draw_bezierTerm(2, 0, t, p_ws[0]->y, p_globals)+draw_bezierTerm(2, 1, t, p_ws[1]->y, p_globals)+draw_bezierTerm(2, 2, t, p_ws[2]->y, p_globals)
  };
  return result;
}

inline draw_vert draw_norm(const draw_vert *p_tan) {
  draw_vert norm={ .x=-p_tan->y, .y=p_tan->x };
  return norm;
}

inline uint32 draw_mem_inc_idx(const uint32 idx) {
  LOG_ASSERT(false, "modulo operator used");
  return (idx+1) % 3;
}

inline float32 draw_manhattanDist(const draw_vert *p_0, const draw_vert *p_1) {
  return ABSF(p_1->x-p_0->x)+ABSF(p_1->y-p_0->y);
}

inline float32 draw_euclideanDistSquared(const draw_vert *p_0, const draw_vert *p_1) {
  float32 x_delta=p_1->x-p_0->x, y_delta=p_1->y-p_0->y;
  return x_delta*x_delta+y_delta*y_delta;
}

inline float32 draw_euclideanDist(const draw_vert *p_0, const draw_vert *p_1) {
  return SQRT32(draw_euclideanDistSquared(p_0, p_1));
}

inline draw_vert draw_copy(const draw_vert *p_0) {
  draw_vert copy={.x=p_0->x, .y=p_0->y};
  return copy;
}

inline draw_vert draw_add(const draw_vert *p_v0, const draw_vert *p_v1) {
  draw_vert res={ .x=p_v0->x+p_v1->x, .y=p_v0->y+p_v1->y };
  return res;
}

inline draw_vert draw_neg(const draw_vert *p_v0) {
  draw_vert res={ .x=-p_v0->x, .y=-p_v0->y };
  return res;
}

inline draw_vert draw_abs(const draw_vert *p_v0) {
  draw_vert res={ .x=ABSF(p_v0->x), .y=ABSF(p_v0->y) };
  return res;
}

inline draw_vert draw_diff(const draw_vert *p_v0, const draw_vert *p_v1) {
  draw_vert res={ .x=p_v0->x-p_v1->x, .y=p_v0->y-p_v1->y };
  return res;
}

inline draw_vert draw_by(const draw_vert *p_v0, const float32 scalar) {
  draw_vert res={ .x=p_v0->x*scalar, .y=p_v0->y*scalar };
  return res;
}

inline draw_vert draw_byOneByOne(const draw_vert *p_v0, const draw_vert *p_multiplier) {
  draw_vert res={ .x=p_v0->x*p_multiplier->x, .y=p_v0->y*p_multiplier->y };
  return res;
}

inline draw_vert draw_divide(const draw_vert *p_v0, const float32 scalar) {
  LOG_INFO("draw_divide invoked");
  LOG_ASSERT(scalar!=0, "divide by zero");
  draw_vert res={ .x=p_v0->x/scalar, .y=p_v0->y/scalar }; //TODO division
  return res;
}

inline float32 draw_mag(const draw_vert *p_vert) {
  LOG_ASSERT(p_vert->x*p_vert->x+p_vert->y*p_vert->y>=0, "sqrt of negative number x %f, y %f", p_vert->x, p_vert->y);
  return sqrt(p_vert->x*p_vert->x+p_vert->y*p_vert->y); //TODO sqrt
}

inline draw_vert draw_fd2Tan(const draw_vert *p_fd, float32 width) {
  float32 mag=draw_mag(p_fd);
  LOG_ASSERT(mag!=0, "division by zero");
  float32 len_adj=width/mag; //TODO division
  return draw_by(p_fd, len_adj);
}

inline draw_rect draw_boundingBox(const draw_vert *p_0, const draw_vert *p_delta) {
  draw_rect box={ .lt=draw_add(p_0, p_delta), .rb=draw_diff(p_0, p_delta) };
  return box;
}

inline draw_rect draw_calcTerminalBoxFromTan(const draw_vert *p_0, const draw_vert *p_tan) {
  draw_vert norm=draw_norm(p_tan); //gets the normal (ie perpendicular)
  return draw_boundingBox(p_0, &norm);
}

inline draw_rect draw_calcTerminalBox(const draw_vert *p_0, const draw_vert *p_fd, const float32 half_width) {
  draw_vert tan=draw_fd2Tan(p_fd, half_width); //normalises *p_fd's magnitude
  draw_vert norm=draw_norm(&tan); //gets the normal (ie perpendicular)
  return draw_boundingBox(p_0, &norm);
}

inline draw_vert draw_vertRecip(const draw_vert *p_0) {
  draw_vert r={ .x=-p_0->y, .y=p_0->x };
  return r;
}

inline bool draw_vertEq(const draw_vert *p_0, const draw_vert *p_1) {
  return p_0->x==p_1->x && p_0->y==p_1->y;
}

inline bool draw_vertSim(const draw_vert *p_0, const draw_vert *p_1, const float32 fuzz) {
  return rtu_similar(p_0->x, p_1->x, fuzz ) && rtu_similar(p_0->y, p_1->y, fuzz);
}

inline draw_vert draw_divideByUInt(const draw_vert *p_v0, const uint32 scalar, const draw_globals *p_globals) {
  LOG_ASSERT(scalar!=0, "divide by zero");
  if(scalar>=DRAW_DIV_LIMIT) {
    LOG_INFO("draw_divideByUInt invoked");
    draw_vert res={.x=p_v0->x/scalar, .y=p_v0->y/scalar }; //TODO division
    return res;
  } else {
    draw_vert res={ .x=rtu_fastDiv(p_v0->x, scalar, p_globals->p_rtu), .y=rtu_fastDiv(p_v0->y, scalar, p_globals->p_rtu) };
    return res;
  }
}

inline draw_vert draw_divideOneByOne(const draw_vert *p_v0, const draw_vert *p_divisor) {
  LOG_INFO("draw_divideOneByOne invoked");
  LOG_ASSERT(p_divisor->x!=0 && p_divisor->y!=0, "divide by zero %f, %f", p_divisor->x, p_divisor->y);
  draw_vert res={ .x=p_v0->x/p_divisor->x, .y=p_v0->y/p_divisor->y }; //TODO division
  return res;
}

inline draw_vert draw_weightedMean(const draw_vert *p_one, const draw_vert *p_two, float32 t) {
  LOG_ASSERT(t>=0 && t<=1, "out of range t: %f", t);
  draw_vert one_weighted=draw_by(p_one, t);
  draw_vert two_weighted=draw_by(p_two, 1-t);
  draw_vert sum=draw_add(&one_weighted, &two_weighted);
  return sum;
}

inline draw_vert draw_mean(const draw_vert *p_one, const draw_vert *p_two) {
  draw_vert sum=draw_add(p_one, p_two);
  return draw_by(&sum, 0.5);
}

inline draw_vert* draw_quadNextFD(draw_quad *p_bez) {
  p_bez->fds[0]=p_bez->fd_t;
  bool done=p_bez->step_sum>=1.0;
  p_bez->step_sum+=p_bez->step;
  p_bez->fd_t=draw_add(&p_bez->fd_t, &p_bez->fd_delta_by_step);
  //LOG_INFO("fd: %f, %f", p_bez->fds[done].x, p_bez->fds[done].y);
  return &p_bez->fds[done];
}

inline draw_vert draw_fd2Norm(const draw_vert *p_fd, float32 width) {
  draw_vert tan=draw_fd2Tan(p_fd, width);
  return draw_norm(&tan);
}

inline void draw_onOffset(uint32 pOffset, uint32 col, const draw_globals *p_globals) {
  draw_atOffset(pOffset, MAX(col, draw_get(pOffset, p_globals)), p_globals);
}

inline void draw_dot(uint32 x, uint32 y, uint32 col, const draw_globals *p_globals) {
  uint32 offset=(y*draw_canvasRenderedWidth(&p_globals->canvas))+x;
  LOG_ASSERT(offset<draw_canvasRenderedWidth(&p_globals->canvas)*draw_canvasHeight(&p_globals->canvas), "offset %u too large (limit: %u)", offset, draw_canvasRenderedWidth(&p_globals->canvas)*draw_canvasHeight(&p_globals->canvas));
  /*
  if(x==414 && y==226 && col==0x9c07abff) {
    LOG_ERROR("col: 0x%8x", col);
  }
  */
  draw_onOffset(offset, col, p_globals);
}

inline void draw_vertDot(const draw_vert *p_0, uint32 col, const draw_globals *p_globals) {
  uint32 x=p_0->x, y=p_0->y;
  draw_dot(x, y, col, p_globals);
}

inline float32 draw_plotBezInitC_0(float32 fdd, float32 step) {
  return fdd*step*step;
}

inline float32 draw_plotBezInitC_1(float32 p_0, float32 p_1, float32 p_2, float32 step) {
  return (2*p_0-4*p_1+2*p_2)*step*step;
}

inline float32 draw_plotBezInitFDD(float32 p_0, float32 p_1, float32 p_2) {
  return ((2*p_0 - 4*p_1 + 2*p_2))/2; 
}

inline float32 draw_plotBezInitFD_0(float32 p_0, float32 p_1) {
  return (-2*p_0 + 2*p_1);
}

inline float32 draw_plotBezInitFD_1(float32 p_1, float32 p_2) {
  //return -2*p_1+2*p_2;
  return draw_plotBezInitFD_0(p_1, p_2);
}

inline float32 draw_plotBezInitFDTimesT(float32 fd_0, float32 step) {
  return fd_0*step;
}

inline float32 draw_fd(float32 c_2, float32 c_3, float32 t) {
  return c_2+c_3*t;
}

inline float32 draw_plotBezUpdateFXT(float32 f_x_t, float32 fd_times_t, float32 c_0) {
  //LOG_INFO("update fxt. orig: %f \ttimes: %f \tc_0: %f", f_x_t, fd_times_t, c_0);
  return f_x_t+fd_times_t+c_0;
}

inline float32 draw_plotBezUpdateFDTimesT(float32 fd_times_t, float32 c_1) {
  return fd_times_t+c_1;
}

inline uint32 draw_largerPowerOf2(uint32 start, uint32 target) {
  uint32 val=MAX(start,1);
  while(val<target) {
    val<<=1;
  }
  return val;
}

inline uint32 draw_scanLogPos2ConstIdx(const draw_scanBrushLog *p_b, const float32 pos) {
  uint32 which_half_idx=pos>p_b->w.half_width;
  bool blurred=pos<p_b->grad_consts.threshes[0] || pos>=p_b->grad_consts.threshes[1];
  return blurred?(which_half_idx<<1):1;
 }

inline uint32 draw_pos2Idx(const draw_scanBrushLog *p_b, const float32 pos) {
  return draw_scanLogPos2ConstIdx(p_b, pos);
}

inline uint32 draw_gradIdx2ConstIdx(const draw_grad *p_grad, const uint32 grad_idx) {
  return grad_idx;
}

inline float32 draw_gradRowSideM(const draw_grad *p_grad, const draw_gradTranslated *p_grad_trans, const float32 y) {
  LOG_ASSERT(p_grad->idx==p_grad_trans->idx, "iter mismatch: %u, %u", p_grad->idx, p_grad_trans->idx);
  float32 c5_cy=p_grad->c5*y;
  float32 c5_cy_plus_c6_ax_less_c5_ay=c5_cy+p_grad_trans->c6_ax_less_c5_ay;
  return c5_cy_plus_c6_ax_less_c5_ay;
}

inline float32 draw_gradNewSideM(const draw_grad *p_grad, const draw_gradTranslated *p_grad_trans, const float32 x, const float32 y) {
  return draw_gradRowSideM(p_grad, p_grad_trans, y)-(p_grad->c6*x);
}

inline float32 draw_gradRow2NewSideM(const draw_grad *p_grad, const float32 row_side_m, const float32 x) {
  return row_side_m-(p_grad->c6*x);
}

inline sint32 draw_gradSideIncSgn4FD(const draw_grad *p_grad, const draw_vert *p_fd) {
  float32 inc=p_grad->c5*p_fd->y-p_grad->c6*p_fd->x;
  return SGN(inc);
}

inline float32 draw_mCs2X(float32 ma, float32 mb, float32 ca, float32 cb) {
  LOG_ASSERT(mb-ma!=0, "slope diff of 0 %f, %f", mb, ma);
  return (ca-cb)/(mb-ma);
}

inline float32 draw_ptSlope2C(const draw_vert *p_v, float32 m) {
  return p_v->y-(m*p_v->x);
}

inline float32 draw_mXC2Y(float32 m, float32 x, float32 c) {
  return m*x+c;
}

inline sint32 draw_capBoundaryCmp(const draw_capBoundary *p_fst, const draw_capBoundary *p_snd) {
  if(p_fst->x==p_snd->x) {
    return CMP(p_snd->start_cap, p_fst->start_cap);
  } else {
    return CMP(p_fst->x, p_snd->x);
  }
}

inline float32 draw_savedWidth2RenderedWidth(float32 saved_width, float32 blur_width) {
  return saved_width+(blur_width*0.5); //only the outer blur is added to width
}

inline float32 draw_breadth2HalfWidth(const float32 breadth, const float32 blur_width) {
  return draw_savedWidth2RenderedWidth(breadth, blur_width)*0.5;
}

inline uint32 draw_blotBreadth2BBoxLen(const float32 breadth, const float32 blur_width) {
  return draw_blotHalfWidth2BBoxLen(draw_breadth2HalfWidth(breadth, blur_width));
}

inline uint32 draw_blotBBoxLen(const draw_blot *p_blot) {
  return draw_blotBreadth2BBoxLen(p_blot->breadth, p_blot->blur_width);
}

inline float32 draw_blotHalfWidth(const draw_blot *p_blot) {
  return draw_breadth2HalfWidth(p_blot->breadth, p_blot->blur_width);
}

inline draw_vert draw_blotBBoxDims(const draw_blot *p_blot) {
  float32 bbox_len=draw_blotBreadth2BBoxLen(p_blot->breadth, p_blot->blur_width);
  draw_vert dims={ .x=bbox_len, .y=bbox_len };
  return dims;
}

inline draw_vert draw_blotBBoxLen2BBoxDims(const uint32 bbox_len) {
  draw_vert dims={ .x=bbox_len, .y=bbox_len };
  return dims;
}

inline sint32 draw_horizVert2Quadrant(const sint32 horiz, const sint32 vert) {
  LOG_ASSERT(horiz==-1 || horiz==+1, "invalid horiz %i", horiz);
  LOG_ASSERT(vert==-1 || vert==+1, "invalid vert %i", vert);
  /*
    +-----+-----+------+
    |horiz|vert |quadra|
    +-----+-----+------+
    |-1   |+1   |0     | 1,1,0
    +-----+-----+------+
    |-1   |-1   |1     | 1,0,1
    +-----+-----+------+
    |+1   |-1   |2     | 2,0,2
    +-----+-----+------+
    |+1   |+1   |3     | 2,1,3
    +-----+-----+------+
   */
  uint32 horiz_base=((horiz+1)>>1)+1;
  LOG_ASSERT(horiz_base==1 || horiz_base==2, "invalid horiz start: %u", horiz_base);
  uint32 vert_flag=((vert+1)>>1);
  LOG_ASSERT(vert_flag==0 || vert_flag==1, "invalid vert flag: %u", vert_flag);
  uint32 quadrant=horiz_base ^ vert_flag;
  
  LOG_ASSERT(quadrant>=0 && quadrant<=3, "invalid quadrant: %i", quadrant);
  return quadrant;
}

inline uint32 draw_quadrant2CoordToIterIdx(const uint32 q) {
  LOG_ASSERT(q<=3, "out of range quadrant %u", q);
  return q==1 || q==3;
}

inline draw_grads *draw_blotQuadrant2Grads(draw_blot *p_blot, const sint32 right, const sint32 down) {
  LOG_ASSERT(right==-1 ||right==1,"invalid right: %i", right);
  LOG_ASSERT(down==-1 ||down==1,"invalid down: %i", down);
  uint32 q=draw_horizVert2Quadrant(right, down);
  return &p_blot->slope_sign_2_iter_2_grads[draw_quadrant2CoordToIterIdx(q)];
}

inline float32 draw_blotBreadth(const draw_blot *p_blot) {
  return p_blot->breadth;
}

inline draw_coordToIter *draw_blotCoordToIters(draw_blot *p_blot) {
  return p_blot->coord_2_iters;
}

inline draw_coordToIter *draw_blotQuadrant2CoordToIter(draw_blot *p_blot, const sint32 right, const sint32 down) {
  LOG_ASSERT(right==-1 ||right==1,"invalid right: %i", right);
  LOG_ASSERT(down==-1 ||down==1,"invalid down: %i", down);
  sint32 q=draw_horizVert2Quadrant(right, down);
  LOG_ASSERT(q>=0 && q<=3, "out of range quadrant. %i, %i", right, down);
  return &p_blot->coord_2_iters[draw_quadrant2CoordToIterIdx(q)];
}

inline float32 draw_gradLongConst(const draw_grad *p_grad, const draw_vert *p_a) {
  float32 c6_ax=p_grad->c6*p_a->x;
  float32 c5_ay=p_grad->c5*p_a->y;
  return c6_ax-c5_ay;
}

inline void draw_gradTranslatedInit(draw_gradTranslated *p_grad_trans, const draw_grad *p_grad, const draw_vert *p_mid) {
  p_grad_trans->mid=*p_mid;
  draw_vert min_bound=draw_diff(&p_grad_trans->mid, &p_grad->half_delta);
  p_grad_trans->c6_ax_less_c5_ay=draw_gradLongConst(p_grad, &min_bound);
  p_grad_trans->offset=draw_diff(&p_grad_trans->mid, &p_grad->mid);
  p_grad_trans->initialised=true;
}

inline bool draw_gradTranslatedFill(draw_gradTranslated *p_grad_trans, const draw_grad *p_grad, const draw_vert *p_mid) {
  LOG_ASSERT(p_grad_trans->idx==p_grad->idx, "draw_grad (%p) mismatch: %u, %u size: %u, idx addr: %p",p_grad_trans, p_grad_trans->idx, p_grad->idx, sizeof(draw_gradTranslated), &p_grad_trans->idx);
  LOG_ASSERT(p_grad->initialised, "draw_grad not initialised: %u", p_grad->idx);
  if(!p_grad_trans->initialised) {
    draw_gradTranslatedInit(p_grad_trans, p_grad, p_mid);
    return false;
  }
  return true;
}

inline sint32 draw_quadrant2Horiz(const uint32 quadrant) {
  LOG_ASSERT(quadrant<=3, "out of range quadrant");
  //returns +1 or -1
  return (((quadrant==2) | (quadrant==3)) << 1)-1;
}

inline sint32 draw_quadrant2Vert(const uint32 quadrant) {
  LOG_ASSERT(quadrant<=3, "out of range quadrant");
  //returns +1 or -1
  return (((quadrant==0) | (quadrant==3)) << 1) -1;
}

inline void draw_dotScanRowLeftBnd(float32 x, float32 y, draw_scanFillLog *p_f, const draw_gradsIf *p_grad_agg, const uint32 iter, draw_globals *p_globals) {
  if(y<0 || y>=draw_canvasHeight(&p_globals->canvas)) {
    return;
  }
  LOG_ASSERT(p_f->xs_pairs.p_y_2_starts, "uninitialised starts array");
  uint32 y_offset=y-p_f->min_y;
  LOG_ASSERT(y_offset<p_f->size, "out of bounds array assignment / read");
  sint32 x_int=(sint32)x;

  p_f->xs_pairs.p_y_2_starts[y_offset]=MIN(p_f->xs_pairs.p_y_2_starts[y_offset], x_int);
}

inline void draw_dotScanRowRightBnd(float32 x, float32 y, draw_scanFillLog *p_f, const draw_gradsIf *p_grad_agg, const uint32 iter, draw_globals *p_globals) {
  if(y<0 || y>=draw_canvasHeight(&p_globals->canvas)) {
    return;
  }
  LOG_ASSERT(p_f->xs_pairs.p_y_2_ends, "uninitialised ends array");
  uint32 y_offset=y-p_f->min_y;
  LOG_ASSERT(y_offset<p_f->size, "out of bounds array assignment / read");
  sint32 x_int=(sint32)x; //+1 so rounding down does cut of the end of a row
  x_int+=x_int!=x;
  p_f->xs_pairs.p_y_2_ends[y_offset]=MAX(p_f->xs_pairs.p_y_2_ends[y_offset], x_int);
}

float32 draw_vertAngBetween(const draw_vert *p_0, const draw_vert *p_1, const draw_vert *p_2);
uint32 draw_calcMaxIter(const float32 width, const float32 ang);

inline uint32 draw_coordToIterVertY2Idx(const draw_coordToIter *p_c2I, const float32 y) {
  return ((p_c2I->min_on_y_y_coord-y)*2+p_c2I->max_on_y_x_coord);
}

inline uint32 draw_coordToIterVert2Idx(
				       const draw_coordToIter *p_c2I, 
				       const draw_vert *p_0) {
  float32 pos_x=ABSF(p_0->x);
  LOG_ASSERT(p_0->y>=0, "negative p_0->y: %f", p_0->y);
  float32 pos_y=p_0->y;
  if(pos_x>pos_y) {
    //only pos_y matters
    if(pos_y<0) {
      pos_y=0;
    } else if(pos_y>p_c2I->min_on_y_y_coord) {
      pos_y=p_c2I->min_on_y_y_coord;
    }
    return ROUND32(draw_coordToIterVertY2Idx(p_c2I, pos_y));
  } else {
    //only pos_x matters
    if(pos_x>=p_c2I->abs_max_y_x) {
      return ROUND32(p_c2I->max_on_y_x_coord);
    }
    return ROUND32(pos_x*2);
  }
}

inline draw_grads *draw_coordToIterGrads(draw_coordToIter *p_c2I) {
  return p_c2I->p_grad_agg;
}

inline uint32 draw_coordToIterMaxIter(draw_coordToIter *p_c2I) {
  return p_c2I->p_grad_agg->max_iter;
}

inline draw_grad *draw_coordToIterGradPtr(draw_coordToIter *p_c2I) {
  return p_c2I->p_grad_agg->p_iter_2_grad;
}

inline uint32 draw_gradReferenceQuadrant(const draw_gradReference *p_grad_ref) {
  return p_grad_ref->start_quadrant;
}

uint32 draw_coordToIterGetIter(const draw_coordToIter *p_c2I, const draw_vert *p_0);
inline uint32 draw_gradReferenceIter(const draw_gradReference *p_grad_ref, const draw_vert *p_0) {
  draw_vert delta=draw_diff(p_0, &p_grad_ref->center);
  /*
  if(G_trace) {
    LOG_ERROR("delta: %f,%f",delta.x, delta.y);
  }
  */
  return draw_coordToIterGetIter(&p_grad_ref->p_coord_2_iters[draw_quadrant2CoordToIterIdx(draw_gradReferenceQuadrant(p_grad_ref))], &delta);
}

inline void draw_vertNullableSetPt(draw_vertNullable *p_inst, const draw_vert *p_0) {
  if(p_0) {
    p_inst->pt=*p_0;
    p_inst->p_pt=&p_inst->pt;
  } else {
    p_inst->p_pt=NULL;
  }
}

inline void draw_gradReferenceSetLastPt(draw_gradReference *p_grad_ref, const draw_vert *p_0) {
  draw_vertNullableSetPt(&p_grad_ref->last_pt, p_0);
}

inline draw_gradTranslated *draw_gradReferenceTrans(draw_gradReference *p_grad_ref, const uint32 q, const uint32 iter) {
  LOG_ASSERT(0<=q && q<=3, "out of range quadrant: %u", q);
  LOG_ASSERT(iter<=p_grad_ref->max_iter, "out of range iter: %u, max_iter: %u", iter, p_grad_ref->max_iter);

  sint32 offset_idx=q*(p_grad_ref->max_iter+1);
  return &p_grad_ref->p_iter_2_grad_trans[offset_idx+iter];
}

inline uint32 draw_blotNumCoord2Iters(const draw_blot *p_blot) {
  return DRAW_BLOT_NUM_COORD_TO_ITERS;
}

inline float32 draw_gradATan(const draw_grad *p_grad, const draw_globals *p_globals) {
  const draw_vert *p_fd_signed=&p_grad->fd_signed;
  return p_fd_signed->x==0?0.0:rtu_fastATan(p_fd_signed->y/p_fd_signed->x, p_globals->p_rtu);
}

inline draw_gradTranslated *draw_gradsTrans(draw_grads *p_grad_agg, const uint32 q, const uint32 iter) {
  LOG_ASSERT(iter<=p_grad_agg->max_iter, "out of range iter: %u, max_iter: %u", iter, p_grad_agg->max_iter);
  LOG_ASSERT(q==UINT_MAX, "draw_grads can't handle multiple quadrants: %u", q);
  return &p_grad_agg->trans.p_iter_2_grad_trans[iter];
}

inline uint32 draw_proxOffset(const draw_prox *p_prox, const float32 cen, const sint32 coord) {
  sint32 offset=(((cen<(sint32)cen)+coord-((sint32)cen))+(sint32)p_prox->rel_origin);
  LOG_ASSERT(offset>=0, "offset is an array idx and must not be negative: span %u, offset %i, cen %f, coord %i, origin %u", p_prox->span, offset, cen, coord, p_prox->rel_origin);
  LOG_ASSERT(offset<p_prox->span, "out of range offset: span %u, offset %i, cen %f, coord %i, origin %u", p_prox->span, offset, cen, coord, p_prox->rel_origin);
  return (uint32)offset;
}

inline uint32 draw_proxOffsetIdx(const draw_prox *p_prox, const float32 x_cen, const sint32 x, const uint32 y_offset_idx) {
  uint32 x_offset_idx=draw_proxOffset(p_prox, x_cen, x);
  return y_offset_idx+x_offset_idx;
}

inline uint32 draw_insertProximityVal(const uint32 old_nearest, const uint32 is_nearest, const uint32 val) {
  return ((old_nearest << DRAW_PROXIMITY_SHIFT(is_nearest)) | (val<<DRAW_PROXIMITY_SHIFT(!is_nearest)));
}

inline uint32 draw_proxRowOffset(const draw_prox *p_prox, const draw_vert *p_center, const sint32 y) {
  return draw_proxOffset(p_prox, p_center->y, y)*p_prox->span;
}

inline uint32 draw_scanBrushLogRowOffset(const draw_scanBrushLog *p_b, const draw_vert *p_center, const sint32 y) {
  return draw_proxRowOffset(&p_b->proximity, p_center, y);
}

inline float32 draw_scanBrushLogRelativise(const draw_scanBrushLog *p_b, float32 brush_center, uint32 coord) {
  float32 relative=((float32)coord)-(brush_center-p_b->w.half_width);
  return relative;
}

inline uint32 draw_proxHalfSpan(const draw_prox *p_prox, const uint32 width, const bool long_half) {
  return (width>>1)+1+long_half;
}

inline uint32 draw_dirtyNumRects(draw_globals *p_globals) {
  return p_globals->canvas.dirty.lt.y!=INT_MAX;
}

inline draw_rectInt* draw_canvasDirty(draw_canvas *p_canvas) {
  return &p_canvas->dirty;
}

inline draw_rectInt* draw_canvasExtantDirty(draw_canvas *p_canvas) {
  return &p_canvas->extantDirty;
}

inline uint32 draw_dirtyTop(draw_globals *p_globals, const uint32 idx) {
  return draw_canvasDirty(&p_globals->canvas)->lt.y;
}

inline uint32 draw_dirtyBottom(draw_globals *p_globals, const uint32 idx) {
  return draw_canvasDirty(&p_globals->canvas)->rb.y;
}

inline uint32 draw_dirtyLeft(draw_globals *p_globals, const uint32 idx) {
  return draw_canvasDirty(&p_globals->canvas)->lt.x;
}

inline uint32 draw_dirtyRight(draw_globals *p_globals, const uint32 idx) {
  return draw_canvasDirty(&p_globals->canvas)->rb.x;
}

inline uint32 draw_extantDirtyNumRects(draw_globals *p_globals) {
  return draw_canvasExtantDirty(&p_globals->canvas)->lt.y!=INT_MAX;
}

inline uint32 draw_extantDirtyTop(draw_globals *p_globals, const uint32 idx) {
  return draw_canvasExtantDirty(&p_globals->canvas)->lt.y;
}

inline uint32 draw_extantDirtyBottom(draw_globals *p_globals, const uint32 idx) {
  return draw_canvasExtantDirty(&p_globals->canvas)->rb.y;
}

inline uint32 draw_extantDirtyLeft(draw_globals *p_globals, const uint32 idx) {
  return draw_canvasExtantDirty(&p_globals->canvas)->lt.x;
}

inline uint32 draw_extantDirtyRight(draw_globals *p_globals, const uint32 idx) {
  return draw_canvasExtantDirty(&p_globals->canvas)->rb.x;
}

float32 draw_brushScaleFactor(const draw_brush *p_brush);
inline void draw_strokeContinue(draw_stroke *p_stroke, const draw_vert *p_start, draw_globals *p_globals) {
  //this simply a move without wiping the canvas first
  draw_vert scaled_start=draw_by(p_start, draw_brushScaleFactor(&p_stroke->brush));
  p_stroke->last=scaled_start;
  p_stroke->state=DRAW_STROKE_MOVED;
}

void draw_scanBrushLogInit(draw_scanBrushLog *p_b, const float32 breadth, const float32 blur_width, const uint32 col, draw_canvas *p_canvas, const bool recalc_scaling);
void draw_blotInit(draw_blot *p_blot, draw_scanBrushLog *p_b, draw_globals *p_globals);
inline void draw_brushInitInternal(draw_brush *p_brush, const uint32 col, const float32 breadth, const float32 blur_width, draw_globals *p_globals, const bool recalc_scaling) {
  draw_scanBrushLogInit(&p_brush->b, breadth, blur_width, col, &p_globals->canvas, recalc_scaling);
  draw_blotInit(&p_brush->blot, &p_brush->b, p_globals);
}

float32 draw_scanFillLogCheckAngle(draw_scanFillLog *p_f, const draw_grad *p_last_grad, const draw_grad *p_grad, const draw_globals *p_globals);
inline float32 draw_scanLogCheckAngle(draw_scanLog *p_log, const draw_grad *p_last_grad, const draw_grad *p_grad, const draw_globals *p_globals) {
  return draw_scanFillLogCheckAngle(&p_log->f, p_last_grad, p_grad, p_globals);
}

inline uint32 draw_renderCoreHeight(const draw_renderCore *p_render_core, const draw_scanFillLog *p_f) {
  //sint32 height=(2+p_render_core->real_max_y-p_f->min_y);  //2+ for rounding error fix mentioned above plus accounting for our call to draw_bottomRow below (to handle partial pixel overflow in the y dim)
  //return MAX(0, height);
  return p_render_core->height;
}

bool draw_tail(draw_vert *p_0, draw_vert *p_1, draw_onPtCb *p_on_pt_cb, draw_scanFillLog *p_arg, const draw_gradsIf *p_grad_agg, const uint32 iter, draw_globals *p_globals);
sint32 draw_plotBez(
		    const float32 step, 
		    draw_vert *p_pts, 
		    draw_onPtCb *p_on_pt_cb,
		    draw_onIterCb *p_on_iter_cb,
		    draw_scanLog *p_log, 
		    draw_gradsIf *p_grad_agg, 
		    draw_globals *p_globals
		    );
void draw_thickQuad(const draw_vert *p_0, const draw_vert *p_1, const draw_vert *p_2, draw_scanBrushLog *p_b, draw_globals *p_globals);
void draw_thickBlot(const draw_vert *p_box_dims, const draw_vert *p_0, const uint32 col, const uint32 breadth, const bool right, const bool down);
void draw_dotSimple(const float32 x, const float32 y, const bool x_dom, const uint32 col);

void draw_quadInit(draw_quad *p_bez, const draw_vert **pp_verts);
void draw_quadStartFDGen(draw_quad *p_bez, float32 step);
float32 draw_test_blurWidth(float32 saved_width);
sint8 *draw_test(void);
void draw_scanLogStartEdge(draw_scanLog *p_log, const uint32 iter, const draw_vert *p_0, const draw_globals *p_globals);
void draw_scanLogStartBezEdge(draw_scanLog *p_log, const uint32 iter, const draw_vert *p_0, const draw_vert *p_1, const draw_vert *p_2, const draw_globals *p_globals);
void draw_scanLogStartLineEdge(draw_scanLog *p_log, const uint32 iter, const draw_vert *p_0, const draw_vert *p_1, const draw_globals *p_globals);
void draw_scanLogEndEdge(draw_scanLog *p_log);

void draw_blotDestroy(draw_blot *p_blot);
void draw_blotRender(draw_blot *p_blot, const draw_vert *p_0, const uint32 col, const bool right, const bool down, draw_globals *p_globals);
void draw_blotRender360(draw_blot *p_blot, const draw_vert *p_0, const uint32 col, draw_globals *p_globals);
void draw_blotHere(const draw_vert *p_0, const float32 breadth, const float32 blur, const uint32 col, draw_globals *p_globals);

void draw_brushInit(draw_brush *p_brush, const uint32 col, const float32 breadth, const float32 blur_width, draw_globals *p_globals);
bool draw_brushRenderable(const draw_brush *p_brush);
float32 draw_brushHalfWidth(const draw_brush *p_brush);
void draw_brushDestroy(draw_brush *p_brush);
uint32 draw_brushMagFactor(const draw_brush *p_brush);

void draw_strokeInit(draw_stroke *p_stroke, const draw_brush *p_brush, draw_globals *p_globals);
bool draw_strokeRenderable(const draw_stroke *p_stroke);
void draw_strokeMoveTo(draw_stroke *p_stroke, const draw_vert *p_start, draw_globals *p_globals);
void draw_strokeQuadTo(draw_stroke *p_stroke, const draw_vert *p_ctrl, const draw_vert *p_end, draw_globals *p_globals);
void draw_strokeRender(draw_stroke *p_stroke, draw_globals *p_globals);
void draw_strokeReset(draw_stroke *p_stroke, draw_globals *p_globals);
void draw_quadPtsInit(draw_quadPts *p_pts, const draw_vert *p_0, const draw_vert *p_1, const draw_vert *p_2);
void draw_quadPtsSplit(const draw_quadPts *p_pts, float32 t, draw_quadPts *p_fst, draw_quadPts *p_snd);
bool draw_fillIter(draw_grad *p_grad, draw_gradTranslated *p_grad_trans, const draw_strokeWidth *p_w, const draw_globals *p_globals);
void draw_gradInitFill(draw_grad *p_grad, const draw_strokeWidth *p_w, const draw_globals *p_globals);
draw_vert draw_vertRotate(const draw_vert *p_vert, const float32 ang, const draw_vert *p_origin);
uint32 draw_renderCoreMaxIter(const draw_renderCore *p_renderCore);
float32 draw_vertAng(const draw_vert *p_0, const draw_vert *p_1);
void draw_gradsUpdateBnds(draw_grads *p_grad_agg, const uint32 iter, const draw_gradTranslated *p_grad_trans);
void draw_rectIntInit(draw_rectInt *p_bnds);
//bool draw_setupCanvas(uint32 w, uint32 h, uint* p_canvas);
void draw_globalsLoadR(draw_globals *p_globals);
void draw_globalsSave(draw_globals *p_globals);
draw_globals* draw_globalsInit(void);
void draw_globalsDestroy(draw_globals *p_globals);
draw_globals draw_globalsInitialFields(void);
bool draw_renderable(const float32 breadth, const uint32 col);
void draw_canvasMarkDirtyRadius(draw_canvas *p_canvas, const draw_vert *p_0, const float32 breadth, const float32 blur_width);
void draw_gradMarkDirty(const draw_grad *p_grad, const draw_gradTranslated *p_grad_trans, draw_canvas *p_canvas);
void draw_rectIntReify(draw_rectInt *p_orig);
bool draw_rectIntFilled(const draw_rectInt *p_rect);
void draw_canvasIgnoreDirt(draw_canvas *p_canvas);
void draw_canvasInit(draw_canvas *p_canvas, const uint32 w, const uint32 h, const float32 devicePixelRatio);
void draw_canvasWipe(draw_canvas *p_canvas, const draw_globals *p_globals);
void draw_canvasClearAll(draw_canvas *p_canvas);
void draw_coordToIterInit(draw_coordToIter *p_c2I, draw_grads *p_grad_agg, const draw_vert *p_origin, const sint32 q, const draw_strokeWidth *p_w, const uint32 start_iter, const uint32 end_iter);
uint32 draw_coordToIterGetIter(const draw_coordToIter *p_c2I, const draw_vert *p_0);
void draw_coordToIterDestroy(draw_coordToIter *p_c2I);
void draw_rowNewSegmentBelow(const draw_scanBrushLog *p_b, const sint32 start_x, const sint32 end_x, const uint32 y, draw_globals *p_globals, draw_gradReference *p_grad_ref);
void draw_rowNewSmallSegmentRangeOnX(const draw_scanBrushLog *p_b, const sint32 first_x, const sint32 last_x, const uint32 y, draw_globals *p_globals, draw_gradReference *p_ref);
void draw_rowNewSmallSegmentRangeOnY(const draw_scanBrushLog *p_b, const sint32 first_x, const sint32 last_x, const uint32 y, draw_globals *p_globals, draw_gradReference *p_ref);
void draw_rowNewSegmentRangeOnX(const draw_scanBrushLog *p_b, const sint32 first_x, const sint32 last_x, const uint32 y, draw_globals *p_globals, draw_gradsIf *p_grads_if);
void draw_rowNewSegmentRangeOnY(const draw_scanBrushLog *p_b, const sint32 first_x, const sint32 last_x, const uint32 y, draw_globals *p_globals, draw_gradsIf *p_grads_if);
void draw_gradReferenceSetIterRange(draw_gradReference *p_grad_ref, const uint32 q, const uint32 iter);
draw_iterRange draw_gradReferenceIterRange(draw_gradReference *p_grad_ref);
void draw_gradsSetIterRange(draw_grads *p_grad_agg, const uint32 q, const uint32 iter);
void draw_gradTranslatedsDestroy(draw_gradTranslateds *p_grad_trans);
void draw_gradTranslatedsInit(draw_gradTranslateds *p_grad_trans, const uint32 max_iter, const uint32 num_iters);
void draw_gradTranslatedsReset(draw_gradTranslateds *p_grad_trans);
uint32 draw_gradsMaxIter(draw_grads *p_grad_agg);
draw_iterRange draw_gradsIterRange(draw_grads *p_grad_agg);
void draw_bezInit(draw_bez *p_bez, const float32 step, const draw_vert *p_0, const draw_vert *p_1, const draw_vert *p_2, const draw_globals *p_globals);
void draw_gradDestroy(draw_grad *p_grad);
uint32 draw_scanBrushLogPt2Iter(const draw_scanBrushLog *p_b, draw_gradReference *p_ref, const draw_vert *p_0);
sint32 draw_gradReferenceVert(const draw_gradReference *p_grad_ref);
draw_grad *draw_gradsGradPtr(draw_grads *p_grad_agg, const uint32 q);
void draw_proxDestroy(draw_prox *p_prox);
void draw_vertNullableInit(draw_vertNullable *p_vertInstPtr, const draw_vert *p_0);
void draw_blotContinue(const draw_vert *p_0, const draw_vert *p_fd, const float32 breadth, const float32 blur_width, const uint32 col, draw_globals *p_globals);

#endif

