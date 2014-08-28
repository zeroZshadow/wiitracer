#ifndef _SPHERE_H
#define _SPHERE_H

#include <ogc/gu.h>
#include "material.h"
#include "ray.h"
#include "hitinfo.h"

typedef struct {
	material_t material;
	guVector position;
	f32 radius;
	f32 sqr_radius;
	f32 rcp_radius;
} sphere_t;

void SPHERE_init(sphere_t* sphere, guVector position, f32 radius, material_t material);

inline void SPHERE_raycast(sphere_t* sphere, ray_t* ray, hitinfo_t* current, hitcallback(callback));

#endif