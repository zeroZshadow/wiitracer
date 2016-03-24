/* SDK Libraries */
#include <gccore.h>
#include <stdio.h>

#include "mtrand.h"
#include "gxutils.h"
#include "pathtracer.h"
#include "mathutils.h"
#include "scene.h"

BOOL isRunning;
void OnResetCalled();

int main() { //int argc, char **argv) {
	SYS_SetResetCallback(OnResetCalled);

	FncMtSrand(time(NULL));

	// Initialize graphics
	GXU_init();

	// Frame buffer
	u16 renderWidth = vmode->viWidth >> 2;
	u16 renderHeight = vmode->viHeight >> 2;

	GXU_createPixelBuffer(renderWidth, renderHeight);
	GXU_clearPixelBuffer(0xFF000000);

	// Render first frame, so there is no corruption
	GXU_renderPixelBuffer();
	GXU_done();

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

	//Walls/Floors
	plane_t floor; PLANE_init(&floor, Vector(0, -10, 0), Vector(0, 1, 0), diffwhite);
	plane_t ceiling; PLANE_init(&ceiling, Vector(0, 20, 0), Vector(0, -1, 0), glowwhite);
	plane_t leftwall; PLANE_init(&leftwall, Vector(-20, 0, 0), Vector(1, 0, 0), diffred);
	plane_t rightwall; PLANE_init(&rightwall, Vector(20, 0, 0), Vector(-1, 0, 0), diffgreen);
	plane_t backwall; PLANE_init(&backwall, Vector(0, 0, 40), Vector(0, 0, -1), diffblue);

	// Add objects to scene
	SCENE_addSphere(scene, sphere1);
	SCENE_addSphere(scene, sphere2);
	SCENE_addSphere(scene, sphere3);
	SCENE_addPlane(scene, floor);
	SCENE_addPlane(scene, ceiling);
	SCENE_addPlane(scene, leftwall);
	SCENE_addPlane(scene, rightwall);
	SCENE_addPlane(scene, backwall);

	// Create and setup pathtracer information
	pathtracer_t* tracer = PATH_create(vmode->viWidth, vmode->viHeight, renderWidth, renderHeight);

	// Initialize camera and generate initial rays
	CAM_init(&tracer->camera, tracer->width, tracer->height);

	isRunning = TRUE;
	while (isRunning) {

		// Generate rays
		PATH_generateRays(tracer);

		const u32 RayHCount = tracer->hrayCount;
		const u32 RayVCount = tracer->vrayCount;
		u16 x, y;
		u32 i = 0;

		for (y = 0; y < RayVCount; y += TILESIZE) {
			for (x = 0; x < RayHCount; x += TILESIZE) {
				// Draw rays
				i = PATH_draw(tracer, scene, x, y, i);
			}

			// Render buffer to screen
			//TODO: Throw this on a thread
			GXU_renderPixelBuffer();
			GXU_done();
		}

		tracer->pass += 1.0;
	}

	SCENE_destroy(scene);
	PATH_destroy(tracer);

	return 0;
}

void OnResetCalled() {
	isRunning = FALSE;
}