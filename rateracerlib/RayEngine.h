#pragma once

#include <vector>

#include "Scene.h"

#include "LightSource.h"
#include "shapes/Shape.h"

#include "Ray.h"

class RayEngine
{
public:
	RayEngine();
	virtual ~RayEngine();

	void init();
	void shutdown();

	Vec3 TraceRay(Ray& ray0, int level, Shape *excludeObject = NULL);

	Shape* findHit(Ray& ray0, Shape *excludeObject = NULL);

	void dbgBeginStoringRays();
	void dbgEndStoringRays();
	void dbgStoreRay(const Ray& r);
	void dbgDrawStoredRays();

	// Tracer features

	bool mUseFresnel;
	bool mUseSchlickApprox;

	bool mUsePathTracing;
	bool mUseImplicitCosSampling;

	int mMaxRecursionLevel;

	void InstantRadiosity(int N, float rho);
	std::vector<Photon> mRadioSamples;

	Scene *mScene;
	
	// Debug stuff
	bool mStoreRays;
	std::vector<Ray> mStoredRays;
};
