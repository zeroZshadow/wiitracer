#ifndef _PLANE_H
#define _PLANE_H

#include <ogc/gu.h>
#include "material.h"
#include "ray.h"
#include "hitinfo.h"

typedef struct {
	material_t material;
	guVector position;
	guVector normal;
} plane_t;

void PLANE_init(plane_t* plane, guVector position, guVector normal, material_t material);

void PLANE_raycast(plane_t* plane, ray_t* ray, hitinfo_t* current, hitcallback(callback));

#endif