#include "sphere.h"
#include <math.h>
#include "mu.h"

void SPHERE_init(sphere_t* sphere, guVector position, f32 radius, material_t material) {
	sphere->position = position;
	sphere->material = material;

	sphere->radius = radius;
	sphere->sqr_radius = radius * radius;
	sphere->rcp_radius = 1.0f / radius;
}

void SPHERE_raycast(sphere_t* sphere, ray_t* ray, hitinfo_t* current, hitcallback(callback)) {
	guVector distance;
	muVecSub(&ray->origin, &sphere->position, &distance);

	f32 B, C;
	B = muVecDotProduct(&distance, &ray->direction);
	C = muVecDotProduct(&distance, &distance);
	C -= sphere->sqr_radius;
	const f32 D = B * B - C;

	if (D > 0.0f) {
		const f32 dist = -B - muSqrtf(D);
		if (dist < 0.0f) return;

		hitinfo_t info;
		info.material = sphere->material;
		info.distance = dist;
		info.hit = TRUE;
		
		//position = ray.origin + (ray.direction * distance)
		muVecScale(&ray->direction, &info.position, dist);
		muVecAdd(&ray->origin, &info.position, &info.position);

		//normal = (hitpos - pos) * rpc_radius
		muVecSub(&info.position, &sphere->position, &info.normal);
		muVecScale(&info.normal, &info.normal, sphere->rcp_radius);
		muVecNormalize(&info.normal);

		callback(&info, current);
	}
}