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

#include <SDL.h>

#include "image_buf.h"
#include "../../types.h"
#include "../draw.h"

void render(SDL_Surface *p_screen, SDL_Surface *p_surface, draw_globals *p_globals, uint32 buf_width, uint32 buf_height) {
  //draw_globalsLoadR(p_globals);
  // Declare all the local variables.
  rtu_initFastDiv(DRAW_DIV_LIMIT, p_globals->p_rtu);

  draw_init(buf_width, buf_height, p_globals);
  uint32 obgr_red=0xff0000ff;

  draw_vert box_dims={.x=150.0f, .y=150.0f};
  LOG_ASSERT(box_dims.x>0 && box_dims.y>0, "box_dims too small %f, %f", box_dims.x, box_dims.y);

  draw_vert p_0={.x=22.0f, .y=22.0f};
  draw_vert p_1={.x=128.0f, .y=22.0f};
  draw_vert p_2={.x=234.0f, .y=22.0f};
  draw_vert p_3={.x=234.0f, .y=128.0f};
  draw_vert p_4={.x=234.0f, .y=220.0f};
  draw_vert p_5={.x=128.0f, .y=220.0f};
  draw_vert p_6={.x=22.0f, .y=220.0f};
  draw_vert p_7={.x=22.0f, .y=128.0f};

  draw_vert p_0b={.x=buf_width-p_0.x*1.45, .y=p_0.y+240};
  draw_vert p_1b={.x=buf_width-p_1.x*1.45, .y=p_1.y+240};
  draw_vert p_2b={.x=buf_width-p_2.x*1.45, .y=p_2.y+240};
  draw_vert p_3b={.x=buf_width-p_3.x*1.45, .y=p_3.y+240};
  draw_vert p_4b={.x=buf_width-p_4.x*1.45, .y=p_4.y+240};
  draw_vert p_5b={.x=buf_width-p_5.x*1.45, .y=p_5.y+240};
  draw_vert p_6b={.x=buf_width-p_6.x*1.45, .y=p_6.y+240};
  draw_vert p_7b={.x=buf_width-p_7.x*1.45, .y=p_7.y+240};

  p_5b.y-=60;

  draw_vert p_8={.x=22.0f, .y=262.0f};
  draw_vert p_9={.x=128.0f, .y=262.0f};
  draw_vert p_a={.x=220.0f, .y=262.0f};

  draw_vert p_b={.x=200.0f, .y=300.0f};
  draw_vert p_c={.x=200.0f, .y=350.0f};
  draw_vert p_d={.x=200.0f, .y=405.0f};

  draw_vert p_e={.x=220.0f, .y=448.0f};
  draw_vert p_f={.x=128.0f, .y=448.0f};
  draw_vert p_g={.x=22.0f, .y=448.0f};

  draw_vert p_h={.x=42.0f, .y=405.0f};
  draw_vert p_i={.x=42.0f, .y=350.0f};
  draw_vert p_j={.x=42.0f, .y=300.0f};

  draw_vert p_l={.x=58.0f, .y=390.0f};
  draw_vert p_0c={.x=p_0b.x-100, .y=p_0b.y-150};
  
  /*
  draw_vert p_cen_o={ .x=128, .y=121 };
  draw_vert offset_o={ .x=416, .y=121 };
  draw_vert end_o={ .x=129, .y=273 };

  draw_vert origin={ .x=200, .y=200 };
  float32 ang=M_PI*0.25;
  draw_vert p_cen=draw_vertRotate(&p_cen_o, ang, &origin);
  draw_vert offset=draw_vertRotate(&offset_o, ang, &origin);
  draw_vert end=draw_vertRotate(&end_o, ang, &origin);
  */
  
  draw_vert p_cen={ .x=128, .y=121 };
  draw_vert offset={ .x=130, .y=121 };
  draw_vert end={ .x=130.25, .y=159 };
  
  draw_vert overlap_a={ .x=470, .y=330 };
  draw_vert overlap_b={ .x=362, .y=340 };
  draw_vert overlap_c={ .x=470, .y=350 };

  draw_brush thin_brush;
  draw_brushInit(&thin_brush, 0xff0000ff, 1, p_globals);

  draw_brush brush;
  draw_brushInit(&brush, 0xffffff00, 16, p_globals);

  draw_brush blot_brush;
  draw_brushInit(&blot_brush, 0xffff00ff, 44, p_globals);

  draw_brush thick_brush;
  draw_brushInit(&thick_brush, 0xff0000ff, 60, p_globals);

  image_buf_commit(p_surface, p_globals->canvas.p_bitmap, buf_width, buf_height);
  //draw_canvasIgnoreDirt(&p_globals->canvas);

  draw_stroke thin_stroke;
  draw_strokeInit(&thin_stroke, &thin_brush, p_globals);
  draw_strokeMoveTo(&thin_stroke, &p_1, p_globals);
  draw_strokeQuadTo(&thin_stroke, &p_2, &p_3, p_globals);
  draw_strokeQuadTo(&thin_stroke, &p_4, &p_5, p_globals);
  draw_strokeQuadTo(&thin_stroke, &p_6, &p_7, p_globals);
  draw_strokeQuadTo(&thin_stroke, &p_0, &p_1, p_globals);
  draw_strokeRender(&thin_stroke, p_globals);

  image_buf_commit(p_surface, p_globals->canvas.p_bitmap, buf_width, buf_height);
  //draw_canvasIgnoreDirt(&p_globals->canvas);

  draw_stroke thick_stroke;
  draw_strokeInit(&thick_stroke, &thick_brush, p_globals);

  draw_strokeMoveTo(&thick_stroke, &p_1b, p_globals);
  draw_strokeQuadTo(&thick_stroke, &p_2b, &p_3b, p_globals);
  draw_strokeQuadTo(&thick_stroke, &p_4b, &p_5b, p_globals);
  draw_strokeQuadTo(&thick_stroke, &p_6b, &p_7b, p_globals);
  draw_strokeQuadTo(&thick_stroke, &p_0b, &p_1b, p_globals);
  draw_strokeRender(&thick_stroke, p_globals);
#if 0
  draw_thickQuad(&p_1b, &p_2b, &p_3b, 0xff00ff00, 20, p_globals);
  draw_thickQuad(&p_3b, &p_4b, &p_5b, 0xff00ff00, 20, p_globals);
  draw_thickQuad(&p_5b, &p_6b, &p_7b, 0xff00ff00, 20, p_globals);
  draw_thickQuad(&p_7b, &p_0b, &p_1b, 0xff00ff00, 20, p_globals);

  draw_thickQuad(&p_8, &p_9, &p_a, 0xffff0000, 24, p_globals);
  draw_thickQuad(&p_e, &p_f, &p_g, 0xfff7007f, 24, p_globals);
#endif

  image_buf_commit(p_surface, p_globals->canvas.p_bitmap, buf_width, buf_height);
  //draw_canvasIgnoreDirt(&p_globals->canvas);

  draw_blotHere(&p_0c, 40, 0xff00ff00, p_globals);

  image_buf_commit(p_surface, p_globals->canvas.p_bitmap, buf_width, buf_height);
  //draw_canvasIgnoreDirt(&p_globals->canvas);

  draw_stroke overlap_stroke;
  draw_strokeInit(&overlap_stroke, &brush, p_globals);
  draw_strokeMoveTo(&overlap_stroke, &overlap_a, p_globals);
  draw_strokeQuadTo(&overlap_stroke, &overlap_b, &overlap_c, p_globals);
  draw_strokeRender(&overlap_stroke, p_globals);
  image_buf_commit(p_surface, p_globals->canvas.p_bitmap, buf_width, buf_height);
  //draw_canvasIgnoreDirt(&p_globals->canvas);

  draw_stroke blot_stroke;
  draw_strokeInit(&blot_stroke, &blot_brush, p_globals);
  draw_strokeMoveTo(&blot_stroke, &p_cen, p_globals);
  draw_strokeQuadTo(&blot_stroke, &offset, &end, p_globals);
  draw_strokeRender(&blot_stroke, p_globals);

  image_buf_commit(p_surface, p_globals->canvas.p_bitmap, buf_width, buf_height);
  //draw_canvasIgnoreDirt(&p_globals->canvas);

  draw_stroke stroke;
  draw_strokeInit(&stroke, &brush, p_globals);
  draw_strokeMoveTo(&stroke, &p_b, p_globals);
  draw_strokeQuadTo(&stroke, &p_c, &p_d, p_globals);
  draw_strokeQuadTo(&stroke, &p_l, &p_j, p_globals);
  draw_strokeQuadTo(&stroke, &p_i, &p_h, p_globals);
  draw_strokeRender(&stroke, p_globals);

  draw_brushDestroy(&thick_brush);
  draw_brushDestroy(&blot_brush);
  draw_brushDestroy(&brush);
  draw_brushDestroy(&thin_brush);
  /*
  draw_dot(0, 0, 0xffffffff);
  draw_dot(buf_width-1, buf_height-1, 0xffffffff);
  */
  draw_vert all_verts[]={ 
#if 0
    p_0, p_1, p_2, p_3, p_4, p_5, p_6, p_7,
    p_0b, p_1b, p_2b, 
    p_3b, p_4b, p_5b, 
    p_6b, p_7b, 
    p_0c,
    p_8, p_9, p_a, p_b, p_c, p_d, p_e, p_f, p_g, p_h, p_i, p_j,
    p_l, 
    p_cen, offset, end,
    overlap_a, overlap_b, overlap_c
#endif
  };

  for(uint32 i=0; i<DIM(all_verts); i++) {
    draw_dot(all_verts[i].x, all_verts[i].y, 0xffffffff, p_globals);
  }
  LOG_INFO("iter updates: %llu, pixels: %llu. ratio: %f", p_globals->draw_iter_updates, p_globals->draw_pixels, ((float32)p_globals->draw_iter_updates)/p_globals->draw_pixels);
}
