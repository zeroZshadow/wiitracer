#ifndef _SCENE_H
#define _SCENE_H

#include <ogc/gu.h>

#include "sphere.h"
#include "plane.h"

#define MAXPRIM 6

typedef struct {
	sphere_t spheres[MAXPRIM];
	plane_t planes[MAXPRIM];
	u8 spherecount;
	u8 planecount;
} scene_t;

scene_t* SCENE_create();

void SCENE_init(scene_t* scene);

void SCENE_destroy(scene_t* scene);

BOOL SCENE_addSphere(scene_t* scene, sphere_t sphere);

BOOL SCENE_addPlane(scene_t* scene, plane_t plane);

#endif