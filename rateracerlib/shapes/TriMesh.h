#pragma once

#include "Shape.h"
#include "Triangle.h"

#include "DebugDraw.h"

extern Vec3 gDefaultNormal;
extern Vec2 gDefaultUV;

class TriMesh : public Shape
{
public:
	TriMesh(FILE* infile, Material *mat)
	{
		lastHit = NULL;

		// TODO: split tris in material groups, scrap this incoming material
		material = mat;

		int n, g, f;

		int numVertices;
		fread(&numVertices, sizeof(int), 1, infile);
		mVertices.resize(numVertices);
		for (n = 0; n < numVertices; n++) {
			fread(&mVertices[n], sizeof(Vec3), 1, infile);
		}

		int numNormals;
		fread(&numNormals, sizeof(int), 1, infile);
		mNormals.resize(numNormals);
		for (n = 0; n < numNormals; n++) {
			fread(&mNormals[n], sizeof(Vec3), 1, infile);
		}

		int numUVs;
		fread(&numUVs, sizeof(int), 1, infile);
		mUVs.resize(numUVs);
		for (n = 0; n < numUVs; n++) {
			fread(&mUVs[n], sizeof(Vec2), 1, infile);
		}

		int numGroups;
		int materialID;
		int numFaces;
		int vi[3], ni[3], uvi[3];
		Triangle *tri;
		fread(&numGroups, sizeof(int), 1, infile);
		for (g = 0; g < numGroups; g++)
		{
			fread(&materialID, sizeof(int), 1, infile);
			fread(&numFaces, sizeof(int), 1, infile);
			for (f = 0; f < numFaces; f++)
			{
				// TODO: write cleaner
				fread(&vi[0], sizeof(int), 3, infile);
				fread(&ni[0], sizeof(int), 3, infile);
				fread(&uvi[0], sizeof(int), 3, infile);

				tri = new Triangle(&mVertices[vi[0]], &mVertices[vi[1]], &mVertices[vi[2]]);
				tri->N0 = &mNormals[ni[0]]; tri->N1 = &mNormals[ni[1]]; tri->N2 = &mNormals[ni[2]];

				if (numUVs == 0) {
					tri->uv0 = tri->uv1 = tri->uv2 = &gDefaultUV;
				} else {
					tri->uv0 = &mUVs[uvi[0]]; tri->uv1 = &mUVs[uvi[1]]; tri->uv2 = &mUVs[uvi[2]];
				}

				tri->material = mat;
				mTris.push_back(tri);
			}
		}
	}

	virtual ~TriMesh()
	{
		for (int n = 0; n < (int)mTris.size(); n++) {
			delete mTris[n];
		}
	}

	std::vector<Vec3> mVertices;
	std::vector<Vec3> mNormals;
	std::vector<Vec2> mUVs;

	std::vector<Triangle*> mTris;

	Triangle *lastHit;

	int getNumTris()
	{
		return (int)mTris.size();
	}

	void calcBounds(Vec3& min, Vec3&max)
	{
		Vec3 tmpMin, tmpMax;

		if (mTris.size() == 0) {
			min.assign( FLT_MAX, FLT_MAX, FLT_MAX);
			max.assign(-FLT_MAX,-FLT_MAX,-FLT_MAX);
			return;
		}

		mTris[0]->calcBounds(min, max);
		for (int i = 1; i < (int)mTris.size(); i++)
		{
			mTris[i]->calcBounds(tmpMin, tmpMax);

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
		for (int i = 0; i < (int)mTris.size(); i++) {
			tri = mTris[i];
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

	void rasterize(Grid *grid)
	{
		for (int n = 0; n < (int)mTris.size(); n++) {
			mTris[n]->rasterize(grid);
		}
	}

	void drawBoundingBoxes()
	{
		material->setPreviewMaterial();
		Vec3 min, max;
		calcBounds(min, max);
		drawAAWireBox(max - min, min);
	}

	void drawPreview()
	{
		material->setPreviewMaterial();
		for (int n = 0; n < (int)mTris.size(); n++) {
			mTris[n]->drawPreview();
		}
	}
};
