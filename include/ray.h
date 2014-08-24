#ifndef _RAY_H
#define _RAY_H

#include <ogc/gu.h>
#include "defines.h"

typedef struct {
	guVector origin;
	guVector direction;
} ray_t;

typedef struct {
	ray_t base;
	//ray_t path[MAXDEPTH];
	//u8 depth;
} raypath_t;

#endif