#pragma once

#include "Shape.h"

class Sphere : public Shape
{
public:
	Sphere(Vec3 Centre, float Radius)
	{
		centre = Centre;
		radiusSq = Radius * Radius;
		invRadius = 1.0f / Radius;
	}

	virtual ~Sphere() {}

	Vec3 centre;
	float radiusSq;
	float invRadius;

	void calcBounds(Vec3& min, Vec3&max)
	{
		const float r = 1.0f / invRadius;

		min[0] = centre[0] - r;
		min[1] = centre[1] - r;
		min[2] = centre[2] - r;

		max[0] = centre[0] + r;
		max[1] = centre[1] + r;
		max[2] = centre[2] + r;
	}

	float collideRay(const Ray& r)
	{
		// Optimized geometric solution
		Vec3 l = centre - r.ori;
		float d = dot(l, r.dir);
		float l2 = dot(l,l);
		if (d < 0 && l2 > radiusSq) return -1; // Ray is facing away and outside!

		float m2 = l2 - d*d;
		if (m2 > radiusSq) return -1; // Ray misses!

		float q = sqrtf(radiusSq - m2);
		if (l2 > radiusSq) return d - q;
		else               return d + q;
	}

	Vec3 getNormal(const Vec3& p)
	{
		return (p - centre) * invRadius;
	}

	/*
	Vec3 getTangent(const Vec3& p)
	{
		Vec3 poleVec(0,1,0); // TODO: fix?
		return cross( p - centre, poleVec ).normalizeSafely();
	}
	*/

	void rasterize(Grid *grid)
	{
		const float r = 1.0f / invRadius;

		int x0 = int(((centre[0] - r) - grid->min[0]) / grid->cellsize[X]);
		int y0 = int(((centre[1] - r) - grid->min[1]) / grid->cellsize[Y]);
		int z0 = int(((centre[2] - r) - grid->min[2]) / grid->cellsize[Z]);

		int x1 = int(((centre[0] + r) - grid->min[0]) / grid->cellsize[X]) + 1;
		int y1 = int(((centre[1] + r) - grid->min[1]) / grid->cellsize[Y]) + 1;
		int z1 = int(((centre[2] + r) - grid->min[2]) / grid->cellsize[Z]) + 1;

		if (x0 < 0) x0 = 0;
		if (y0 < 0) y0 = 0;
		if (z0 < 0) z0 = 0;

		if (x1 > grid->subdivs[X]) x1 = grid->subdivs[X];
		if (y1 > grid->subdivs[Y]) y1 = grid->subdivs[Y];
		if (z1 > grid->subdivs[Z]) z1 = grid->subdivs[Z];

		float cellDiag = grid->cellsize.length();

		bool s1, s2, s3, s4, s5, s6, s7, s8;

		for (int x = x0; x < x1; x++) {
			for (int y = y0; y < y1; y++) {
				for (int z = z0; z < z1; z++) {
					s1 = fabsf(distance(grid->voxelOrigin(x  ,y  ,z  ), centre) - r) <= cellDiag; 
					s2 = fabsf(distance(grid->voxelOrigin(x  ,y  ,z+1), centre) - r) <= cellDiag; 
					s3 = fabsf(distance(grid->voxelOrigin(x  ,y+1,z  ), centre) - r) <= cellDiag; 
					s4 = fabsf(distance(grid->voxelOrigin(x  ,y+1,z+1), centre) - r) <= cellDiag; 
					s5 = fabsf(distance(grid->voxelOrigin(x+1,y  ,z  ), centre) - r) <= cellDiag; 
					s6 = fabsf(distance(grid->voxelOrigin(x+1,y  ,z+1), centre) - r) <= cellDiag; 
					s7 = fabsf(distance(grid->voxelOrigin(x+1,y+1,z  ), centre) - r) <= cellDiag; 
					s8 = fabsf(distance(grid->voxelOrigin(x+1,y+1,z+1), centre) - r) <= cellDiag; 
					if ( s1 || s2 || s3 || s4 || s5 || s6 || s7 || s8 )
					{
						grid->voxel(x,y,z).push_back(this);
					}
				}
			}
		}
	}

	void drawPreview()
	{
		material->setPreviewMaterial();

		glPushMatrix();
		glTranslatef(centre[0], centre[1], centre[2]);

		const int slices = 10, stacks = 5;
		GLUquadricObj *q = gluNewQuadric();
		gluSphere(q, 1.0f / invRadius, slices, stacks);
		gluDeleteQuadric(q);

		glPopMatrix();
	}
};
