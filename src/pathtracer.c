#include "pathtracer.h"
#include <malloc.h>
#include "gxutils.h"
#include "mathutils.h"
#include "mtrand.h"
#include <math.h>

pathtracer_t* PATH_create(u16 width, u16 height, u16 hcount, u16 vcount) {
	pathtracer_t* tracer = malloc(sizeof(pathtracer_t));

	// Fill in object data
	tracer->width = width;
	tracer->height = height;
	tracer->hrayCount = hcount;
	tracer->vrayCount = vcount;
	tracer->rayCount = hcount * vcount;

	// Allocate rays

	tracer->raypaths = malloc(sizeof(raypath_t)* tracer->rayCount);

	// Reset pass count
	tracer->pass = 0;
	tracer->RCP_pass = 0.0f;

	// Return object
	return tracer;
}


void PATH_destroy(pathtracer_t* tracer) {
	free(tracer->raypaths);
	free(tracer);
}

void PATH_generateRays(pathtracer_t* tracer) {
	camera_t* camera = &tracer->camera;
	guVector delta;
	guVecSub(&camera->point1, &camera->point0, &delta);
	const u16 RayHCount = tracer->hrayCount;
	const u16 RayVCount = tracer->vrayCount;

	// Create all ray data as identity
	const f32 HBlock = delta.y / (f32)RayVCount;
	const f32 VBlock = delta.x / (f32)RayHCount;

	// Rays are tiled because of how the texture works

	u32 i = 0;
	u16 x, y, ix, iy;
	for (y = 0; y<RayVCount; y += TILESIZE) {
		for (x = 0; x<RayHCount; x += TILESIZE) {
			//This is a single tile
			for (iy = 0; iy < TILESIZE; ++iy) {
				for (ix = 0; ix < TILESIZE; ++ix) {
					// Calculate direction
					guVector rpos = { (VBlock * (float)(x + ix)), (HBlock * (float)(y + iy)), 0 };
					guVector* direction = &tracer->raypaths[i].base.direction;

					// Calculate direction
					guVecAdd(&rpos, &camera->point0, direction);
					guVecSub(direction, &camera->position, direction);
					guVecNormalize(direction);

					// Set Ray origin
					tracer->raypaths[i].base.origin = camera->position;

					// Next ray
					++i;
				}
			}
		}
	}
}

void PATH_draw(pathtracer_t* tracer) {
	tracer->pass++;
	tracer->RCP_pass = 1.0f / (f32)tracer->pass;

	// Since we do not have a massive amount of memory, we have to pathtrace is the tiling order so we only have to store 4*4 pixels of data
	const u16 RayHCount = tracer->hrayCount;
	const u16 RayVCount = tracer->vrayCount;
	GXColor tile[TILESIZE*TILESIZE];

	u16 x, y, ix, iy;
	u32 i = 0;
	for (y = 0; y<RayVCount; y += TILESIZE) {
		for (x = 0; x<RayHCount; x += TILESIZE) {
			//This is a single tile
			for (iy = 0; iy < TILESIZE; ++iy) {
				for (ix = 0; ix < TILESIZE; ++ix) {
					raypath_t* path = &tracer->raypaths[i];
					path->depth = 0;

					tile[ix + (iy * TILESIZE)] = PATH_trace(path);

					// Set pixel color
					++i;
				}
			}

			//Entire tile traced, write to texture
			GXU_copyTilePixelBuffer((u32*)tile, x / TILESIZE, y / TILESIZE);
		}
	}
}

GXColor PATH_trace(raypath_t* path) {
	GXColor color = { 0xFF, 0xFF, 0xFF, 0xFF };
	guVector origin = path->base.origin;
	guVector direction = path->base.direction;

	u8 depth;
	for (depth = 0; depth < MAXDEPTH; ++depth) {
		//raycast from origin
		//no hit, black, out
		//hit, color info, emissive?
		//generate new ray
		guVector hitnormal;

		direction = RandomVectorInHemisphere(&hitnormal);


		//multiply last hit color with current hit color
		//If it was emissive, exit
	}

	return color;
}