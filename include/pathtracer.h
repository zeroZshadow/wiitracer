#ifndef _PATHTRACER_H
#define _PATHTRACER_H

#define MAXDEPTH 4

#include <gccore.h>
#include "scene.h"
#include "camera.h"
#include "ray.h"

typedef struct {
	u16 width, height;
	u16 hrayCount, vrayCount;
	u16 rayCount;
	u16 depth;

	u16 pass;
	f32 RCP_pass;
	camera_t camera;

	raypath_t* raypaths;
} pathtracer_t;

pathtracer_t* PATH_create(u16 width, u16 height, u16 hcount, u16 vcount);

void PATH_destroy(pathtracer_t* tracer);

void PATH_generateRays(pathtracer_t* tracer);

void PATH_draw(pathtracer_t* tracer, scene_t* scene);

GXColor PATH_trace(raypath_t* path, scene_t* scene);

#endif