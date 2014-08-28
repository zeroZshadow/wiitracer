#ifndef _PATHTRACER_H
#define _PATHTRACER_H

#include <gccore.h>
#include "scene.h"
#include "camera.h"
#include "ray.h"
#include "gxutils.h"
#include "profiler.h"

extern profiler_t trace, sphere, plane, output;

typedef struct {
	u32 width, height;
	u32 hrayCount, vrayCount;
	u32 rayCount;

	u32 pass;
	camera_t camera;

	raypath_t* raypaths;
	guVector* fbuffer;
} pathtracer_t;

pathtracer_t* PATH_create(u32 width, u32 height, u32 hcount, u32 vcount);

void PATH_destroy(pathtracer_t* tracer);

void PATH_generateRays(pathtracer_t* tracer);

void PATH_draw(pathtracer_t* tracer, scene_t* scene);

guVector PATH_trace(raypath_t* path, scene_t* scene);

#endif