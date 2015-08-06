#include "plane.h"
#include "mu.h"
#include <math.h>

void PLANE_init(plane_t* plane, guVector position, guVector normal, material_t material) {
	plane->position = position;
	plane->normal = normal;
	muVecNormalize(&plane->normal);
	plane->material = material;
}

inline void PLANE_raycast(plane_t* plane, ray_t* ray, hitinfo_t* current, hitcallback(callback)) {
	guVector delta;
	muVecSub(&plane->position, &ray->origin, &delta);

	const f32 B = muVecDotProduct(&plane->normal, &ray->direction);
	const f32 C = muVecDotProduct(&plane->normal, &delta);
	const f32 dist = C / B;

	if (dist > 0.0f) {
		hitinfo_t info;
		info.material = plane->material;
		info.distance = dist;
		info.hit = TRUE;

		muVecScale(&ray->direction, &info.position, dist);
		muVecAdd(&ray->origin, &info.position, &info.position);

		info.normal = plane->normal;

		callback(&info, current);
	}
}