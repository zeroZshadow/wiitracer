#include "pathtracer.h"

#include <malloc.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#include "gxutils.h"
#include "mathutils.h"
#include "mu.h"
#include "mtrand.h"

pathtracer_t* PATH_create(u32 width, u32 height, u32 hcount, u32 vcount) {
	pathtracer_t* tracer = malloc(sizeof(pathtracer_t));
	if (tracer == NULL) {
		printf("failed to alloc tracer");
		return 0;
	}

	// Fill in object data
	tracer->width = width;
	tracer->height = height;
	tracer->hrayCount = hcount;
	tracer->vrayCount = vcount;
	tracer->rayCount = hcount * vcount;

	// Allocate rays
	tracer->rays = malloc(sizeof(ray_t)* tracer->rayCount);
	if (tracer->rays == NULL) {
		printf("failed to alloc raypaths");
		free(tracer);
		return 0;
	}

	u32 fbuffersize = sizeof(guVector)* hcount * vcount;
	tracer->fbuffer = malloc(fbuffersize);
	if (tracer->fbuffer == NULL) {
		printf("failed to alloc fbuffer");
		free(tracer->rays);
		free(tracer);
		return 0;
	} else {
		memset(tracer->fbuffer, 0, fbuffersize);
	}

	// Reset pass count
	tracer->pass = 0.0;

	// Return object
	return tracer;
}


void PATH_destroy(pathtracer_t* tracer) {
	free(tracer->fbuffer);
	free(tracer->rays);
	free(tracer);
}

void PATH_generateRays(pathtracer_t* tracer) {
	camera_t* camera = &tracer->camera;
	guVector delta;
	muVecSub(&camera->point1, &camera->point0, &delta);
	const u16 RayHCount = tracer->hrayCount;
	const u16 RayVCount = tracer->vrayCount;

	// Create all ray data as identity
	const f32 HBlock = delta.y / (f32)RayVCount;
	const f32 VBlock = delta.x / (f32)RayHCount;

	// Rays are tiled because of how the texture works
	u32 i = 0;
	u16 x, y, ix, iy;
	for (y = 0; y < RayVCount; y += TILESIZE) {
		for (x = 0; x < RayHCount; x += TILESIZE) {
			//This is a single tile
			for (iy = 0; iy < TILESIZE; ++iy) {
				for (ix = 0; ix < TILESIZE; ++ix) {
					// Calculate direction
					guVector rpos = {
						VBlock * ( (float)(x + ix) + fioraRand()),
						HBlock * ( (float)(y + iy) + fioraRand()),
						0
					};
					guVector* direction = &tracer->rays[i].direction;

					// Calculate direction
					muVecAdd(&rpos, &camera->point0, direction);
					muVecSub(direction, &camera->position, direction);
					muVecNormalize(direction);

					// Set Ray origin
					tracer->rays[i].origin = camera->position;

					// Next ray
					++i;
				}
			}
		}
	}
}

void PATH_draw(pathtracer_t* tracer, scene_t* scene) {
	// Since we do not have a massive amount of memory, we have to pathtrace is the tiling order so we only have to store 4*4 pixels of data
	const u32 RayHCount = tracer->hrayCount;
	const u32 RayVCount = tracer->vrayCount;
	GXColor datatile[TILESIZE*TILESIZE];

	const f64 blendvalue = tracer->pass / (tracer->pass + 1.0);

	u16 x, y, ix, iy;
	u32 i = 0;// , r;
	for (y = 0; y < RayVCount; y += TILESIZE) {
		for (x = 0; x < RayHCount; x += TILESIZE) {
			//This is a single tile
			for (iy = 0; iy < TILESIZE; ++iy) {
				for (ix = 0; ix < TILESIZE; ++ix) {
					const u32 index = (x + ix) + ((y + iy) * RayHCount);
					ray_t* ray = &tracer->rays[i];
					guVector pixel = tracer->fbuffer[index];

					// Raytrace same pixel R times
					//for (r = 0; r < 1; ++r) {
						//Get pixel color for tile
						const guVector color = PATH_trace(ray, scene);

						//Blend with old pixel
						pixel = GXU_blendColors(color, pixel, blendvalue);
					//}

					//Store pixel back
					tracer->fbuffer[index] = pixel;

					//Convert to u32
					guVecMax(&pixel, 1.0f);
					guVecMin(&pixel, 0.0f);
					datatile[ix + (iy * TILESIZE)] = GXU_vectorToColorData(pixel);
					++i;
				}
			}


			//Entire tile traced, write to texture
			GXU_copyTilePixelBuffer((u32*)datatile, x / TILESIZE, y / TILESIZE);
		}
	}

	tracer->pass += 1.0;
}

void callback(hitinfo_t* hitinfo, hitinfo_t* out) {
	if (hitinfo->distance < out->distance) {
		*out = *hitinfo;
	}
}

guVector PATH_trace(ray_t* ray, scene_t* scene) {
	guVector color = { 1, 1, 1 };
	const guVector black = { 0, 0, 0 };
	ray_t currentRay;
	f32 cost = 1.0f;
	currentRay.origin = ray->origin;
	currentRay.direction = ray->direction;

	u8 depth;
	for (depth = 0; depth < MAXDEPTH; ++depth) {
		u8 i;
		hitinfo_t hitinfo;
		hitinfo.distance = INFINITY;
		hitinfo.hit = FALSE;

		for (i = 0; i < scene->spherecount; ++i) {
			SPHERE_raycast(&scene->spheres[i], &currentRay, &hitinfo, callback);
		}
		for (i = 0; i < scene->planecount; ++i) {
			PLANE_raycast(&scene->planes[i], &currentRay, &hitinfo, callback);
		}

		if (hitinfo.hit == FALSE) {
			//Nothing was hit, eary out
			return black;
		}

		// Alter the current color
		muVecScale(&hitinfo.material.color, &hitinfo.material.color, cost);
		ps_float3Mul(&color, &hitinfo.material.color, &color);

		if (hitinfo.material.emissive > 0.0f) {
			muVecScale(&color, &color, hitinfo.material.emissive);
			return color;
		}

		//TODO: Holy shit this is slow
		currentRay.direction = RandomVectorInHemisphere(&hitinfo.normal);
		currentRay.origin = hitinfo.position;

		//Calculate cost of next color
		cost = fabs(muVecDotProduct(&hitinfo.normal, &currentRay.direction));
	}

	return black;
}