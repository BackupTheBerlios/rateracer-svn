#pragma once

#include <string>
#include <map>

#include "Shape.h"
#include "Triangle.h"
#include "TriMesh.h"

#include "chunks/ChunksFile.h"
#include "chunks/chunkMesh.h"

class Model : public Shape
{
public:
	Model(const char* modelfile)
	{
		mTotalNumTris = 0;
		lastHit = NULL;

		Serializer serializer;
		//serializer.mVerbosePrint = true;

		printf("-------------------------------------------------------------------------------\n");

		if ( !serializer.open(Serializer::SERIALIZE_READ, modelfile) )
		{
			printf("chunksExport::importMeshes() : Could not open input file '%s'\n", modelfile);
			return;
		}

		printf("Importing meshes from file '%s'\n\n", modelfile);

		ChunksFile chunksFile;
		chunksFile.serialize( &serializer );

		printf("formatstring: %s\n", chunksFile.formatstring);
		printf("numChunks: %d\n", chunksFile.numChunks);

		mTotalNumTris = 0;
		for (int n = 0; n < chunksFile.numChunks; n++)
		{
			ChunkHeader *chunkHeader = (ChunkHeader *)chunksFile.chunkHeaders[n];

			switch (chunkHeader->chunkType)
			{
				case CHUNKTYPE_MESH :
				{
					ChunkMesh *pMesh = (ChunkMesh *)chunkHeader->chunkData;

					TriMesh *mesh = new TriMesh(pMesh, chunkHeader->chunkName, mMaterialMap);
					mMeshes.push_back(mesh);

					break;
				}

				default :
				{
					printf("\n* Unknown chunk '%s', type = %d\n",
						chunkHeader->chunkName, chunkHeader->chunkType);
					break;
				}
			}
		}

		serializer.close();

		printf("-------------------------------------------------------------------------------\n");

		mTotalNumTris = 0;
		for (int n = 0; n < (int)mMeshes.size(); n++) {
			mTotalNumTris += mMeshes[n]->getNumTris();
		}
		printf("Total nr of triangles: %d\n", mTotalNumTris);
	}

	virtual ~Model()
	{
		for (int n = 0; n < (int)mMeshes.size(); n++) {
			delete mMeshes[n];
		}

    MaterialMap::iterator mat;
    for (mat = mMaterialMap.begin(); mat != mMaterialMap.end(); mat++) {
      delete mat->second;
    }
	}

	int mTotalNumTris;

  typedef std::map< std::string, Material* > MaterialMap;
  MaterialMap mMaterialMap;

	std::vector<TriMesh*> mMeshes;

	TriMesh *lastHit;

	void calcBounds(Vec3& min, Vec3&max)
	{
		Vec3 tmpMin, tmpMax;

		if (mMeshes.size() == 0) {
			min.assign( FLT_MAX, FLT_MAX, FLT_MAX);
			max.assign(-FLT_MAX,-FLT_MAX,-FLT_MAX);
			return;
		}

		mMeshes[0]->calcBounds(min, max);
		for (int i = 1; i < (int)mMeshes.size(); i++)
		{
			mMeshes[i]->calcBounds(tmpMin, tmpMax);

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
		TriMesh *mesh;
		for (int i = 0; i < (int)mMeshes.size(); i++) {
			mesh = mMeshes[i];
			t = mesh->collideRay(r);
			if (t > cEpsilon && t < tHit) {
				tHit = t;
				lastHit = mesh;
			}
		}
		return tHit;
	}

	Shape* getPrimitiveObject()
	{
		return lastHit->lastHit; 
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

  /*void rasterize(Grid *grid)
	{
		for (int n = 0; n < (int)mMeshes.size(); n++) {
			mMeshes[n]->rasterize(grid);
		}
	}*/

	void drawBoundingBoxes()
	{
		for (int n = 0; n < (int)mMeshes.size(); n++) {
			mMeshes[n]->drawBoundingBoxes();
		}

		material->setPreviewMaterial();
		Vec3 min, max;
		calcBounds(min, max);
		drawAAWireBox(max - min, min);
	}

	void drawPreview()
	{
		if (mTotalNumTris > 500000) {
			for (int n = 0; n < (int)mMeshes.size(); n++) {
				mMeshes[n]->drawBoundingBoxes();
			}
		}
		else {
			for (int n = 0; n < (int)mMeshes.size(); n++) {
				mMeshes[n]->drawPreview();
			}
		}
	}
};
