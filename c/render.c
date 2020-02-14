#include <jni.h>
#include <stdbool.h>

#include "types.h"
#include "rendering/draw.h"
/*
 */

uint32 tests_run=0;

jlong Java_com_hourglassapps_tiles_render_CRenderEngine_renderInit(JNIEnv* env, jobject x, jint w, jint h) {
	draw_globals *p_globals=draw_globalsInit();
	if(p_globals!=NULL) {
		LOG_INFO("renderInit. globals: 0x%x", p_globals);
		//draw_globalsLoadR(p_globals);
		p_globals->all_xs_pairs.p_y_2_starts = rtu_memAlloc(sizeof(sint32) * h);
		if (p_globals->all_xs_pairs.p_y_2_starts == NULL) {
			goto error;
		}

		p_globals->all_xs_pairs.p_y_2_ends = rtu_memAlloc(sizeof(sint32) * h);
		if (p_globals->all_xs_pairs.p_y_2_ends == NULL) {
			rtu_memFree(p_globals->all_xs_pairs.p_y_2_starts);
			goto error;
		}

		rtu_initFastATan(DRAW_ATAN_DIVISORS, p_globals->p_rtu);
		rtu_initFastDiv(DRAW_DIV_LIMIT, p_globals->p_rtu);
		draw_canvasInit(&p_globals->canvas, w, h, -1);
		return (jlong)p_globals;
	}
	return NULL;

	error:
		rtu_globalsDestroy(p_globals);
		return NULL;

}

void Java_com_hourglassapps_tiles_render_CRenderEngine_renderDestroy(JNIEnv* env, jobject x, jlong globals_ptr) {
        draw_globals *p_globals=(draw_globals *)globals_ptr;
       rtu_destroyDiv(p_globals->p_rtu);
       rtu_destroyATan(p_globals->p_rtu);
       draw_globalsDestroy(p_globals);
}


jint Java_com_hourglassapps_tiles_render_CRenderEngine_renderDirtyNumRects(JNIEnv* env, jobject x, jlong globals_ptr) {
	draw_globals *p_globals=(draw_globals *)globals_ptr;
	return draw_dirtyNumRects(p_globals);
}

jint Java_com_hourglassapps_tiles_render_CRenderEngine_renderDirtyTop(JNIEnv* env, jobject x, jlong globals_ptr, jint idx) {
	draw_globals *p_globals=(draw_globals *)globals_ptr;
	return draw_dirtyTop(p_globals, idx);
}

jint Java_com_hourglassapps_tiles_render_CRenderEngine_renderDirtyBottom(JNIEnv* env, jobject x, jlong globals_ptr, jint idx) {
	draw_globals *p_globals=(draw_globals *)globals_ptr;
	return draw_dirtyBottom(p_globals, idx);
}

jint Java_com_hourglassapps_tiles_render_CRenderEngine_renderDirtyLeft(JNIEnv* env, jobject x, jlong globals_ptr, jint idx) {
	draw_globals *p_globals=(draw_globals *)globals_ptr;
	return draw_dirtyLeft(p_globals, idx);
}

jint Java_com_hourglassapps_tiles_render_CRenderEngine_renderDirtyRight(JNIEnv* env, jobject x, jlong globals_ptr, jint idx) {
	draw_globals *p_globals=(draw_globals *)globals_ptr;
	return draw_dirtyRight(p_globals, idx);
}

void Java_com_hourglassapps_tiles_render_CRenderEngine_renderIgnoreDirt(JNIEnv* env, jobject x, jlong globals_ptr) {
	draw_globals *p_globals=(draw_globals *)globals_ptr;
	draw_canvasIgnoreDirt(&p_globals->canvas);
}

jint* renderGetCanvas(JNIEnv* pp_env, jintArray pixels, jint w, jint h) {
	jboolean is_copy;
	jint* p_pixels=(*pp_env)->GetPrimitiveArrayCritical(pp_env, pixels, &is_copy);
	if((*pp_env)->ExceptionCheck(pp_env)) {
		LOG_ASSERT(false, "failed to get array");
		return NULL;
	}
	return p_pixels;
}

