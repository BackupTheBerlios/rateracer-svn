#pragma once

#include "Shape.h"

#define CYL_AXIS 1

class Cylinder : public Shape
{
public:
	Cylinder(Vec3 Base, float Height, float Radius,
					 float topHoleRadius = 0.0f, float botHoleRadius = 0.0f)
	{
		base = Base;
		height = Height;
		radiusSq = Radius * Radius;
		axis = Vec3(0,1,0);

		top    = new Disc(base + height*axis, topHoleRadius, Radius, axis);
		bottom = new Disc(base, botHoleRadius, Radius, -axis);
	}

	virtual ~Cylinder()
	{
		delete top;
		delete bottom;
	}

	Vec3 base;
	float     radiusSq;
	float     height;
	Vec3 axis;

	Disc *bottom, *top;

	float collideRay(const Ray& r)
	{
		float y = dot(r.dir, axis);
		Vec3 X = r.dir - y*axis;

		float x2 = dot(X,X);
		if (fabsf(x2) < cEpsilon) return -1;

		float xlen = sqrtf(x2);
		Vec3 projDir = X / xlen;

		Vec3 l = base - r.ori;
		float s = dot(l, projDir);
		Vec3 projl = l - dot(l,axis)*axis;
		float projl2 = dot(projl, projl);
		if (s < 0 && projl2 > radiusSq) return -1;

		float m2 = projl2 - s*s;
		if (m2 > radiusSq) return -1;

		float q = sqrtf(radiusSq - m2);

		float t;
		if (projl2 > radiusSq)
			t = (s - q) / xlen;
		else
			t = (s + q) / xlen;

		Vec3 p = r.ori + t*r.dir;

		float tTop = top->collideRay(r);
		if (tTop > cEpsilon && tTop < t) t = tTop;
		if (p[CYL_AXIS] > base[CYL_AXIS] + height) t = tTop;

		float tBot = bottom->collideRay(r);
		if (tBot > cEpsilon && tBot < t) t = tBot;
		if (p[CYL_AXIS] < base[CYL_AXIS]) t = tBot;

		return t;
	}

	Vec3 getNormal(const Vec3& p)
	{
		if (p[CYL_AXIS] > base[CYL_AXIS] + height - cEpsilon) return axis;
		if (p[CYL_AXIS] < base[CYL_AXIS] + cEpsilon) return -axis;
		Vec3 rc = p - base;
		return (rc - dot(rc, axis)*axis).normalizeSafely();
	}

	/*
	Vec3 getTangent(const Vec3& p)
	{
		Vec3 poleVec(0,0,1); // TODO: fix?
		Vec3 rc = p - base;
		return cross( rc - dot(rc, axis)*axis, poleVec ).normalizeSafely();
	}
	*/

	void drawPreview()
	{
		material->setPreviewMaterial();

		top->drawPreview();
		bottom->drawPreview();

		glPushMatrix();
		glTranslatef(base[0], base[1], base[2]);
		glRotatef(-90,1,0,0);

		const int slices = 30, stacks = 20;
		GLUquadricObj *q = gluNewQuadric();
		gluCylinder(q, sqrtf(radiusSq), sqrtf(radiusSq), height, slices, stacks);
		gluDeleteQuadric(q);

		glPopMatrix();
	}
};
