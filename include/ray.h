#ifndef _RAY_H
#define _RAY_H

#include <ogc/gu.h>

typedef struct {
	guVector origin;
	guVector direction;
} ray_t;

#endif