#pragma once

#include <string>
#include <map>

#include "Shape.h"
#include "Triangle.h"

#include "DebugDraw.h"

#include "chunks/chunkMesh.h"

extern Vec3 gDefaultNormal;
extern Vec2 gDefaultUV;

// FIXME
typedef std::map< std::string, Material* > MaterialMap;

class TriMesh : public Shape
{
public:
	TriMesh(ChunkMesh *pMesh, const char* name, MaterialMap& materialMap)
	{
		lastHit = NULL;

    mMeshName = name;

		printf("\n* Mesh chunk: '%s'\n", name);
		printf("numVertices: %d\n", pMesh->numVertices);
		printf("numNormals: %d\n", pMesh->numNormals);
		printf("numUVs: %d\n", pMesh->numUVs);

		int n, g, f;

    // FIXME: Reuse (point into) existing arrays in model!

		mVertices.resize(pMesh->numVertices);
		for (n = 0; n < pMesh->numVertices; n++) {
      mVertices[n][0] = pMesh->vertices[3*n+0];
      mVertices[n][1] = pMesh->vertices[3*n+1];
      mVertices[n][2] = pMesh->vertices[3*n+2];
		}

		mNormals.resize(pMesh->numNormals);
		for (n = 0; n < pMesh->numNormals; n++) {
			mNormals[n][0] = pMesh->normals[3*n+0];
			mNormals[n][1] = pMesh->normals[3*n+1];
			mNormals[n][2] = pMesh->normals[3*n+2];
		}

		mUVs.resize(pMesh->numUVs);
		for (n = 0; n < pMesh->numUVs; n++) {
			mUVs[n][0] = pMesh->uvs[2*n+0];
			mUVs[n][0] = pMesh->uvs[2*n+1];
		}

		int vi[3], ni[3], uvi[3];
		Triangle *tri;

		printf("numMeshGroups: %d\n", pMesh->numMeshGroups);
    for (g = 0; g < pMesh->numMeshGroups; g++)
		{
			MeshGroup *pGroup = pMesh->meshGroups[g];
			printf("  Mesh group: material = '%s' numTriangles = %d\n",
				pGroup->materialName, pGroup->numTriangles);

      Material *mat = materialMap[pGroup->materialName];
      if (mat == NULL)
      {
        mat = new Material();
        materialMap[pGroup->materialName] = mat;
      }
      /* MaterialMap::iterator it = materialMap.find(pGroup->materialName);
      if (it != materialMap.end()) {
        mat = it->second;
      }
      else {
        mat = new Material();
        materialMap[pGroup->materialName] = mat;
      }*/

      // FIXME: Allocate all triangles here!

			for (f = 0; f < pGroup->numTriangles; f++)
			{
				vi[0] = pGroup->vertexIndices[3*f+0];
				vi[1] = pGroup->vertexIndices[3*f+1];
				vi[2] = pGroup->vertexIndices[3*f+2];

				ni[0] = pGroup->normalIndices[3*f+0];
				ni[1] = pGroup->normalIndices[3*f+1];
				ni[2] = pGroup->normalIndices[3*f+2];

				uvi[0] = pGroup->uvIndices[3*f+0];
				uvi[1] = pGroup->uvIndices[3*f+1];
				uvi[2] = pGroup->uvIndices[3*f+2];

				tri = new Triangle(&mVertices[vi[0]], &mVertices[vi[1]], &mVertices[vi[2]]);
				tri->N0 = &mNormals[ni[0]]; tri->N1 = &mNormals[ni[1]]; tri->N2 = &mNormals[ni[2]];

				if (pMesh->numUVs == 0) {
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

  std::string mMeshName;
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
		//material->setPreviewMaterial();
		for (int n = 0; n < (int)mTris.size(); n++) {
      mTris[n]->material->setPreviewMaterial();
			mTris[n]->drawPreview();
		}
	}
};
