#include "plane.h"
#include "mu.h"
#include <math.h>

void PLANE_init(plane_t* plane, guVector position, guVector normal, material_t material) {
	plane->material = material;
	plane->position = position;
	plane->normal = normal;
	muVecNormalize(&plane->normal);
}

inline void PLANE_raycast(plane_t* plane, ray_t* ray, hitinfo_t* current) {
	guVector delta;
	muVecSub(&plane->position, &ray->origin, &delta);

	const f32 B = muVecDotProduct(&plane->normal, &ray->direction);
	const f32 C = muVecDotProduct(&plane->normal, &delta);
	const f32 dist = C / B;

	if (dist > 0.0f && dist < current->distance) {
		current->material = plane->material;
		current->distance = dist;

		muVecScale(&ray->direction, &current->position, dist);
		muVecAdd(&ray->origin, &current->position, &current->position);

		current->normal = plane->normal;

		current->hit = TRUE;
	}
}