/* SDK Libraries */
#include <gccore.h>

#include "mtrand.h"
#include "gxutils.h"
#include "pathtracer.h"

int main(int argc, char **argv) {

	FncMtSrand(0);

	// Initialize graphics
	GXU_init();

	// Frame buffer
	u16 renderWidth = rmode->viWidth / 2;
	u16 renderHeight = rmode->viHeight / 2;
	GXU_createPixelBuffer(renderWidth, renderHeight);
	GXU_clearPixelBuffer(0xFFFF00FF);

	// Create and setup pathtracer information
	pathtracer_t* tracer = PATH_create(renderWidth, renderHeight, renderWidth, renderHeight);

	// Initialize camera and generate initial rays
	CAM_init(&tracer->camera, tracer->width, tracer->height);
	PATH_generateRays(tracer);

	while (1) {
		if (SYS_ResetButtonDown()) return 0;

		//Pathtrace scene
		PATH_draw(tracer);

		//Render buffer to screen
		GXU_renderPixelBuffer();
		GXU_done();
	}

	PATH_destroy(tracer);

	return 0;
}