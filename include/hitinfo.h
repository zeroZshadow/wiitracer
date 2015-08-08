#ifndef _HITINFO_H
#define _HITINFO_H

#include <ogc/gu.h>
#include "material.h"

typedef struct {
	f32 distance;
	material_t material;
	guVector position;
	guVector normal;
	BOOL hit;
} hitinfo_t;

#endif