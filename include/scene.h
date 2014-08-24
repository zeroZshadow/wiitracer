#ifndef _SCENE_H
#define _SCENE_H

#include <ogc/gu.h>

#include "sphere.h"

#define MAXPRIM 5

typedef struct {
	sphere_t spheres[MAXPRIM];
	u8 spherecount;
} scene_t;

scene_t* SCENE_create();

void SCENE_init(scene_t* scene);

void SCENE_destroy(scene_t* scene);

BOOL SCENE_addSphere(scene_t* scene, sphere_t sphere);

#endif