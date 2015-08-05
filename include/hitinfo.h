#ifndef _HITINFO_H
#define _HITINFO_H

#include <ogc/gu.h>
#include "material.h"

typedef struct {
	material_t material;
	guVector position;
	guVector normal;
	f32 distance;
	BOOL hit;
} hitinfo_t;

#define hitcallback(x) void(*x)(hitinfo_t*, hitinfo_t*)

#endif