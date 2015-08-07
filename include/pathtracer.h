#ifndef _PATHTRACER_H
#define _PATHTRACER_H

#include <gccore.h>
#include "scene.h"
#include "camera.h"
#include "ray.h"
#include "gxutils.h"

#define MAXDEPTH 8

typedef struct {
	u32 width, height;
	u32 hrayCount, vrayCount;
	u32 rayCount;

	f64 pass;
	camera_t camera;

	ray_t* rays;
	guVector* fbuffer;
} pathtracer_t;

pathtracer_t* PATH_create(u32 width, u32 height, u32 hcount, u32 vcount);

void PATH_destroy(pathtracer_t* tracer);

void PATH_generateRays(pathtracer_t* tracer);

void PATH_draw(pathtracer_t* tracer, scene_t* scene);

guVector PATH_trace(ray_t* path, scene_t* scene);

#endif