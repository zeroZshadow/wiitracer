#include "material.h"

void MAT_init(material_t* material, guVector color, f32 emissive, f32 reflection) {
	material->color = color;
	material->emissive = emissive;
	material->reflection = reflection;
}
