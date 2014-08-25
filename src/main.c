/* SDK Libraries */
#include <gccore.h>
#include <stdio.h>

#include "mtrand.h"
#include "gxutils.h"
#include "pathtracer.h"
#include "mathutils.h"
#include "scene.h"

int main(int argc, char **argv) {
	FncMtSrand(time(NULL));

	// Initialize graphics
	GXU_init();

	// Frame buffer
	u16 renderWidth = rmode->viWidth >> 1;
	u16 renderHeight = rmode->viHeight >> 1;
	printf("starting %u %u", renderWidth, renderHeight);
	GXU_createPixelBuffer(renderWidth, renderHeight);
	GXU_clearPixelBuffer(0xFFFF00FF);

	// Create scene
	scene_t* scene = SCENE_create();

	//Materials
	material_t diffwhite; MAT_init(&diffwhite, Vector(1, 1, 1), 0.0f, 0);
	material_t glowwhite; MAT_init(&glowwhite, Vector(1, 1, 1), 1.0f, 0);
	material_t diffred; MAT_init(&diffred, Vector(1, 0, 0), 0.0f, 0);
	material_t diffgreen; MAT_init(&diffgreen, Vector(0, 1, 0), 0.0f, 0);
	material_t diffblue; MAT_init(&diffblue, Vector(0, 0, 1), 0.0f, 0);

	//Balls
	sphere_t sphere1; SPHERE_init(&sphere1, Vector(0, -5, 25), 5.0f, diffwhite);
	sphere_t sphere2; SPHERE_init(&sphere2, Vector(10, 0, 35), 5.0f, diffwhite);
	sphere_t sphere3; SPHERE_init(&sphere3, Vector(-10, 0, 35), 5.0f, diffwhite);

	//Bottom Floor
	plane_t floor; PLANE_init(&floor, Vector(0, -10, 0), Vector(0, 1, 0), diffwhite);
	plane_t ceiling; PLANE_init(&ceiling, Vector(0, 20, 0), Vector(0, -1, 0), glowwhite);

	//Side walls
	plane_t leftwall; PLANE_init(&leftwall, Vector(-20, 0, 0), Vector(1, 0, 0), diffred);
	plane_t rightwall; PLANE_init(&rightwall, Vector(20, 0, 0), Vector(-1, 0, 0), diffgreen);

	//Back wall
	plane_t backwall; PLANE_init(&backwall, Vector(0, 0, 40), Vector(0, 0, -1), diffblue);


	SCENE_addSphere(scene, sphere1);
	SCENE_addSphere(scene, sphere2);
	SCENE_addSphere(scene, sphere3);
	SCENE_addPlane(scene, floor);
	SCENE_addPlane(scene, ceiling);
	SCENE_addPlane(scene, leftwall);
	SCENE_addPlane(scene, rightwall);
	SCENE_addPlane(scene, backwall);

	// Create and setup pathtracer information
	pathtracer_t* tracer = PATH_create(rmode->viWidth, rmode->viHeight, renderWidth, renderHeight);

	// Initialize camera and generate initial rays
	CAM_init(&tracer->camera, tracer->width, tracer->height);

	while (1) {
		if (SYS_ResetButtonDown()) return 0;

		//Pathtrace scene
		PATH_generateRays(tracer);
		PATH_draw(tracer, scene);

		//Render buffer to screen
		GXU_renderPixelBuffer();
		GXU_done();
	}

	SCENE_destroy(scene);
	PATH_destroy(tracer);

	return 0;
}