#pragma once

#include <float.h>

#include "mathlib.h"

class Vec3;

//const float cEpsilon = 0.000001f;
const float cEpsilon = 0.001f;

//enum { LAMBDA_RGB, LAMBDA_R, LAMBDA_G, LAMBDA_B};

class Ray
{
public:
	Ray()
	{
		dimRet = 1;
		refractInsideCounter = 0;
		//transmitColor.assign(1,1,1);
		//lambda = LAMBDA_RGB;
	}

	Ray(const Vec3& o, const Vec3& d /*, byte wavelength = LAMBDA_RGB*/)
	{
		ori = o;
		dir = d;

		dimRet = 1;
		refractInsideCounter = 0;
		//transmitColor.assign(1,1,1);
		//lambda = wavelength;
	}

	inline void offset(float amount, const Vec3& direction)
	{
		ori += amount * direction;
	}

	inline Vec3 hitPoint() { return ori + tHit * dir; }
	inline Vec3 pointAtT(float t) { return ori + t * dir; }

	Vec3 ori;
	Vec3 dir;

	float tHit;

	float dimRet; // diminishing return...

	int refractInsideCounter;

	//Vec3 transmitColor;
	//byte lambda;

	// Debug stuff...
	Vec3 color;
};
