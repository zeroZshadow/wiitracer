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

	//White emissive
	material_t mat1; MAT_init(&mat1, Vector(1, 1, 1), 0.0f, 0);
	sphere_t sphere1; SPHERE_init(&sphere1, Vector(0, -5, 25), 5.0f, mat1);

	//Green diffuse
	material_t mat2; MAT_init(&mat2, Vector(0, 1, 0), 0.0f, 0);
	sphere_t sphere2; SPHERE_init(&sphere2, Vector(10, 0, 35), 5.0f, mat2);

	//Red diffuse
	material_t mat3; MAT_init(&mat3, Vector(1, 0, 0), 1.0f, 0);
	sphere_t sphere3; SPHERE_init(&sphere3, Vector(-10, 0, 35), 5.0f, mat3);

	//Bottom Floor
	material_t mat4; MAT_init(&mat4, Vector(1, 1, 1), 0.0f, 0);
	plane_t plane1; PLANE_init(&plane1, Vector(0, -10, 0), Vector(0, 1, 0), mat4);

	//Back Panel
	material_t mat5; MAT_init(&mat5, Vector(0.9, 0.8, 0.9), 0.0f, 0);
	plane_t plane2; PLANE_init(&plane2, Vector(0, 0, 40), Vector(0, 0, -1), mat5);

	//FRONT Panel
	material_t mat6; MAT_init(&mat6, Vector(1, 1, 1), 0.8f, 0);
	plane_t plane3; PLANE_init(&plane3, Vector(0, 0, -40), Vector(0, 0, 1), mat6);

	SCENE_addSphere(scene, sphere1);
	SCENE_addSphere(scene, sphere2);
	SCENE_addSphere(scene, sphere3);
	SCENE_addPlane(scene, plane1);
	SCENE_addPlane(scene, plane2);
	SCENE_addPlane(scene, plane3);

	// Create and setup pathtracer information
	pathtracer_t* tracer = PATH_create(rmode->viWidth, rmode->viHeight, renderWidth, renderHeight);

	// Initialize camera and generate initial rays
	CAM_init(&tracer->camera, tracer->width, tracer->height);
	PATH_generateRays(tracer);

	while (1) {
		if (SYS_ResetButtonDown()) return 0;

		//Pathtrace scene
		PATH_draw(tracer, scene);

		//Render buffer to screen
		GXU_renderPixelBuffer();
		GXU_done();
	}

	SCENE_destroy(scene);
	PATH_destroy(tracer);

	return 0;
}