bool renderReleaseCanvas(JNIEnv* pp_env, jintArray pixels, jint *p_pixels) {
	(*pp_env)->ReleasePrimitiveArrayCritical(pp_env, pixels, p_pixels, 0);
	if((*pp_env)->ExceptionCheck(pp_env)) {
		LOG_ASSERT(false, "failed to release array");
		return false;
	}
	return true;
}

jlong Java_com_hourglassapps_tiles_render_CRenderEngine_renderStrokeInit(JNIEnv* env, jobject x, jlong brush_ptr,
		jintArray pixels, jint w, jint h, jlong globals_ptr) {
	draw_globals *p_globals=(draw_globals *)globals_ptr;
	//draw_globalsLoadR(p_globals);

	jint* p_pixels=renderGetCanvas(env, pixels, w, h);
	if(!p_pixels) {
		return 0;
	}
	draw_canvasSetup(&p_globals->canvas, w, h, (uint32 *)p_pixels);

	draw_stroke *p_stroke=rtu_memAlloc(sizeof(draw_stroke));
	if(p_stroke) {
		draw_brush *p_brush=(draw_brush *)brush_ptr;
		if(p_brush) {
			draw_strokeInit(p_stroke, p_brush, p_globals);
		} else {
			LOG_ASSERT(false, "invalid brush ptr");
		}
	} else {
		LOG_ASSERT(false, "failed to malloc stroke");
	}
	if(renderReleaseCanvas(env, pixels, p_pixels)) {
		return (jlong)p_stroke;
	} else {
		return 0;
	}
}
/*
void renderRecordIter2Grads(const jlong stroke_ptr, draw_grad *p_grad_arrs[]) {
	draw_stroke *p_stroke=(draw_stroke *)stroke_ptr;
	if(p_stroke) {
		if(p_stroke->brush.breadth>0) {
			for(uint32 down=0; down<=1; down++) {
				for(uint32 right=0; right<=1; right++) {
					p_grad_arrs[(down<<1)+right]=&p_stroke->brush.blot.left_2_down_2_iter_2_grads[down][right];
				}
			}
		}
	} else {
		LOG_ASSERT(false, "null stroke ptr");
	}
}

void renderValidateIter2Grads(const jlong stroke_ptr, draw_grad *p_grad_arrs[]) {
	draw_stroke *p_stroke=(draw_stroke *)stroke_ptr;
	if(p_stroke) {
		if(p_stroke->brush.breadth>0) {
			for(uint32 down=0; down<=1; down++) {
				for(uint32 right=0; right<=1; right++) {
					LOG_ASSERT(p_grad_arrs[(down<<1)+right]==&p_stroke->brush.blot.left_2_down_2_iter_2_grads[down][right], "iter_2_grad mismatch at: %u, %u", down, right);
				}
			}
		}
	} else {
		LOG_ASSERT(false, "null stroke ptr");
	}
}
*/
void Java_com_hourglassapps_tiles_render_CRenderEngine_renderStrokeMoveTo(JNIEnv* env, jobject ref, jlong stroke_ptr,
		jfloat x, jfloat y, jintArray pixels, jint w, jint h, jlong globals_ptr) {
	//draw_grad *p_grad_arrs[4];
	//renderRecordIter2Grads(stroke_ptr, p_grad_arrs);

	draw_globals *p_globals=(draw_globals *)globals_ptr;
	//draw_globalsLoadR(p_globals);

	jint* p_pixels=renderGetCanvas(env, pixels, w, h);
	if(!p_pixels) {
		return;
	}
	draw_canvasSetup(&p_globals->canvas, w, h, (uint32 *)p_pixels);

	draw_stroke *p_stroke=(draw_stroke *)stroke_ptr;
	if(p_stroke) {
		draw_vert pos={ .x=x, .y=y };
		draw_strokeMoveTo(p_stroke, &pos, p_globals);
	} else {
		LOG_ASSERT(false, "invalid stroke ptr");
	}
	renderReleaseCanvas(env, pixels, p_pixels);

	//renderValidateIter2Grads(stroke_ptr, p_grad_arrs);
}

