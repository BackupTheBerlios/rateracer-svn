#pragma once

#include <vector>

#include "LightSource.h"
#include "shapes/Shape.h"

#include "Ray.h"

void testGdiPlus(HDC hdc);
void updateGdiPlusBitmap(Vec3* fpix, int w, int h);

class RayEngine
{
public:
	RayEngine();
	virtual ~RayEngine();

	void init();
	void shutdown();

	void InitScene();
	void DestroyScene();

	Vec3 TraceRay(Ray& ray0, int level, Shape *excludeObject = NULL);

	Shape* findHit(Ray& ray0, Shape *excludeObject = NULL);

	void drawScenePreview();

	void dbgBeginStoringRays();
	void dbgEndStoringRays();
	void dbgDrawStoredRays();

	//void dbgDrawGrid();

	// Tracer features

	bool mUseFresnel;
	bool mUseSchlickApprox;

	bool mUsePathTracing;
	bool mUseImplicitCosSampling;

	int mMaxRecursionLevel;
	Vec3 mBottomLevelColor;

	void calcSceneBoundingBox(Vec3& min, Vec3& max);
	//void prepareGrid();
	//Grid *mGrid;
	//bool mUseGrid;
	//bool mDrawGrid;

	void InstantRadiosity(int N, float rho);
	struct Photon {
		Vec3 pos;
		Vec3 color;
	};
	std::vector<Photon> mRadioSamples;
	
	// Scene stuff

	std::vector<LightSource*>	mSceneLights;
	std::vector<Shape*> mSceneObjects;
	int mNumSceneObjs;

	Vec3 mSceneBackgroundColor;
	Vec3 mSceneAmbientLight;

	bool mUseAttenuation;
	float mAttC, mAttL, mAttQ;

	// Debug stuff

	bool mStoreRays;
	std::vector<Ray> mStoredRays;
};
