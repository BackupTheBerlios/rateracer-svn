#pragma once

#include "Shape.h"

class Disc : public Shape
{
public:
	Disc(Vec3 Centre, float InnerRadius, float OuterRadius, Vec3 Normal)
	{
		centre = Centre;
		innerRadiusSq = InnerRadius * InnerRadius;
		outerRadiusSq = OuterRadius * OuterRadius;
		N = Normal;
		w = dot(N, centre);
	}

	virtual ~Disc() {}

	Vec3 centre;
	float innerRadiusSq;
	float outerRadiusSq;
	Vec3 N;
	float w;

	float collideRay(const Ray& r)
	{
		float divider = dot(r.dir, N);
		if (fabsf(divider) < cEpsilon) return -1;
		float t = (w - dot(r.ori, N)) / divider;
		float d2 = distanceSquared(r.ori + t*r.dir, centre);
		if (d2 < innerRadiusSq) return -1;
		if (d2 > outerRadiusSq) return -1;
		return (w - dot(r.ori, N)) / divider;
	}

	Vec3 getNormal(const Vec3& p)
	{
		//const float r = 0.01f;
		//return (N + Vec3(rnd(-r, r), rnd(-r, r), rnd(-r, r))).normalize();
		return N;
	}

	Vec3 getTangent(const Vec3& p)
	{
		return cross( N, p - centre ).normalizeSafely();
	}

	Vec2 getUV(const Vec3& p)
	{
		Vec3 cp = p - centre;
		//if (fabsf(cp[0]) < 0.5f && fabsf(cp[2]) < 0.5f)
		{
			return Vec2(cp[0] + 0.5f, cp[2] + 0.5f);
		}
	}

	void drawPreview()
	{
		material->setPreviewMaterial();

		glPushMatrix();
		glTranslatef(centre[0], centre[1], centre[2]);

		glMultMatrixf(&createONBasis(N)[0][0]);

		const int slices = 30, loops = 10;
		GLUquadricObj *q = gluNewQuadric();
		gluDisk(q, sqrtf(innerRadiusSq), sqrtf(outerRadiusSq), slices, loops);
		gluDeleteQuadric(q);

		glPopMatrix();
	}
};
