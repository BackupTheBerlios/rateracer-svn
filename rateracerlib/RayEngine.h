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
  Vec3 InstantTraceRay(Ray& ray0, int level, Vec3& lpos);

  Vec3 tracePaths(int numSamples, int level, Vec3 p, Vec3 N);
  Vec3 directIllumination(Vec3 &p, Vec3& N, Vec3&V, Vec3& R,
    Shape *hitObject, const Material* material, Vec3& matColor);

	Shape* findHit(Ray& ray0, Shape *excludeObject = NULL);

	void dbgBeginStoringRays();
	void dbgEndStoringRays();
	void dbgStoreRay(const Ray& r);
	void dbgDrawStoredRays();

  // tmp
  Vec3 handleRefraction(const Material* material, Ray& ray0, float NdotV, float& fresnelTerm,
    Vec3& p, Vec3& N, Vec3& V, bool hitInsideOfObject, int level,
    Vec3& matColor);
  Vec3 multiTonePaint(Vec3& p, Vec3& N, Vec3& V);

	// Tracer features

	bool mUseFresnel;

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
