/* SDK Libraries */
#include <gccore.h>

#include "mtrand.h"
#include "gxutils.h"
#include "pathtracer.h"
#include "mathutils.h"
#include "scene.h"

int main(int argc, char **argv) {

	FncMtSrand(0);

	// Initialize graphics
	GXU_init();

	// Frame buffer
	u16 renderWidth = rmode->viWidth >> 2;
	u16 renderHeight = rmode->viHeight >> 2;
	GXU_createPixelBuffer(renderWidth, renderHeight);
	GXU_clearPixelBuffer(0xFFFF00FF);

	// Create scene
	scene_t* scene = SCENE_create();

	//White emissive
	material_t mat1; MAT_init(&mat1, Vector(1, 1, 1), 8.0f, 0);
	sphere_t sphere1; SPHERE_init(&sphere1, Vector(0, 0, 30), 5.0f, mat1);

	//Green diffuse
	material_t mat2; MAT_init(&mat2, Vector(0, 1, 0), 0.0f, 0);
	sphere_t sphere2; SPHERE_init(&sphere2, Vector(20, 0, 30), 5.0f, mat2);

	//Red diffuse
	material_t mat3; MAT_init(&mat3, Vector(1, 0, 0), 0.0f, 0);
	sphere_t sphere3; SPHERE_init(&sphere3, Vector(-20, 0, 30), 5.0f, mat3);

	SCENE_addSphere(scene, sphere1);
	SCENE_addSphere(scene, sphere2);
	SCENE_addSphere(scene, sphere3);

	// Create and setup pathtracer information
	pathtracer_t* tracer = PATH_create(renderWidth, renderHeight, renderWidth, renderHeight);

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

	PATH_destroy(tracer);

	return 0;
}