#include "sphere.h"
#include <math.h>

inline void SPHERE_init(sphere_t* sphere, guVector position, f32 radius, material_t material) {
	sphere->position = position;
	sphere->material = material;

	sphere->radius = radius;
	sphere->sqr_radius = radius * radius;
	sphere->rcp_radius = 1.0f / radius;
}

BOOL SPHERE_raycast(sphere_t* sphere, ray_t* ray, hitinfo_t* current, hitcallback(callback)) {
	guVector distance;
	guVecSub(&ray->origin, &sphere->position, &distance);

	const f32 B = guVecDotProduct(&distance, &ray->direction);
	const f32 C = guVecDotProduct(&distance, &distance) - sphere->sqr_radius;
	const f32 D = B * B - C;

	if (D > 0.0f) {
		const f32 dist = -B - sqrtf(D);
		if (dist < 0.0f) return FALSE;

		hitinfo_t info;
		info.material = sphere->material;
		info.distance = dist;
		info.hit = TRUE;
		
		//position = ray.origin + (ray.direction * distance)
		guVecScale(&ray->direction, &info.position, dist);
		guVecAdd(&ray->origin, &info.position, &info.position);

		//normal = (hitpos - pos) * rpc_radius
		guVecSub(&info.position, &sphere->position, &info.normal);
		guVecScale(&info.normal, &info.normal, sphere->rcp_radius);

		callback(info, current);

		return TRUE;
	}

	return FALSE;
}