#ifndef _MATERIAL_H
#define _MATERIAL_H

#include <ogc/gu.h>

typedef struct {
	guVector color;
	f32 emissive;
	f32 reflection;
} material_t;

void MAT_init(material_t* material, guVector color, f32 emissive, f32 reflection);

#endif