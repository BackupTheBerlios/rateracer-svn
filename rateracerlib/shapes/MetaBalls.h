#pragma once

#include "Shape.h"

struct Ball
{
	Ball(Vec3 Centre, float RadiusSq)
	{
		centre = Centre;
		radiusSq = RadiusSq;
	}

	Vec3 centre;
	float radiusSq;
};

class MetaBalls : public Shape
{
public:
	MetaBalls() {}
	virtual ~MetaBalls() {}

	void addBall(Vec3 Centre, float Radius)
	{
		balls.push_back( Ball(Centre, Radius*Radius) );
	}

	std::vector<Ball> balls;

	/*
	// "Soft Object" density function
	float densityFunc( float rmin, float rmax, float r )
	{
		if( r <= rmin ) return 1;
		if( r >= rmax ) return 0;
		
		// "Soft Object" density function
		float d  = rmax - rmin;
		float t = (r-rmin) * (r-rmin) / (d * d);
		
		return 1.f + t * (-22.f/9.f + t * (17.f/9.f + t * (-4.f/9.f)));
	}
	*/

	// "Soft Object" density function
	float fieldFunction(float r, float R)
	{
		if (r > R) return 0;
		float rr = r / R;
		float rr2 = rr * rr;
		return 1 + rr2 * (-22.0f/9.0f + rr2 * (17.0f/9.0f + rr2 * (-4.0f/9.0f)));
	}

	// Same, but taking squared arguments...
	float fieldFunction2(float r2, float R2)
	{
		if (r2 > R2) return 0;
		float rr2 = r2 / R2;
		return 1 + rr2 * (-22.0f/9.0f + rr2 * (17.0f/9.0f + rr2 * (-4.0f/9.0f)));
	}

	/*
	float fieldFunction2(float r2, float R2)
	{
		return R2 / (R2 + r2);
	}
	*/

	// Return negative for inside, positive for outside
	float potentialField(const Vec3& p, float& minDistSq)
	{
		minDistSq = 1000;

		Vec3 d;
		float distSq;
		float f = 0;
		for (int i = 0; i < (int)balls.size(); i++)
		{
			d = balls[i].centre - p;
			distSq = dot(d,d);
			if (distSq < minDistSq) minDistSq = distSq;
			f += fieldFunction2(distSq, balls[i].radiusSq);
		}
		return 0.5f - f;
	}

	float collideRay(const Ray& r)
	{
		Vec3 p = r.ori;

		float minDistSq;
		float f = potentialField(p, minDistSq);

		// minDistSq is distance to closest ball (field src) center
		// It would be better to use distance from outer influence radius of 
		// closest ball (field src). Then we could take one large tStep right
		// up to the closest ball influence (taking note that sum of fields can
		// exceed influence radius if balls can get arbitrarily close - use an upper
		// bound?) Also, balls could be rasterized into grid using influence radius,
		// for efficiency potentialField() should then only consider balls in
		// current voxel during hit testing...

		float t = 0, lastT = 0, tStep;

		while (f > 0.0f)
		{
			if (t > 20.0f) return -1;

      tStep = max2( cEpsilon, 0.5f * f * sqrtf(minDistSq) ); // AD HOC!
			//tStep = cEpsilon; // Slow but harmless...

			lastT = t;
			t += tStep;

			p = r.ori + t * r.dir;
			f = potentialField(p, minDistSq);

			//printf("f = %f, tStep = %f, t = %f\n", f, tStep, t);
		}
		return 0.5f*(lastT + t); // Urgh... Should find exact intersection point
	}

	Vec3 getNormal(const Vec3& p)
	{
		Vec3 N;
		float minDistSq;
		// Calc gradient of pot. field...
		float f0 = potentialField(p, minDistSq);
		const float cGradStep = 0.000001f; // Experiment with this for 
		N[0] = potentialField(p+Vec3(cGradStep,0,0), minDistSq) - f0;
		N[1] = potentialField(p+Vec3(0,cGradStep,0), minDistSq) - f0;
		N[2] = potentialField(p+Vec3(0,0,cGradStep), minDistSq) - f0;
		N.normalizeSafely();
		return N;
	}

	/*
	Vec3 getTangent(const Vec3& p)
	{
		Vec3 poleVec(0,0,1); // TODO: fix?
		return cross( getNormal(p), poleVec ).normalizeSafely();
	}
	*/

	/*
	void rasterize(Grid *grid)
	{
		for (int i = 0; i < (int)balls.size(); i++)
		{
			//Sphere *sph = new Sphere(balls[i].centre, balls[i].radiusSq);
			//sph->rasterize(grid);
		}
	}*/

	void drawPreview()
	{
		material->setPreviewMaterial();

		for (int i = 0; i < (int)balls.size(); i++)
		{
			glPushMatrix();
			glTranslatef(balls[i].centre[0], balls[i].centre[1], balls[i].centre[2]);

			const int slices = 10, stacks = 5;
			GLUquadricObj *q = gluNewQuadric();
			gluSphere(q, sqrtf(balls[i].radiusSq), slices, stacks);
			gluDeleteQuadric(q);

			glPopMatrix();
		}
	}
};
