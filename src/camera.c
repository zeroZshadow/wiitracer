#include "camera.h"

void CAM_init(camera_t* camera, u16 width, u16 height) {

	// Camera position rotation
	guVector zero = { 0, 0, 0 };
	camera->position = zero;
	guMtxIdentity(camera->rotMatrix);
	guMtxIdentity(camera->endMatrix);
	c_guMtxQuat(camera->rotMatrix, &camera->rotation);

	// Screen info
	const u16 hw = width / 2;
	const u16 hh = height / 2;
	guVector topleft = { -hw, hh, hw };
	guVector bottomright = { hw, -hh, hw };
	camera->point0 = topleft;
	camera->point1 = bottomright;
}