#pragma once

#include <vector>

#include "mathlib/Vec3.h"

#include "shapes/Shape.h"
#include "LightSource.h"

#include "BoundVolHier.h"

class Model;

struct Photon {
	Vec3 pos;
	Vec3 color;
};

class Scene
{
public:
	Scene();
	virtual ~Scene();

	void InitScene();
	void DestroyScene();

	void drawScenePreview();
	void dbgDrawGrid();

	void calcSceneBoundingBox(Vec3& min, Vec3& max);

	void prepareGrid();
	Grid *mGrid;
	bool mUseGrid;
	bool mDrawGrid;

	std::vector<LightSource*>	mLights;
	std::vector<Shape*> mShapes;
	int mNumShapes;

	std::vector<Model*> mModels;

	Vec3 mBackgroundColor;
	Vec3 mBottomLevelColor;

	Vec3 mGlobalAmbientLight;

	bool mUseAttenuation;
	float mAttC, mAttL, mAttQ;

  BBox mBoundingBox;
};
