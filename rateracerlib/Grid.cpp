#include "Grid.h"

#include "shapes/Shape.h"

#include "DebugDraw.h"

//Grid::Grid(int Subdivs, float Boundsize, Vec3 Centre)
Grid::Grid(int Subdivs, Vec3 Min, Vec3 Max)
{
	//boundsize = Boundsize;
	//min = Centre - Vec3(0.5f*boundsize, 0.5f*boundsize, 0.5f*boundsize);
	//max = Centre + Vec3(0.5f*boundsize, 0.5f*boundsize, 0.5f*boundsize);

	min = Min; max = Max;

	// TODO: calc subdivs based on cubesize/numdivsonlargestaxis!!!
	subdivs[X] = Subdivs;
	subdivs[Y] = Subdivs;
	subdivs[Z] = Subdivs;

	boundsize[X] = max[X] - min[X];
	boundsize[Y] = max[Y] - min[Y];
	boundsize[Z] = max[Z] - min[Z];

	cellsize[X] = boundsize[X] / subdivs[X];
	cellsize[Y] = boundsize[Y] / subdivs[Y];
	cellsize[Z] = boundsize[Z] / subdivs[Z];

	iStep[X] = 1;
	iStep[Y] = subdivs[X];
	iStep[Z] = subdivs[X] * subdivs[Y];

	voxels = new GridElement[subdivs[X] * subdivs[Y] * subdivs[Z]];

	mStoreVoxels = false;
}

Grid::~Grid()
{
	delete [] voxels;
}

Shape* Grid::findHitInList(Ray& ray0, std::vector<Shape*>& objList,
														Shape *excludeShape)
{
	ray0.tHit = FLT_MAX;

	Shape* hitShape = NULL;

	Shape *o;
	float t;
	for (int i = 0; i < (int)objList.size(); i++) {
		o = objList[i];
		if (o == excludeShape) continue;
		t = o->collideRay(ray0);
		if (t > cEpsilon && t < ray0.tHit) {
			ray0.tHit = t;
			hitShape = o;
		}
	}
	return hitShape;
}

