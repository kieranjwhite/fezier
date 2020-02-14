/* Copyright 2015 Kieran White and also copyright of the author of the
   minunit library (available at
   http://www.jera.com/techinfo/jtns/jtn002.html).

   This file is part of Fezier and is based on the minunit
   library. This file is dual-licenced as described below:

   (1) This program is free software: you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   (2) The original minunit licence: you may use the code in this file
   for any purpose, with the understanding that it comes with NO
   WARRANTY.
*/

#ifndef MIN_UNIT_H
#define MIN_UNIT_H

#include "rtu.h"
#include "types.h"

#define G_MU_LINE_LEN 256

extern sint8 G_MU_LINE[];

#define mu_assert(message, test) do { \
    if (!(test)  && rtu_onFail()) {					\
      snprintf(G_MU_LINE, G_MU_LINE_LEN, "ERROR %s:%u. %s", __FILE__, __LINE__, message); \
      G_MU_LINE[G_MU_LINE_LEN-1]=0;					\
      return G_MU_LINE;							\
    }									\
  } while (0)
#define mu_run_test(test) do { sint8 *message = test(); tests_run++;	\
    if (message) return message; } while (0)
extern uint32 tests_run;

#endif
