#pragma once

#include <vector>

#include "mathlib/Vec3.h"

#include "Ray.h"

class Shape;
typedef std::vector<Shape*> GridElement;

class Grid
{
public:
	//Grid(int Subdivs, float Boundsize, Vec3 Centre);
	Grid(int Subdivs, Vec3 Min, Vec3 Max);
	virtual ~Grid();

	inline GridElement& voxel(int x, int y, int z)
	{
		return voxels[x + y * iStep[Y] + z * iStep[Z]];
	}

	inline Vec3 voxelOrigin(int x, int y, int z)
	{
		return min + Vec3(cellsize[X] * float(x), cellsize[Y] * float(y), cellsize[Z] * float(z));
	}

	inline Vec3 voxelCenter(int x, int y, int z)
	{
		return min + Vec3(cellsize[X] * (float(x)+0.5f), cellsize[Y] * (float(y)+0.5f), cellsize[Z] * (float(z)+0.5f));
	}

	Shape* findHitInList(Ray& ray0, std::vector<Shape*>& objList,
											  Shape *excludeShape = NULL);
	Shape* findHitInGrid(Ray& ray0, Shape *excludeShape = NULL);

	void printGridStatistics();

	void dbgBeginStoringVoxels();
	void dbgEndStoringVoxels();
	void dbgDrawStoredVoxels();
	void dbgDrawGrid();

	int subdivs[3];
	int iStep[3];
	Vec3 boundsize;
	Vec3 min, max;

	Vec3 cellsize;
	GridElement *voxels;

	bool mStoreVoxels;
	struct Voxel { int x, y, z;	};
	std::vector<Voxel> mStoredVoxels;
};
