#pragma once

#include <string>
#include <map>

#include "Shape.h"
#include "TriangleGroup.h"
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
  // (FIXME make materialMap static member of Model?)
	TriMesh(ChunkMesh *pMesh, const char* name, MaterialMap& materialMap)
	{
		lastHit = NULL;

    mMeshName = name;

		printf("\n* Mesh chunk: '%s'\n", name);
		printf("numVertices: %d\n", pMesh->numVertices);
		printf("numNormals: %d\n", pMesh->numNormals);
		printf("numUVs: %d\n", pMesh->numUVs);
		printf("numMeshGroups: %d\n", pMesh->numMeshGroups);

    // FIXME: Reuse (point into) existing arrays in model!

		int n;

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
			mUVs[n][1] = pMesh->uvs[2*n+1];
		}

		int vi[3], ni[3], uvi[3];
		Triangle *tri;

    for (int g = 0; g < pMesh->numMeshGroups; g++)
		{
			MeshGroup *pGroup = pMesh->meshGroups[g];

      Material *mat = materialMap[pGroup->materialName];
      if (mat == NULL)
      {
        mat = new Material();
        materialMap[pGroup->materialName] = mat;
      }

      TriangleGroup *triGroup = new TriangleGroup(pGroup, mat);

		  printf("  Mesh group: material = '%s' numTriangles = %d\n",
		    pGroup->materialName, pGroup->numTriangles);

      triGroup->material = mat;

  		triGroup->mTriangles.resize(pGroup->numTriangles);
		  for (int f = 0; f < pGroup->numTriangles; f++)
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

			  tri = new Triangle();

        tri->setVertices(&mVertices[vi[0]], &mVertices[vi[1]], &mVertices[vi[2]]);
			  tri->setNormals(&mNormals[ni[0]], &mNormals[ni[1]], &mNormals[ni[2]]);
			  if (pMesh->numUVs == 0) {
				  tri->setUVs(&gDefaultUV, &gDefaultUV, &gDefaultUV);
			  } else {
				  tri->setUVs(&mUVs[uvi[0]], &mUVs[uvi[1]], &mUVs[uvi[2]]);
			  }

			  tri->material = mat;

			  //triGroup->mTriangles.push_back(tri);
        triGroup->mTriangles[f] = tri;
		  }
      mTriangleGroups.push_back(triGroup);
		}
	}

	virtual ~TriMesh()
	{
		for (int g = 0; g < (int)mTriangleGroups.size(); g++)
    {
      TriangleGroup *triGroup = mTriangleGroups[g];

		  for (int n = 0; n < (int)triGroup->mTriangles.size(); n++) {
			  delete triGroup->mTriangles[n];
		  }
      delete triGroup;
    }
  }

  void addToShapeList(std::vector<Shape*>& shapes)
  {
		for (int n = 0; n < (int)mTriangleGroups.size(); n++) {
			mTriangleGroups[n]->addToShapeList(shapes);
		}
  }

  std::string mMeshName;
	std::vector<Vec3> mVertices;
	std::vector<Vec3> mNormals;
	std::vector<Vec2> mUVs;

 	std::vector<TriangleGroup*> mTriangleGroups;

	TriangleGroup *lastHit;

	int getNumTris()
	{
    int numTris = 0;
		for (int g = 0; g < (int)mTriangleGroups.size(); g++) {
		  numTris += (int)mTriangleGroups[g]->mTriangles.size();
    }
    return numTris;
	}

	void calcBounds(Vec3& min, Vec3&max)
	{
		Vec3 tmpMin, tmpMax;

		if (mTriangleGroups.size() == 0) {
			min.assign( FLT_MAX, FLT_MAX, FLT_MAX);
			max.assign(-FLT_MAX,-FLT_MAX,-FLT_MAX);
			return;
		}

		mTriangleGroups[0]->calcBounds(min, max);
		for (int i = 1; i < (int)mTriangleGroups.size(); i++)
		{
			mTriangleGroups[i]->calcBounds(tmpMin, tmpMax);

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
		TriangleGroup *triGroup;
		for (int i = 0; i < (int)mTriangleGroups.size(); i++) {
			triGroup = mTriangleGroups[i];
			t = triGroup->collideRay(r);
			if (t > cEpsilon && t < tHit) {
				tHit = t;
				lastHit = triGroup;
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
		for (int n = 0; n < (int)mTriangleGroups.size(); n++) {
			mTriangleGroups[n]->rasterize(grid);
		}
	}

	void drawBoundingBoxes()
	{
		for (int n = 0; n < (int)mTriangleGroups.size(); n++) {
			mTriangleGroups[n]->drawBoundingBoxes();
		}

		material->setPreviewMaterial();
		Vec3 min, max;
		calcBounds(min, max);
		drawAAWireBox(max - min, min);
	}

	void drawPreview()
	{
		for (int g = 0; g < (int)mTriangleGroups.size(); g++)
    {
      mTriangleGroups[g]->drawPreview();
    }
	}
};
