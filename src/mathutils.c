#include "mathutils.h"
#include <math.h>
#include <ogc/gu.h>
#include "mtrand.h"

void EulerToQuaternion(guQuaternion* q, const f32 rX, const f32 rY, const f32 rZ) {
	guVector vec;
	vec.x = rX; vec.y = rY; vec.z = rZ;
	guVecScale(&vec, &vec, 0.5f);

	f32 x[2], y[2], z[2];

	sincosf(vec.x, &x[0], &x[1]);
	sincosf(vec.y, &y[0], &y[1]);
	sincosf(vec.z, &z[0], &z[1]);

	ps_eulerQuat(x, y, z, q);
}

void AxisAngleToQuaternion(guQuaternion* q, guVector rAxis, const f32 rAngle) {
	f32 s, c;
	sincosf(rAngle / 2.0f, &s, &c);
	guVector out;
	guVecScale(&rAxis, &out, s);
	q->x = out.x;
	q->y = out.y;
	q->z = out.z;
	q->w = c;
}

void QUAT_lookat(guVector* forward, guVector *up, guQuaternion* out) {
	Mtx tmpMatrix;
	guVector zero = { 0, 0, 0 };
	guLookAt(tmpMatrix, forward, up, &zero);
	c_guQuatMtx(out, tmpMatrix);
}

void QUAT_slerp(guQuaternion* q0, guQuaternion* q1, const float t, guQuaternion* out) {
	if (t <= 0.0f) {
		*out = *q0;
		return;
	} else if (t >= 1.0f) {
		*out = *q1;
		return;
	}

	f32 cosOmega;
	QUAT_dotProduct(q0, q1, &cosOmega);
	guQuaternion q0c = *q0,
				 q1c = *q1;

	if (cosOmega < 0.0f) {
		f32 invert = -1.f;
		QUAT_scale(&q1c, &q1c, &invert);
		cosOmega = -cosOmega;
	}

	f32 k0, k1;
	if (cosOmega > 0.9999f) {
		k0 = 1.0f - t;
		k1 = t;
	} else {
		f32 sinOmega = sqrt(1.0f - cosOmega*cosOmega);
		f32 omega = atan2(sinOmega, cosOmega);
		f32 oneOverSinOmega = 1.0f / sinOmega;

		k0 = sin((1.0f - t) * omega) * oneOverSinOmega;
		k1 = sin(t * omega) * oneOverSinOmega;
	}

	QUAT_scale(&q0c, &q0c, &k0);
	QUAT_scale(&q1c, &q1c, &k1);
	guQuatAdd(&q0c, &q1c, out);
	return;
}

/* This should be in gu.h or something */

inline f32 guVecMag(guVector* vec) {
	return sqrt(guVecDotProduct(vec, vec));
}

inline f32 vecDistance(guVector* point1, guVector* point2) {
	guVector sub;
	guVecSub(point2, point1, &sub);
	return guVecMag(&sub);
}

inline u8 vecMinAxis(guVector* vec) {
	guVector abs;
	ps_vecAbs(vec, &abs);
	return (abs.x <= abs.y ? (abs.x <= abs.z ? 0 : 2) : (abs.y <= abs.z) ? 1 : 2);
}

void vecPerpendicular(guVector* vec, guVector* out) {
	const u8 ma = vecMinAxis(vec);
	if (ma == 0) {
		const guVector val = { 0, vec->z, -vec->y };
		*out = val;
	} else if (ma == 1) {
		const guVector val = { vec->z, 0, -vec->x };
		*out = val;
	} else {
		const guVector val = { vec->y, -vec->x, 0 };
		*out = val;
	}
}

inline guVector Vector(f32 x, f32 y, f32 z) {
	guVector res = { x, y, z };
	return res;
}


guVector RandomVectorInHemisphere(guVector* normal) {
	// Jacco Bikker
	// Altered by Martijn Gerkes
	guVector T, B, N;
	N = *normal;
	vecPerpendicular(normal, &T);
	guVecCross(&T, normal, &B);

	f32 rand[2] = { FncMtRandR32(), FncMtRandR32() };
	f32 mad[2] = { 2.0f, -1.0f };

	ps_randScale(&T, &B, rand, mad);
	guVecAdd(&T, &B, &B);
	guVecSub(&B, &N, &N);
	guVecNormalize(&N);
	guVecScale(&N, &N, -1.0f);

	return N;
}

void guVecMax(guVector* vector, f32 max) {
	vector->x = vector->x > max ? max : vector->x;
	vector->y = vector->y > max ? max : vector->y;
	vector->z = vector->z > max ? max : vector->z;
}

void guVecMin(guVector* vector, f32 min) {
	vector->x = vector->x < min ? min : vector->x;
	vector->y = vector->y < min ? min : vector->y;
	vector->z = vector->z < min ? min : vector->z;
}