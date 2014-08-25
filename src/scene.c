#include "scene.h"
#include <malloc.h>

scene_t* SCENE_create() {
	scene_t* scene = malloc(sizeof(scene_t));
	if (scene == -1) {
		printf("failed to alloc scene");
		return 0;
	}

	SCENE_init(scene);
	return scene;
}

void SCENE_init(scene_t* scene) {
	scene->spherecount = 0;
}

void SCENE_destroy(scene_t* scene) {
	free(scene);
}

BOOL SCENE_addSphere(scene_t* scene, sphere_t sphere) {
	if (scene->spherecount == MAXPRIM - 1) {
		return FALSE;
	}

	scene->spheres[scene->spherecount++] = sphere;
	return TRUE;
}

BOOL SCENE_addPlane(scene_t* scene, plane_t plane) {
	if (scene->planecount == MAXPRIM - 1) {
		return FALSE;
	}

	scene->planes[scene->planecount++] = plane;
	return TRUE;
}