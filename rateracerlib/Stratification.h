#pragma once

#include <time.h>

#include "mathlib/Random.h"

inline void setRandomSeed()
{
	nseed( (ULONG)time(NULL) );
}

inline float rnd0()
{
	return nrand() - 0.5f;
}

inline float rnd(float a, float b)
{
	return a + (b - a) * nrand();
}

inline void rndCircle(float& x, float& y)
{
	do {
		x = nrand() - 0.5f;
		y = nrand() - 0.5f;
	} while (x*x + y*y > 0.25f);
}

inline void rndUnitVec(Vec3& vec)
{
	float lenSq;
	do {
		vec[0] = nrand() - 0.5f;
		vec[1] = nrand() - 0.5f;
		vec[2] = nrand() - 0.5f;
		lenSq = dot(vec,vec);
	} while (lenSq < cEpsilon || lenSq > 0.25f);
	vec *= 1.0f / sqrtf(lenSq);
}

inline void rndImplicitCosVec(Vec3& vec)
{
	rndCircle(vec[0], vec[1]);
	vec[0] *= 2; vec[1] *= 2;
	vec[2] = sqrtf(1 - vec[0]*vec[0] - vec[1]*vec[1]);
}

inline void createONBasis(Vec3& U, Vec3& V, const Vec3& N)
{
	if (fabsf(N[0]) > fabsf(N[1])) {
		// x or z is largest component, swap those
		float invLen = 1.0f / sqrtf(N[0]*N[0] + N[2]*N[2]);
		U.assign(-N[2]*invLen, 0, N[0]*invLen);
	}
	else {
		// y or z is largest component, swap those
		float invLen = 1.0f / sqrtf(N[1]*N[1] + N[2]*N[2]);
		U.assign(0, N[2]*invLen, -N[1]*invLen);
	}
	V = cross(N,U);
}

inline Matrix4 createONBasis(const Vec3& N)
{
	Vec3 U, V;
	createONBasis(U, V, N);
	return Matrix4(U,V,N);
}

inline Vec3 compMul(const Vec3& a, const Vec3& b)
{
	return Vec3(a[0] * b[0], a[1] * b[1], a[2] * b[2]);
}

inline Vec3 compDiv(const Vec3& a, const Vec3& b)
{
	return Vec3(a[0] / b[0], a[1] / b[1], a[2] / b[2]);
}

inline float posDot(const Vec3& a, const Vec3& b)
{
	float d = dot(a,b);
	return d > 0 ? d : 0;
}
