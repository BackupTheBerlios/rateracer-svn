#pragma once

#include "Shape.h"

#include "DebugDraw.h"

#include "Grid.h"
#include "mathlib/triboxtest.h"

class Triangle : public Shape
{
public:
	Triangle() {}

	Triangle(Vec3 *V0, Vec3 *V1, Vec3 *V2)
	{
    setVertices(V0, V1, V2);
	}

	virtual ~Triangle() {}

  void setVertices(Vec3 *V0, Vec3 *V1, Vec3 *V2)
  {
		v0 = V0; v1 = V1; v2 = V2;
		Vec3 e1 = *v1 - *v0;
		Vec3 e2 = *v2 - *v0;
		faceNormal = cross(e1,e2).normalize();
  }

  void setNormals(Vec3 *N0, Vec3 *N1, Vec3 *N2)
  {
		n0 = N0; n1 = N1; n2 = N2;
  }

  void setUVs(Vec2 *UV0, Vec2 *UV1, Vec2 *UV2)
  {
		uv0 = UV0; uv1 = UV1; uv2 = UV2;
  }

	Vec3 *v0, *v1, *v2;
	Vec3 *n0, *n1, *n2;
	Vec2 *uv0, *uv1, *uv2;

	Vec3 faceNormal;

	float u, v;

	void calcBounds(Vec3& min, Vec3&max)
	{
		min[0] = min3( (*v0)[0], (*v1)[0], (*v2)[0] );
		min[1] = min3( (*v0)[1], (*v1)[1], (*v2)[1] );
		min[2] = min3( (*v0)[2], (*v1)[2], (*v2)[2] );

		max[0] = max3( (*v0)[0], (*v1)[0], (*v2)[0] );
		max[1] = max3( (*v0)[1], (*v1)[1], (*v2)[1] );
		max[2] = max3( (*v0)[2], (*v1)[2], (*v2)[2] );
	}

	float collideRay(const Ray& r)
	{
		const float cTriEps = 0.000001f;

		// TODO: per-material back-face culling!
		//if (dot(N, r.dir) > 0) return -1;

		// Möller-Haines method

		Vec3 pVec, sVec, qVec;

		Vec3 edge1 = *v1 - *v0;
		Vec3 edge2 = *v2 - *v0;

		pVec = cross(r.dir, edge2);
		float det = dot(edge1, pVec);

		sVec = r.ori - *v0;

		if (det > cTriEps)
		{
			u = dot(sVec, pVec);
			if (u < 0.0f || u > det) return -1;

			qVec = cross(sVec, edge1);

			v = dot(r.dir, qVec);
			if (v < 0.0f || u + v > det) return -1;
		}
		else if (det < cTriEps)
		{
			//if (cullBackFaces) return -1;

			u = dot(sVec, pVec);
			if (u > 0.0f || u < det) return -1;

			qVec = cross(sVec, edge1);

			v = dot(r.dir, qVec);
			if (v > 0.0f || u + v < det) return -1;
		}
		else return -1; // Ray is parallel to triangle's plane

		float invDet = 1.0f / det;
		u *= invDet;
		v *= invDet;
		return invDet * dot(edge2, qVec);
	}

	Vec3 getNormal(const Vec3& p)
	{
		return ((1-u-v) * (*n0) + u * (*n1) + v * (*n2)).normalize();
	}

	Vec3 getPrimitiveNormal(const Vec3& p)
	{
		return faceNormal;
	}

	/*
	Vec3 getTangent(const Vec3& p)
	{
		// TODO: make something better here?
		return cross( getNormal(p), p ).normalizeSafely();
	}
	*/

	virtual Vec2 getUV(const Vec3& p)
	{
		if (material->mTexture != NULL)
      return ((1-u-v) * (*uv0) + u * (*uv1) + v * (*uv2));
		else
			return Vec2(0,0);
	}

	void rasterize(Grid *grid)
	{
		Vec3 min, max;
		calcBounds(min, max);

		int x0 = int((min[0] - grid->min[0]) / grid->cellsize[X]);
		int y0 = int((min[1] - grid->min[1]) / grid->cellsize[Y]);
		int z0 = int((min[2] - grid->min[2]) / grid->cellsize[Z]);

		int x1 = int((max[0] - grid->min[0]) / grid->cellsize[X]) + 1;
		int y1 = int((max[1] - grid->min[1]) / grid->cellsize[Y]) + 1;
		int z1 = int((max[2] - grid->min[2]) / grid->cellsize[Z]) + 1;

		if (x0 < 0) x0 = 0;
		if (y0 < 0) y0 = 0;
		if (z0 < 0) z0 = 0;

		if (x1 > grid->subdivs[X]) x1 = grid->subdivs[X];
		if (y1 > grid->subdivs[Y]) y1 = grid->subdivs[Y];
		if (z1 > grid->subdivs[Z]) z1 = grid->subdivs[Z];

		extern int triBoxOverlap(Vec3 boxcenter, Vec3 boxhalfsize, Vec3 triV0, Vec3 triV1, Vec3 triV2);

		for (int x = x0; x < x1; x++) {
			for (int y = y0; y < y1; y++) {
				for (int z = z0; z < z1; z++) {
					// TODO: use Triangle/Box Overlap test
					if (triBoxOverlap(grid->voxelCenter(x,y,z),
														0.5f * grid->cellsize,
														*v0, *v1, *v2))
					{
						grid->voxel(x,y,z).push_back(this);
					}
				}
			}
		}
	}

	void drawBoundingBoxes()
	{
    return;
		//material->setPreviewMaterial();
		Vec3 min, max;
		calcBounds(min, max);
		drawAAWireBox(max - min, min);
	}

	void drawPreview()
	{
		material->setPreviewMaterial();

		//glPushMatrix();
		glNormal3fv(&faceNormal[0]);
		glBegin(GL_TRIANGLES);
			glVertex3fv(&(*v0)[0]);
			glVertex3fv(&(*v1)[0]);
			glVertex3fv(&(*v2)[0]);
		glEnd();
    //glPopMatrix();
	}
};