void Java_com_hourglassapps_tiles_render_CRenderEngine_renderStrokeQuadTo(JNIEnv* env, jobject ref, jlong stroke_ptr, jfloat x1, jfloat y1, jfloat x2, jfloat y2, jintArray pixels, jint w, jint h, jlong globals_ptr) {
	//draw_grad *p_grad_arrs[4];
	//renderRecordIter2Grads(stroke_ptr, p_grad_arrs);

	draw_globals *p_globals=(draw_globals *)globals_ptr;
	//draw_globalsLoadR(p_globals);

	jint* p_pixels=renderGetCanvas(env, pixels, w, h);
	if(!p_pixels) {
		return;
	}
	draw_canvasSetup(&p_globals->canvas, w, h, (uint32 *)p_pixels);

	draw_stroke *p_stroke=(draw_stroke *)stroke_ptr;
	if(p_stroke) {

		draw_vert pos_1={ .x=x1, .y=y1 };
		draw_vert pos_2={ .x=x2, .y=y2 };
		draw_strokeQuadTo(p_stroke, &pos_1, &pos_2, p_globals);

	} else {
		LOG_ASSERT(false, "invalid stroke ptr");
	}

	renderReleaseCanvas(env, pixels, p_pixels);

	//renderValidateIter2Grads(stroke_ptr, p_grad_arrs);
}

/*
void Java_com_hourglassapps_tiles_render_CRenderEngine_renderStrokeReset(JNIEnv* env, jobject ref, jlong stroke_ptr, jintArray pixels, jint w, jint h, jlong globals_ptr) {
	//draw_grad *p_grad_arrs[4];
	//renderRecordIter2Grads(stroke_ptr, p_grad_arrs);

	draw_globals *p_globals=(draw_globals *)globals_ptr;
	//draw_globalsLoadR(p_globals);

	jint* p_pixels=renderGetCanvas(env, pixels, w, h);
	if(!p_pixels) {
		return;
	}

	draw_canvasSetup(&p_globals->canvas, w, h, (uint32 *)p_pixels);

	draw_stroke *p_stroke=(draw_stroke *)stroke_ptr;
	if(p_stroke) {
		draw_strokeReset(p_stroke, p_globals);
	} else {
		LOG_ASSERT(false, "invalid stroke ptr");
	}

	renderReleaseCanvas(env, pixels, p_pixels);
	//renderValidateIter2Grads(stroke_ptr, p_grad_arrs);
}
*/

void Java_com_hourglassapps_tiles_render_CRenderEngine_renderStrokeRender(JNIEnv* env, jobject ref, jlong stroke_ptr, jintArray pixels, jint w, jint h, jlong globals_ptr) {
	//draw_grad *p_grad_arrs[4];
	//renderRecordIter2Grads(stroke_ptr, p_grad_arrs);

	draw_globals *p_globals=(draw_globals *)globals_ptr;
	//draw_globalsLoadR(p_globals);

	jint* p_pixels=renderGetCanvas(env, pixels, w, h);
	if(!p_pixels) {
		return;
	}
	draw_canvasSetup(&p_globals->canvas, w, h, (uint32 *)p_pixels);

	draw_stroke *p_stroke=(draw_stroke *)stroke_ptr;
	if(p_stroke) {
		draw_strokeRender(p_stroke, p_globals);
	} else {
		LOG_ASSERT(false, "invalid stroke ptr");
	}

	renderReleaseCanvas(env, pixels, p_pixels);
	//renderValidateIter2Grads(stroke_ptr, p_grad_arrs);
}

void Java_com_hourglassapps_tiles_render_CRenderEngine_renderStrokeDestroy(JNIEnv* env, jobject ref, jlong stroke_ptr, jintArray pixels, jint w, jint h, jlong globals_ptr) {
	draw_globals *p_globals=(draw_globals *)globals_ptr;
	//draw_globalsLoadR(p_globals);

	draw_stroke *p_stroke=(draw_stroke *)stroke_ptr;
	if(p_stroke) {
		rtu_memFree(p_stroke);
	} else {
		LOG_ASSERT(false, "invalid stroke ptr");
	}
}

