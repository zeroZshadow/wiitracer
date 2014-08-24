#include "sphere.h"
#include <math.h>

inline void SPHERE_init(sphere_t* sphere, guVector position, f32 radius, material_t material) {
	sphere->position = position;
	sphere->material = material;

	sphere->radius = radius;
	sphere->sqr_radius = radius * radius;
	sphere->rcp_radius = 1.0f / radius;
}

BOOL SPHERE_raycast(sphere_t* sphere, ray_t* ray, hitinfo_t* out) {
	guVector distance;
	guVecSub(&ray->origin, &sphere->position, &distance);

	const f32 B = guVecDotProduct(&distance, &ray->direction);
	const f32 C = guVecDotProduct(&distance, &distance) - sphere->sqr_radius;
	const f32 D = B * B - C;

	if (D > 0.0f) {
		const f32 dist = -B - sqrtf(D);
		if (dist < 0.0f) return FALSE;

		out->material = sphere->material;
		out->distance = dist;
		
		//position = ray.origin + (ray.direction * distance)
		guVecScale(&ray->direction, &out->position, dist);
		guVecAdd(&ray->origin, &out->position, &out->position);

		//normal = (hitpos - pos) * rpc_radius
		guVecSub(&out->position, &sphere->position, &out->normal);
		guVecScale(&out->normal, &out->normal, sphere->rcp_radius);

		return TRUE;
	}

	return FALSE;
}