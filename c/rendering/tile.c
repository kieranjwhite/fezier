#include <math.h>
#include <limits.h>

#include "draw.h"
#include "tile.h"
#include "../rtu.h"
#include "../ccan/typesafe_cb/typesafe_cb.h"

uint32 nextOrd=0;

uint32 tile_render(uint32 *p_strokes, uint32 strokesLen, uint32 left, uint32 top, uint32 right, uint32 bottom, float32 reduction) {
  LOG_INFO("tile_render. start");
  //int w=right-left+1;
  //int h=bottom-top+1;
  draw_vert dim;

  dim.x=CEIL32((1+right-left)/reduction);
  dim.y=CEIL32((1+bottom-top)/reduction);

  //returns next ord to plot from
  if(nextOrd>=strokesLen) {
    return strokesLen;
  }
  nextOrd=strokesLen;

  draw_vert box_dims={.x=150.0f, .y=150.0f};
  draw_vert p_0={.x=22.0f, .y=22.0f};
  draw_vert p_1={.x=128.0f, .y=22.0f};
  draw_vert p_2={.x=128.0f, .y=128.0f};

  LOG_ASSERT(box_dims.x>0 && box_dims.y>0, "box_dims too small %f, %f", box_dims.x, box_dims.y);
  //draw_thickQuad(&box_dims, &p_0, &p_1, &p_2, 0xff0000ff, 20);

  return strokesLen;
}

void tile_init(uint32 w, uint32 h) {
  draw_init(w, h);
  rtu_initFastDiv(DRAW_DIV_LIMIT);
}

void tile_finalise() {
  rtu_finaliseDiv();
  draw_finalise();
}

uint32 tile_size(uint32 w, uint32 h) {
    return w*h*sizeof(uint32);
}

void tile_reset() {
  nextOrd=0;
}
