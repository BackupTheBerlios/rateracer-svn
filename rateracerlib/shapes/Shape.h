#pragma once

#include <windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#include "mathlib.h"

#include "Ray.h"
#include "Material.h"

class Vec3;
class Grid;

// NOTE: Shape primitives should be convex for optimization assumptions to work!
// Maybe add: kDOP, metaballs

class Shape
{
public:
	Shape()
	{
		material = &gDefaultMaterial;
	}

	virtual ~Shape() {}

	virtual void calcBounds(Vec3& min, Vec3&max)
	{
		min.assign( FLT_MAX, FLT_MAX, FLT_MAX);
		max.assign(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	}

	virtual float collideRay(const Ray& r) = 0;
	virtual Shape* getPrimitiveObject() { return this; }

	virtual Vec3 getNormal(const Vec3& p) = 0;
	virtual Vec3 getPrimitiveNormal(const Vec3& p) { return getNormal(p); }

	virtual Vec3 getTangent(const Vec3& p)
	{
		return cross(getNormal(p), Vec3(0,1,0)).normalizeSafely();
	}

	virtual Vec2 getUV(const Vec3& p) { return Vec2(0,0); }

	virtual void rasterize(Grid *grid) {}

	virtual void drawPreview() = 0;

	Material const *material;
};
