#pragma once

#include "mathlib/Vec3.h"

#include "DebugDraw.h"

class BBox
{
public:
  BBox() {}
  BBox(const Vec3& a, const Vec3& b) { pp[0] = a; pp[1] = b; }

 	bool collideRay(const Ray& r)
  {
    Vec3 invDir = Vec3(1.0f / r.dir[0], 1.0f / r.dir[1], 1.0f / r.dir[2]);

    float tmin = cEpsilon;
    float tmax = FLT_MAX;

    float t0, t1;
    for (int a = 0; a < 3; a++) {
      if (invDir[a] < 0) {
        t0 = (pp[1][a] - r.ori[a]) * invDir[a];
        t1 = (pp[0][a] - r.ori[a]) * invDir[a];
      }
      else {
        t0 = (pp[0][a] - r.ori[a]) * invDir[a];
        t1 = (pp[1][a] - r.ori[a]) * invDir[a];
      }
      if (t0 > tmin) tmin = t0;
      if (t1 < tmax) tmax = t1;
      if (tmin > tmax) return false;
    }
    return true;
  }

  Vec3 pp[2];
};

class BVH : public Shape
{
public:
  BVH() {}
  BVH(Shape* s1, Shape *s2)
  {
    left = s1; right = s2;

    Vec3 min, max;
		Vec3 tmpMin, tmpMax;

		left->calcBounds(min, max);
		right->calcBounds(tmpMin, tmpMax);

		if (tmpMin[0] < min[0]) min[0] = tmpMin[0];
		if (tmpMin[1] < min[1]) min[1] = tmpMin[1];
		if (tmpMin[2] < min[2]) min[2] = tmpMin[2];

		if (tmpMax[0] > max[0]) max[0] = tmpMax[0];
		if (tmpMax[1] > max[1]) max[1] = tmpMax[1];
		if (tmpMax[2] > max[2]) max[2] = tmpMax[2];

    bb.pp[0] = min;
    bb.pp[1] = max;
  }

  static Shape* buildBVH(Shape **shapes, int numShapes, int axis)
  {
    //printf("buildBVH( %X (%d) axis %d\n", shapes, numShapes, axis);

    if (numShapes == 1) return shapes[0];
    if (numShapes == 2) return new BVH(shapes[0], shapes[1]);

    // FIXME per axis only
    Vec3 min, max;
		Vec3 tmpMin, tmpMax;
		shapes[0]->calcBounds(min, max);
    for (int n = 0; n < numShapes; n++)
    {
			shapes[n]->calcBounds(tmpMin, tmpMax);

			if (tmpMin[0] < min[0]) min[0] = tmpMin[0];
			if (tmpMin[1] < min[1]) min[1] = tmpMin[1];
			if (tmpMin[2] < min[2]) min[2] = tmpMin[2];

			if (tmpMax[0] > max[0]) max[0] = tmpMax[0];
			if (tmpMax[1] > max[1]) max[1] = tmpMax[1];
			if (tmpMax[2] > max[2]) max[2] = tmpMax[2];
    }
    float pivot = (min[axis] + max[axis]) * 0.5f;

    int midIndex = qsplit(shapes, numShapes, pivot, axis);
    //int midIndex = numShapes / 2;

    return new BVH(
      buildBVH(&shapes[0], midIndex, (axis + 1) % 3),
      buildBVH(&shapes[midIndex], numShapes - midIndex, (axis + 1) % 3));
  }

  static int qsplit(Shape **shapes, int numShapes, float pivot, int axis)
  {
    int midIndex = 0;

    Vec3 min, max;
    float centroid;

    for (int n = 0; n < numShapes; n++)
    {
  		shapes[n]->calcBounds(min, max);
      centroid = (min[axis] + max[axis]) * 0.5f;
      if (centroid < pivot && n > midIndex)
      {
        swap( (int&)shapes[n], (int&)shapes[midIndex] );
        midIndex++;
      }
    }

    if (midIndex == 0 || midIndex == numShapes) {
      midIndex = numShapes / 2;
    }
    return midIndex;
  }

	void calcBounds(Vec3& min, Vec3&max)
	{
    min = bb.pp[0];
    max = bb.pp[1];
	}

	float collideRay(const Ray& r)
	{
		float t, tHit = FLT_MAX;
    if (!bb.collideRay(r)) return tHit;

		t = left->collideRay(r);
		if (t > cEpsilon && t < tHit) {
			tHit = t;
			lastHit = left;
		}

		t = right->collideRay(r);
		if (t > cEpsilon && t < tHit) {
			tHit = t;
			lastHit = right;
		}
		return tHit;
	}

	Shape* getPrimitiveObject()
	{
		return lastHit; 
	}

	Vec3 getNormal(const Vec3& p)
	{
		return lastHit->getNormal(p);
	}

	Vec3 getPrimitiveNormal(const Vec3& p)
	{
		return lastHit->getPrimitiveNormal(p);
	}

	Vec3 getTangent(const Vec3& p)
	{
		return lastHit->getTangent(p);
	}

	void drawBoundingBoxes()
	{
		drawAAWireBox(bb.pp[0], bb.pp[1]);
    left->drawBoundingBoxes();
    right->drawBoundingBoxes();
	}

	void drawPreview()
	{
    left->drawPreview();
    right->drawPreview();
	}

	Shape *lastHit;

  Shape *left;
  Shape *right;
  BBox bb;
};