Shape* Grid::findHitInGrid(Ray& ray0, Shape *excludeShape)
{
	const float cGridEps = 0.001f;

	ray0.tHit = FLT_MAX;

	int n;

	/////////////////////////////////////////////////////////////////////////////
	// Intersect grid bounding box and find starting point

	float t, maxT = -1;
	int candidatePlane = -1;

	Vec3 invRayDir;
	Vec3 start;

	for (n = 0; n < 3; n++) {
		invRayDir[n] = 1.0f / ray0.dir[n];
		if (ray0.ori[n] < min[n]) {
			if (ray0.dir[n] < cGridEps) return NULL;
			t = (min[n] - ray0.ori[n]) * invRayDir[n];
			if (t > maxT) { maxT = t; candidatePlane = n; start[n] = min[n]; }
		}
		else if (ray0.ori[n] > max[n]) {
			if (ray0.dir[n] > -cGridEps) return NULL;
			t = (max[n] - ray0.ori[n]) * invRayDir[n];
			if (t > maxT) { maxT = t; candidatePlane = n; start[n] = max[n]; }
		}
	}

	if (candidatePlane == -1) {
		// If no candidate plane was found, ray0.ori is already inside the grid
		start = ray0.ori;
	}
	else {
		if (maxT < 0.0f) return NULL;
		for (n = 0; n < 3; n++) {
			if (n != candidatePlane) {
				start[n] = ray0.ori[n] + maxT * ray0.dir[n];
				if (start[n] < min[n] || start[n] > max[n]) return NULL;
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	// Initialization phase

	int voxIdx[3], step[3], out[3]; // Three-dimensional voxel index
	int vIndex, vStep[3]; // One-dimensional index into voxel array
	Vec3 tMax, tDelta; // Step sizes for t variable

	for (n = 0; n < 3; n++)
	{
		// Find starting voxel...
		voxIdx[n] = int((start[n] - min[n]) / cellsize[n]);

		// Must clamp voxel index since starting point could be located on grid bound!
		clamp(voxIdx[n], 0, subdivs[n]-1);

		// Setup increment variables
		if (ray0.dir[n] > 0.0f) {
			step[n] = 1;
			out[n] = subdivs[n];
			vStep[n] = iStep[n];
			tMax[n] = (min[n] + (voxIdx[n]+1) * cellsize[n] - ray0.ori[n]) * invRayDir[n];
			tDelta[n] = cellsize[n] * invRayDir[n];
		}
		else {
			step[n] = -1;
			out[n] = -1;
			vStep[n] = -iStep[n];
			tMax[n] = (min[n] + voxIdx[n] * cellsize[n] - ray0.ori[n]) * invRayDir[n];
			tDelta[n] = cellsize[n] * -invRayDir[n];
		}
	}

	// Index directly into one-dimensional voxel array...
	vIndex = voxIdx[X] + voxIdx[Y] * iStep[Y] + voxIdx[Z] * iStep[Z];

	/////////////////////////////////////////////////////////////////////////////
	// Incremental update phase

	Shape* hitShape = NULL;

	for(;;)
	{
		if (mStoreVoxels) {
			Voxel vox; vox.x = voxIdx[X]; vox.y = voxIdx[Y]; vox.z = voxIdx[Z];
			mStoredVoxels.push_back(vox);
		}

		hitShape = findHitInList(ray0, voxels[vIndex], excludeShape);

		/*if (hitShape != NULL) { // Must check if hit is inside current voxel!
			if (ray0.tHit <= tMax[X] && ray0.tHit <= tMax[Y] && ray0.tHit <= tMax[Z])
				return hitShape;
			//mNumNearMisses++;
			//ray0.tHit = FLT_MAX;
		}*/

		if(tMax[X] < tMax[Y]) {
			if(tMax[X] < tMax[Z]) {
				if (hitShape != NULL && ray0.tHit <= tMax[X]) return hitShape;
				voxIdx[X] += step[X]; if(voxIdx[X] == out[X]) return NULL;
				vIndex += vStep[X];
				tMax[X] += tDelta[X];
			} else {
				if (hitShape != NULL && ray0.tHit <= tMax[Z]) return hitShape;
				voxIdx[Z] += step[Z]; if(voxIdx[Z] == out[Z]) return NULL;
				vIndex += vStep[Z];
				tMax[Z] += tDelta[Z];
			}
		} else {
			if(tMax[Y] < tMax[Z]) {
				if (hitShape != NULL && ray0.tHit <= tMax[Y]) return hitShape;
				voxIdx[Y] += step[Y]; if(voxIdx[Y] == out[Y]) return NULL;
				vIndex += vStep[Y];
				tMax[Y] += tDelta[Y];
			} else {
				if (hitShape != NULL && ray0.tHit <= tMax[Z]) return hitShape;
				voxIdx[Z] += step[Z]; if(voxIdx[Z] == out[Z]) return NULL;
				vIndex += vStep[Z];
				tMax[Z] += tDelta[Z];
			}
		}
	}
}

void Grid::printGridStatistics()
{
	printf("Grid created: %d x %d x %d cells\n"
				 "   cell size: %f x %f x %f\n"
				 "  total size: %f x %f x %f\n",
		subdivs[X], subdivs[Y], subdivs[Z],
		cellsize[X], cellsize[Y], cellsize[Z],
		boundsize[X], boundsize[Y], boundsize[Z]);
	printf("\n");

	int numCells = subdivs[X] * subdivs[Y] * subdivs[Z];
	int numActiveTris = 0;
	int numActiveCells = 0;
	int maxNumTrisInCell = 0;
	int numTris;
	for (int n = 0; n < numCells; n++)
	{
		numTris = (int)voxels[n].size();
		if (numTris > 0) {
			numActiveTris += numTris;
			numActiveCells++;
			if (numTris > maxNumTrisInCell) maxNumTrisInCell = numTris;
		}
	}
	printf("numActiveCells: %d\n", numActiveCells);
	printf(" numActiveTris: %d\n", numActiveTris);
	if (numActiveCells > 0)
		printf(" avgActiveTris: %f\n", float(numActiveTris) / float(numActiveCells));
	printf("    maxNumTris: %d\n", maxNumTrisInCell);
	printf("\n");
}

void Grid::dbgBeginStoringVoxels()
{
	mStoreVoxels = true;
	mStoredVoxels.clear();
}

void Grid::dbgEndStoringVoxels()
{
	mStoreVoxels = false;
}

void Grid::dbgDrawStoredVoxels()
{
	Vec3 pos;
	glColor3f(1,0,1);
	for (int n = 0; n < (int)mStoredVoxels.size(); n++) {
		pos[0] = min[0] + mStoredVoxels[n].x * cellsize[X];
		pos[1] = min[1] + mStoredVoxels[n].y * cellsize[Y];
		pos[2] = min[2] + mStoredVoxels[n].z * cellsize[Z];
		drawAAWireBox(cellsize, pos);
	}
}

void Grid::dbgDrawGrid()
{
	Vec3 pos;
	glBegin(GL_POINTS);
	glPointSize(3);
	for (int x = 0; x < subdivs[X]; x++) {
		for (int y = 0; y < subdivs[Y]; y++) {
			for (int z = 0; z < subdivs[Z]; z++) {
				pos[0] = min[0] + x * cellsize[X];
				pos[1] = min[1] + y * cellsize[Y];
				pos[2] = min[2] + z * cellsize[Z];
				if (voxel(x,y,z).size() == 0) {
					glColor3f(1,0,0);
				}
				else {
					glColor3f(1,1,0);
					//drawAAWireBox(cellsizeX, cellsizeY, cellsizeZ, Vec3(xf, yf, zf));
					glVertex3fv(&(pos + 0.5f * cellsize)[0]);
					glColor3f(0,1,0);
				}
				//glVertex3f(xf + 0.5f*cellsizeX, yf + 0.5f*cellsizeY, zf + 0.5f*cellsizeZ);
			}
		}
	}
	glEnd();

	glColor3f(0,0,1);
	drawAAWireBox(boundsize, min);
}