jlong Java_com_hourglassapps_tiles_render_CRenderEngine_renderBrushInit(JNIEnv* env, jobject ref, jint col, jfloat breadth, jfloat blur_width, jintArray pixels, jint w, jint h, jlong globals_ptr) {
	draw_globals *p_globals=(draw_globals *)globals_ptr;
	//draw_globalsLoadR(p_globals);

	jint* p_pixels=renderGetCanvas(env, pixels, w, h);
	if(!p_pixels) {
		return 0;
	}
	draw_canvasSetup(&p_globals->canvas, w, h, (uint32 *)p_pixels);

	draw_brush *p_brush=rtu_memAlloc(sizeof(draw_brush));
	if(p_brush) {
		draw_brushInit(p_brush, col, breadth, blur_width, p_globals);
	} else {
		LOG_ASSERT(false, "failed to malloc brush");
		return 0;
	}

	if(renderReleaseCanvas(env, pixels, p_pixels)) {
		return (jlong)p_brush;
	} else {
		return 0;
	}
}

jfloat Java_com_hourglassapps_tiles_render_CRenderEngine_renderBrushBreadth(JNIEnv* env, jobject ref, jlong brush_ptr, jintArray pixels, jint w, jint h, jlong globals_ptr) {
	draw_globals *p_globals=(draw_globals *)globals_ptr;
	//draw_globalsLoadR(p_globals);

	draw_brush *p_brush=(draw_brush *)brush_ptr;
	if(p_brush) {
		return p_brush->blot.breadth;
	} else {
		LOG_ASSERT(false, "invalid brush ptr");
		return 0;
	}
}

jint Java_com_hourglassapps_tiles_render_CRenderEngine_renderBrushCol(JNIEnv* env, jobject ref, jlong brush_ptr, jintArray pixels, jint w, jint h, jlong globals_ptr) {
	draw_globals *p_globals=(draw_globals *)globals_ptr;
	//draw_globalsLoadR(p_globals);

	draw_brush *p_brush=(draw_brush *)brush_ptr;
	if(p_brush) {
		return p_brush->b.col;
	} else {
		LOG_ASSERT(false, "invalid brush ptr");
		return 0;
	}
}

void Java_com_hourglassapps_tiles_render_CRenderEngine_renderBrushDestroy(JNIEnv* env, jobject ref, jlong brush_ptr, jintArray pixels, jint w, jint h, jlong globals_ptr) {
	draw_globals *p_globals=(draw_globals *)globals_ptr;
	//draw_globalsLoadR(p_globals);

	draw_brush *p_brush=(draw_brush *)brush_ptr;
	if(p_brush) {
		draw_brushDestroy(p_brush);
		rtu_memFree(p_brush);
	} else {
		LOG_ASSERT(false, "invalid stroke ptr");
	}
}

void Java_com_hourglassapps_tiles_render_CRenderEngine_renderBlot(
		JNIEnv* env, jobject ref, jfloat x, jfloat y, jfloat breadth, jfloat blur, jint col, jintArray pixels, jint w, jint h,
		jlong globals_ptr) {
	LOG_INFO("start blot");
	draw_globals *p_globals=(draw_globals *)globals_ptr;
	//draw_globalsLoadR(p_globals);

	jint* p_pixels=renderGetCanvas(env, pixels, w, h);
	if(!p_pixels) {
		return;
	}
	draw_canvasSetup(&p_globals->canvas, w, h, (uint32 *)p_pixels);

	draw_vert pt={ .x=x, .y=y };
	LOG_INFO("blot at %f, %f", x, y);
	draw_blotHere(&pt, breadth, blur, col, p_globals);

	renderReleaseCanvas(env, pixels, p_pixels);
	LOG_INFO("end blot");
}

/*
jfloat Java_com_hourglassapps_tiles_render_CRenderEngine_renderBlurWidth(JNIEnv* env, jobject ref, jfloat breadth, jintArray pixels, jint w, jint h, jlong globals_ptr) {
	return draw_blurWidth(breadth);
}
*/

void Java_com_hourglassapps_tiles_render_CRenderEngine_renderStartProfiling(JNIEnv* env, jobject ref) {
	//monstartup("tiles.so");
}

void Java_com_hourglassapps_tiles_render_CRenderEngine_renderStopProfiling(JNIEnv* env, jobject ref) {
	//moncleanup();
}
