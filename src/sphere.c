#include "sphere.h"
#include <math.h>
#include "mu.h"

void SPHERE_init(sphere_t* sphere, guVector position, f32 radius, material_t material) {
	sphere->material = material;
	sphere->position = position;
	sphere->radius = radius;
	sphere->sqr_radius = radius * radius;
	sphere->rcp_radius = 1.0f / radius;
}

void SPHERE_raycast(sphere_t* sphere, ray_t* ray, hitinfo_t* current) {
	guVector delta;
	muVecSub(&ray->origin, &sphere->position, &delta);

	f32 B, C;
	B = muVecDotProduct(&delta, &ray->direction);
	C = muVecDotProduct(&delta, &delta);
	C -= sphere->sqr_radius;
	const f32 D = B * B - C;

	if (D <= 0.0f) return;

	const f32 dist = -B - muSqrtf(D);
	if (dist >= 0.0f && dist < current->distance) {
		current->material = sphere->material;
		current->distance = dist;

		muVecScale(&ray->direction, &current->position, dist);
		muVecAdd(&ray->origin, &current->position, &current->position);

		muVecSub(&current->position, &sphere->position, &current->normal);
		muVecScale(&current->normal, &current->normal, sphere->rcp_radius);

		current->hit = TRUE;
	}
}