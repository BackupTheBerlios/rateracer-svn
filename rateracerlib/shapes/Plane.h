#pragma once

#include "Shape.h"

#include "Stratification.h"

class Plane : public Shape
{
public:
	Plane(Vec3 Normal, float Offset)
	{
		N = Normal;
		offset = Offset;
	}

	virtual ~Plane() {}

	Vec3 N;
	float offset;

	float collideRay(const Ray& r)
	{
		float divider = dot(r.dir, N);
		if (fabsf(divider) < cEpsilon) return -1;
		return (offset - dot(r.ori, N)) / divider;
	}

	Vec3 getNormal(const Vec3& p)
	{
		return N;
	}

	/*
	Vec3 getTangent(const Vec3& p)
	{
		// TODO: make something better here?
		return cross( N, p ).normalizeSafely();
	}
	*/

	Vec2 getUV(const Vec3& p)
	{
		//if (fabsf(p[0]) < 0.5f && fabsf(p[2]) < 0.5f)
		{
			return Vec2(p[0] + 0.5f, p[2] + 0.5f);
		}
	}

	void drawPreview()
	{
		material->setPreviewMaterial();

		glPushMatrix();

		Vec3 T = offset * N;
		glTranslatef(T[0], T[1], T[2]);

		glMultMatrixf(&createONBasis(N)[0][0]);

		glDisable(GL_LIGHTING);

		const float size = 100;
		// TODO: calc transform to orient the plane in space
		/*
		const int slices = 30, loops = 10;
		GLUquadricObj *q = gluNewQuadric();
		gluDisk(q, 0, size, slices, loops);
		gluDeleteQuadric(q);
		*/
		glNormal3fv(&N[0]);
		glBegin(GL_QUADS);
			glVertex3f(-size, -size, 0);
			glVertex3f( size, -size, 0);
			glVertex3f( size,  size, 0);
			glVertex3f(-size,  size, 0);
		glEnd();

		glEnable(GL_LIGHTING);

		glPopMatrix();
	}
};
