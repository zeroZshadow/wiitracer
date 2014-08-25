#include "plane.h"
#include <math.h>

inline void PLANE_init(plane_t* plane, guVector position, guVector normal, material_t material) {
	plane->position = position;
	plane->normal = normal;
	guVecNormalize(&plane->normal);
	plane->material = material;
}

BOOL PLANE_raycast(plane_t* plane, ray_t* ray, hitinfo_t* current, hitcallback(callback)) {

	guVector delta;
	guVecSub(&plane->position, &ray->origin, &delta);

	const f32 dist = guVecDotProduct(&plane->normal, &delta) / guVecDotProduct(&plane->normal, &ray->direction);
	if (dist > 0) {
		hitinfo_t info;
		info.material = plane->material;
		info.distance = dist;
		info.hit = TRUE;

		//position = ray.origin + (ray.direction * distance)
		guVecScale(&ray->direction, &info.position, dist);
		guVecAdd(&ray->origin, &info.position, &info.position);

		//normal = plane.normale
		info.normal = plane->normal;

		callback(info, current);

		return TRUE;
	}

	return FALSE;
}