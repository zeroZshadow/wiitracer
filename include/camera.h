#ifndef _CAMERA_H
#define _CAMERA_H

#include <ogc/gu.h>

typedef struct {
	guVector position;
	guQuaternion rotation;

	Mtx rotMatrix;
	Mtx endMatrix;

	//Screen
	guVector point0, point1;
} camera_t;

void CAM_init(camera_t* camera, u16 width, u16 height);

#endif