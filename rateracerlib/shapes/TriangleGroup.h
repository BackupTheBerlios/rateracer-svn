#pragma once

#include "Shape.h"
#include "Triangle.h"

#include "DebugDraw.h"

#include "chunks/chunkMesh.h"

class TriangleGroup : public Shape
{
public:
	TriangleGroup(MeshGroup* pGroup, Material* mat)
	{
  }

  virtual ~TriangleGroup()
  {
  }

	Triangle *lastHit;

	int getNumTris()
	{
	  return (int)mTriangles.size();
	}

	void calcBounds(Vec3& min, Vec3&max)
	{
		Vec3 tmpMin, tmpMax;

		if (mTriangles.size() == 0) {
			min.assign( FLT_MAX, FLT_MAX, FLT_MAX);
			max.assign(-FLT_MAX,-FLT_MAX,-FLT_MAX);
			return;
		}

		mTriangles[0]->calcBounds(min, max);
		for (int i = 1; i < (int)mTriangles.size(); i++)
		{
			mTriangles[i]->calcBounds(tmpMin, tmpMax);

			if (tmpMin[0] < min[0]) min[0] = tmpMin[0];
			if (tmpMin[1] < min[1]) min[1] = tmpMin[1];
			if (tmpMin[2] < min[2]) min[2] = tmpMin[2];

			if (tmpMax[0] > max[0]) max[0] = tmpMax[0];
			if (tmpMax[1] > max[1]) max[1] = tmpMax[1];
			if (tmpMax[2] > max[2]) max[2] = tmpMax[2];
		}
	}

	float collideRay(const Ray& r)
	{
		float t, tHit = FLT_MAX;
		Triangle *tri;
		for (int i = 0; i < (int)mTriangles.size(); i++) {
			tri = mTriangles[i];
			t = tri->collideRay(r);
			if (t > cEpsilon && t < tHit) {
				tHit = t;
				lastHit = tri;
			}
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
		material->setPreviewMaterial();

		//for (int n = 0; n < (int)mTriangles.size(); n++) {
		//	mTriangles[n]->drawBoundingBoxes();
		//}

		Vec3 min, max;
		calcBounds(min, max);
		drawAAWireBox(max - min, min);
	}

	void drawPreview()
	{
    material->setPreviewMaterial();
		for (int n = 0; n < (int)mTriangles.size(); n++) {
			mTriangles[n]->drawPreview();
		}
	}

  //Material *mMaterial;
  std::vector<Triangle*> mTriangles;
};
