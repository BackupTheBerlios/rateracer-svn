#include "Scene.h"

#include "ImagePlane.h"

#include "LightSource.h"

#include "Grid.h"

#include "shapes/Sphere.h"
#include "shapes/MetaBalls.h"
#include "shapes/Plane.h"
#include "shapes/Disc.h"
#include "shapes/Cylinder.h"
//#include "shapes/Triangle.h"
#include "shapes/TriMesh.h"
#include "shapes/Model.h"

// TODO: move these globals elsewhere...
Material gDefaultMaterial;
Vec3 gDefaultNormal;
Vec2 gDefaultUV(0,0);

Scene::Scene()
{
	mUseGrid = true;
	mDrawGrid = false;

  mGrid = NULL;
}

Scene::~Scene()
{
}

void Scene::InitScene()
{
	LightSource *src;
	Material *mat;
	Shape *obj;
  Model *model;

  setRandomSeed();

	//mBottomLevelColor.assign(1,1,0);
	mBottomLevelColor.assign(0,0,0);

	//mBackgroundColor.assign(0,0,0);
	//mBackgroundColor.assign(0.5f,0.5f,0.6f);
	mBackgroundColor.assign(0.5f,0.5f,1);
	//mBackgroundColor *= 0.0f;

	float ambientI = 0.05f;
	mGlobalAmbientLight.assign(ambientI,ambientI,ambientI);

	float lightI = 10000.0f;

	mUseAttenuation = true;
	// Attenuation from Voley van Dam: C = L = 0.25, Q = 0.5
	mAttC = lightI; //1;
	mAttL = 0;
	mAttQ = 1;

  //src = new LightSource(Vec3(1,100,8), Vec3(lightI, lightI, lightI));
  src = new LightSource(Vec3(0,7,0), Vec3(lightI, lightI, lightI));
	//src = new LightSource(10*Vec3(1,8,-8), Vec3(lightI, lightI, lightI));
	mLights.push_back(src);

	//src = new LightSource(Vec3(8,8,8), Vec3(lightI, 0, 0));
	//mLights.push_back(src);

	//src = new LightSource(Vec3(-8,8,8), Vec3(0, lightI, 0));
	//mLights.push_back(src);

	//src = new LightSource(Vec3(8,-8,8), Vec3(0, 0, lightI));
	//mLights.push_back(src);
/*
	// Mega test
	const int numberPerAxis = 5;
	const float size = 0.1f; //1.0f / sqrtf(float(number));
	float sep = 0.25f;
	Vec3 min = Vec3(-0.4f, -0.4f, 1.6f);
	//Vec3 bounds = 2 * Vec3(0.4f, 0.4f, 0.4f);
	for (int i = 0; i < numberPerAxis; i++) {
		for (int j = 0; j < numberPerAxis; j++) {
			for (int k = 0; k < numberPerAxis; k++) {
				obj = new Sphere(min + Vec3(size, size,size) + Vec3(i*sep, j*sep, k*sep), size);
				obj->diffColor.assign(rnd(),rnd(),rnd());
				obj->shininess = 0;
				//obj->setRefractionIndex(cRefrWater, cRefrAir);
				mShapes.push_back(obj);
			}
		}
	}

	obj = new Sphere(Vec3(0,0,2), 0.4f);
	obj->diffColor.assign(rnd(),rnd(),rnd());
	obj->shininess = 0;
	//obj->setRefractionIndex(cRefrWater, cRefrAir);
	mShapes.push_back(obj);
*/
/*
	obj = new Sphere(Vec3(0,3,4), 2.0f);
	//obj->diffColor = mLightColor;
	obj->emitIntensity = 10.0f;
	obj->emitColor.assign(1,0.8f,0.3f);
	mShapes.push_back(obj);

	// Red diffuse (Lambert) sphere
	obj = new Sphere(Vec3(0,0, 1.01f), 1);
	obj->diffColor.assign(0.7f,0.2f,0.2f);
	mShapes.push_back(obj);
*/
/*
	// Metaballs test
	obj = new MetaBalls();
	const float cR = 2.0f;
	for (int i = 0; i < 20; i++) {
		((MetaBalls*)obj)->addBall(
			cR*Vec3(snrand(),snrand(),1+0*snrand()),
			1*(0.1f+0.9f*nrand()));
	}
	//((MetaBalls*)obj)->addBall( Vec3(0,0,1.01f), 1);
	//((MetaBalls*)obj)->addBall( Vec3(1,0,1.01f), 1);
	//((MetaBalls*)obj)->addBall( Vec3(0,1,1.01f), 1);
	obj->material = mat = new Material();
	mat->diffColor.assign(0.7f,0.2f,0.2f);
	mShapes.push_back(obj);
*/

// Load some mesh objects

#if 1
  //model = new Model("models/test.chunks");
  //model = new Model("models/lucy.chunks");
  model = new Model("models/causticRing.chunks");
  MaterialMap::iterator m = model->mMaterialMap.begin();
  for(; m != model->mMaterialMap.end(); m++)
  {
    m->second->diffColor.assign(nrand(), nrand(), nrand());
    //printf("Material: %s - %f %f %f\n",
    //  m->first.c_str(), m->second->diffColor[0], m->second->diffColor[1], m->second->diffColor[2]);
  }
  mat = model->mMaterialMap["lambert1"];
  if (mat != NULL) {
    //mat->diffColor.assign(1,0.9f,0.8f);
    mat->diffColor.assign(0.5f,0.5f,0.5f);
    mat->shininess = 0.0f;
    //mat->fresnelAmountAtNormalIncidence = cFresnelGlass;
    //mat->setRefractionIndex(cRefrAir, cRefrGlass);
  }
  mat = model->mMaterialMap["metal"];
  if (mat != NULL) {
    mat->diffColor.assign(1,0.8f,0.2f);
    //mat->diffColor.assign(1,1,1);
    mat->specColor.assign(1,0.8f,0.2f);
    mat->shininess = 1000.0f;
    mat->fresnelAmountAtNormalIncidence = 0.8f;
    mat->reflect = 0.5f;
  }
  //mShapes.push_back(obj);
  model->addToShapeList(mShapes);
  mModels.push_back(model);
#endif

#if 0
  //model = new Model("models/test.chunks");
  model = new Model("models/rings2.chunks");
  mat = model->mMaterialMap["glass"];
  if (mat != NULL) {
    mat->diffColor.assign(1,1,1);
    mat->shininess = 500.0f;
    mat->fresnelAmountAtNormalIncidence = cFresnelGlass;
    mat->setRefractionIndex(cRefrAir, cRefrGlass);
  }
  mat = model->mMaterialMap["wine"];
  if (mat != NULL) {
    mat->diffColor.assign(0.6f,0.06f,0.2f);
    mat->shininess = 50.0f;
    //mat->fresnelAmountAtNormalIncidence = cFresnelGlass;
    mat->setRefractionIndex(cRefrAir, cRefrWater);
  }
  mat = model->mMaterialMap["wineInsideGlass"];
  if (mat != NULL) {
    mat->diffColor.assign(0.6f,0.06f,0.2f);
    mat->shininess = 50.0f;
    mat->fresnelAmountAtNormalIncidence = cFresnelGlass;
    mat->setRefractionIndex(cRefrWater, cRefrGlass);
  }
  mat = model->mMaterialMap["photo"];
  if (mat != NULL) {
    mat->diffColor.assign(1,1,1);
    mat->shininess = 0;
  }
  mat = model->mMaterialMap["frame"];
  if (mat != NULL) {
    mat->diffColor.assign(0.6f,0.4f,0.05f);
    mat->shininess = 0;
  }
  mat = model->mMaterialMap["gold"];
  if (mat != NULL) {
    mat->diffColor.assign(1,0.8f,0.2f);
    //mat->diffColor.assign(1,1,1);
    mat->specColor.assign(1,0.8f,0.2f);
    mat->shininess = 1000.0f;
    mat->fresnelAmountAtNormalIncidence = 0.8f;
    mat->reflect = 0.5f;
  }
  mat = model->mMaterialMap["silver"];
  if (mat != NULL) {
    mat->diffColor.assign(1,1,1);
    //mat->diffColor.assign(1,1,1);
    mat->specColor.assign(1,1,1);
    mat->shininess = 1000.0f;
    mat->fresnelAmountAtNormalIncidence = 0.8f;
    mat->reflect = 0.5f;
  }
  mat = model->mMaterialMap["church"];
  if (mat != NULL) {
    mat->diffColor.assign(1,1,1);
    mat->shininess = 0;
  }
	//mShapes.push_back(obj);
  model->addToShapeList(mShapes);
  mModels.push_back(model);
#endif

#if 0
	//obj = new Model("models/plant.gw");
	//((Model*)obj)->mMaterials[0]->diffColor.assign(0.0f,0.8f,0.0f);
	//((Model*)obj)->mMaterials[1]->diffColor.assign(0.8f,0.0f,0.0f);
	//((Model*)obj)->mMaterials[2]->diffColor.assign(0.1f,0.0f,0.0f);
	//((Model*)obj)->mMaterials[0]->shininess = 0;
	//((Model*)obj)->mMaterials[1]->shininess = 0;
	//((Model*)obj)->mMaterials[2]->shininess = 0;
	//mShapes.push_back(obj);
#endif

#if 0
	obj = new Model("models/cow.gw");
	mat = ((Model*)obj)->mMaterialMap["MyCoolShader"];
	mat->diffColor.assign(0.1f, 0, 0.2f);
	mat->specColor.assign(0.1f, 0, 0.2f);
	mat->shininess = 50;
	mat->anisotropy = Material::AnisotropyTangent;
	//mat->anisotropy = Material::AnisotropyBinormal;
	//mat->anisotropy = Material::AnisotropyTangentBinormal;
	mShapes.push_back(obj);
#endif

#if 0
	//obj = new Model("models/teapotpure.gw");
	obj = new Model("models/snaps.gw");
	//obj = new Model("models/simplehouse.gw");
	//obj = new Model("models/f5.gw");
	//obj = new Model("models/legoracer.gw");
	//obj = new Model("models/fiat.gw");
	mat = ((Model*)obj)->mMaterialMap["test"];
	mat->diffColor.assign(1,1,1);
	mat->shininess = 500.0f;
	mat->fresnelAmountAtNormalIncidence = cFresnelGlass;
	mat->setRefractionIndex(cRefrAir, cRefrGlass);
	mShapes.push_back(obj);
#endif

#if 0
	obj = new Model("models/texture_sphere.gw");
	mat = ((Model*)obj)->mMaterialMap["test"];
	//mat->fresnelAmountAtNormalIncidence = 2*cFresnelGlass;
	mat->loadTexture("textures/earth.bmp");
	mShapes.push_back(obj);
#endif

	// White diffuse (Lambert) sphere
	//obj = new Sphere(Vec3(0,0, 5), 1);
	//obj->diffColor.assign(1,1,1);
	//mShapes.push_back(obj);

	/*
	// DOF test...
	obj = new Sphere(Vec3(0,2.5f-2.5f,0.5f), 0.5f);
	obj->diffColor.assign(1,0,0);
	mShapes.push_back(obj);

	obj = new Sphere(Vec3(0,5-2.5f,0.5f), 0.5f);
	obj->diffColor.assign(0,1,0);
	mShapes.push_back(obj);

	obj = new Sphere(Vec3(0,10-2.5f,0.5f), 0.5f);
	obj->diffColor.assign(0,0,1);
	mShapes.push_back(obj);

	//obj = new Sphere(Vec3(0,500-2.5f,0.5f), 120);
	//mShapes.push_back(obj);
	*/

#if 0
	// diffuse (Lambert) spheres

	obj = new Sphere(Vec3(0,1.01f,1.5f), 1);
	obj->material = mat = new Material();
	mat->diffColor.assign(1,1,1);
	mat->shininess = 0;
	mShapes.push_back(obj);

	obj = new Sphere(Vec3(0,1.01f,-1.5f), 1);
	obj->material = mat = new Material();
	mat->diffColor.assign(1,0,0);
	mat->shininess = 0;
	mShapes.push_back(obj);

	obj = new Sphere(Vec3(1.5f,1.01f,0), 1);
	obj->material = mat = new Material();
	mat->diffColor.assign(0,1,0);
	mat->shininess = 0;
	mShapes.push_back(obj);

	obj = new Sphere(Vec3(-1.5f,1.01f,0), 1);
	obj->material = mat = new Material();
	mat->diffColor.assign(0,0,1);
	mat->shininess = 0;
	mShapes.push_back(obj);
#endif

#if 0
	// Four Spheres
	//Vec3 pos(3,0,3);
	Vec3 pos(0,0,0);

	// Inner sphere (air bubble)
	//obj = new Sphere(Vec3(-1.5f,0, 1.21f), 0.75f);
	obj = new Sphere(Vec3(-1.5f,1.01f,0), 0.95f);
	((Sphere*)obj)->centre += pos;
	obj->material = mat = new Material();
	//mat->diffColor.assign(1,0,1);
	mat->diffColor.assign(1,1,1);
	//mat->diffColor.assign(0,0,0);
	mat->shininess = 0.0f;
	mat->setRefractionIndex(cRefrGlass, cRefrAir);
	//mat->setRefractionIndex(1,1);
	mShapes.push_back(obj);

	// Hollow glass sphere
	obj = new Sphere(Vec3(-1.5f,1.01f,0), 1);
	((Sphere*)obj)->centre += pos;
	obj->material = mat = new Material();
	//mat->diffColor.assign(1,0,1);
	mat->diffColor.assign(0.5f,0.5f,1);
	//mat->diffColor.assign(1,1,1);
	//mat->diffColor *= 0.5f;
	//mat->diffColor.assign(0,0,0);
	mat->shininess = 50.0f;
	//mat->shininess = 0.0f;
	mat->fresnelAmountAtNormalIncidence = cFresnelGlass;
	//mat->lambdaShift = 0.005f;
	mat->setRefractionIndex(cRefrAir, cRefrGlass);
	//mat->setRefractionIndex(1,1);
	mShapes.push_back(obj);

	// Massive glass sphere
	obj = new Sphere(Vec3(0,1.01f,-1.5f), 1);
	((Sphere*)obj)->centre += pos;
	obj->material = mat = new Material();
	mat->diffColor.assign(1,0.5f,0.5f);
	//mat->diffColor.assign(1,1,1);
	mat->fresnelAmountAtNormalIncidence = 2*cFresnelGlass;
	//mat->shininess = 500.0f;
	mat->shininess = 10.0f;
	//mat->shininess = 0.0f;
	mat->setRefractionIndex(cRefrAir, cRefrGlass);
	//mat->anisotropy = Material::AnisotropyTangent;
	//mat->anisotropy = Material::AnisotropyTangentBinormal;
	mShapes.push_back(obj);

	// Green glossy sphere
	obj = new Sphere(Vec3(1.5f,1.01f,0), 1);
	((Sphere*)obj)->centre += pos;
	obj->material = mat = new Material();
	mat->diffColor.assign(0,1,0);
	//mat->shininess = 500.0f;
	mat->shininess = 10.0f;
	//mat->shininess = 0.0f;
	mat->fresnelAmountAtNormalIncidence = 0;
	//mat->anisotropy = Material::AnisotropyTangent;
	mat->anisotropy = Material::AnisotropyBinormal;
	//mat->anisotropy = Material::AnisotropyTangentBinormal;
	mShapes.push_back(obj);

	// Reflective sphere
	obj = new Sphere(Vec3(0,1.01f,1.5f), 1);
	((Sphere*)obj)->centre += pos;
	obj->material = mat = new Material();
	mat->diffColor.assign(1,0.8f,0.2f);
	//mat->diffColor.assign(1,1,1);
	mat->specColor.assign(1,0.8f,0.2f);
	mat->shininess = 1000.0f;
	mat->fresnelAmountAtNormalIncidence = 0.8f;
	mat->reflect = 0.5f;
	mShapes.push_back(obj);
#endif

// Compact Disc (Anisotropic)
#if 0
	obj = new Disc(Vec3(-2,0.1f,3), 0.5f, 2.0f, Vec3(0,1,0));
	obj->material = mat = new Material();
	mat->diffColor.assign(1,1,0);
	//mat->diffColor.assign(1,1,1);
	mat->shininess = 200.0f;
	//mat->shininess = 0.0f;
	mat->fresnelAmountAtNormalIncidence = 0.0f;
	mat->reflect = 0.0f;
	mat->anisotropy = Material::AnisotropyUV;
	mShapes.push_back(obj);
#endif

#if 0
	// Inner sphere (air bubble in base)
	obj = new Sphere(Vec3(-2,0.31f, 3), 0.25f);
	obj->material = mat = new Material();
	//mat->diffColor.assign(1,0,1);
	mat->diffColor.assign(1,1,1);
	mat->setRefractionIndex(cRefrGlass, cRefrAir);
	mShapes.push_back(obj);

	// Inner spheres (water/lemonade air bubbles)
	Vec3 middle = Vec3(-2,3.51f, 3);
	mat = new Material();
	mat->diffColor.assign(1,1,1);
	mat->setRefractionIndex(cRefrWater, cRefrAir);
	for (int i = 0; i < 25; i++) {
		obj = new Sphere(middle + Vec3(1.0f*rnd0(), 4*rnd0(), 1.0f*rnd0()),
										 0.02f + 0.05f*nrand());
		obj->material = mat;
		mShapes.push_back(obj);
	}

	// Innermost solid cylinder (water/lemonade)
	obj = new Cylinder(Vec3(-2,1.02f,3), 2.5f, 0.89f);
	obj->material = mat = new Material();
	//mat->diffColor.assign(0,1,1);
	mat->diffColor.assign(1,0.2f,0.2f);
	mat->shininess = 500.0f;
	//mat->fresnelAmountAtNormalIncidence = cFresnelGlass;
	mat->setRefractionIndex(cRefrAir, cRefrWater);
	//mat->setRefractionIndex(1,1);
	mShapes.push_back(obj);

	// Inner cylinder (air canal)
	obj = new Cylinder(Vec3(-2,1.01f,3), 3, 0.9f, 0.9f);
	obj->material = mat = new Material();
	//mat->diffColor.assign(0,1,1);
	mat->diffColor.assign(1,1,1);
	mat->setRefractionIndex(cRefrGlass, cRefrAir);
	//mat->setRefractionIndex(1,1);
	mShapes.push_back(obj);

	// Hollow glass cylinder
	obj = new Cylinder(Vec3(-2,0.01f,3), 4, 1, 0.9f);
	obj->material = mat = new Material();
	//mat->diffColor.assign(0,1,1);
	mat->diffColor.assign(0.8f,1,1);
	//mat->diffColor.assign(0.5f,0.5f,1);
	mat->shininess = 50.0f;
	mat->fresnelAmountAtNormalIncidence = cFresnelGlass;
	mat->setRefractionIndex(cRefrAir, cRefrGlass);
	//mat->setRefractionIndex(1,1);
	mShapes.push_back(obj);
#endif

#if 0
	// Roof ////////////////////////////////////////////////////////////////////
	obj = new Plane( Vec3(0,-1,0), -10 );
	obj->material = mat = new Material();
	mat->diffColor.assign(0,0,1);
	//mat->shininess = 1000.0f;
	//mat->reflect = 1.0f;
	mShapes.push_back(obj);

	// Walls ////////////////////////////////////////////////////////////////////

	obj = new Plane( Vec3(0,0,1), -10 );
	obj->material = mat = new Material();
	mat->diffColor.assign(0,1,0);
	//mat->shininess = 1000.0f;
	//mat->reflect = 1.0f;
	mShapes.push_back(obj);

	obj = new Plane( Vec3(0,0,-1), -10 );
	obj->material = mat = new Material();
	mat->diffColor.assign(1,1,0);
	//mat->shininess = 1000.0f;
	//mat->emitIntensity = 10.0f;
	//mat->emitColor.assign(1,1,1);
	//mat->reflect = 1.0f;
	mShapes.push_back(obj);

	obj = new Plane( Vec3(1,0,0), -10 );
	obj->material = mat = new Material();
	mat->diffColor.assign(1,0,0);
	//obj->shininess = 1000.0f;
	//mat->reflect = 1.0f;
	mShapes.push_back(obj);

	obj = new Plane( Vec3(-1,0,0), -10 );
	obj->material = mat = new Material();
	mat->diffColor.assign(1,0,1);
	//obj->shininess = 1000.0f;
	//mat->reflect = 1.0f;
	mShapes.push_back(obj);
#endif

#if 0
	// Floor disc
	obj = new Disc( Vec3(0,0,0), 0, 15, Vec3(0,1,0));
	obj->material = mat = new Material();
	//mat->diffColor.assign(1,1,1);
	mat->diffColor.assign(0.5f,0.5f,0.5f);
	mat->shininess = 60.0f;
	mat->fresnelAmountAtNormalIncidence = 0;
	//mat->loadTexture("textures/glass.bmp");
	//mat->reflect = 0.5f;
	mShapes.push_back(obj);

	// Floor ////////////////////////////////////////////////////////////////////
	/*
	obj = new Plane( Vec3(0,0,1), 0 );
	obj->material = mat = new Material();
	//mat->diffColor.assign(1,1,1);
	mat->diffColor.assign(0.5f,0.5f,0.5f);
	mat->shininess = 60.0f;
	mat->fresnelAmountAtNormalIncidence = 0;
	//mat->loadTexture("textures/glass.bmp");
	mShapes.push_back(obj);
	*/
#endif

	mNumShapes = (int)mShapes.size();
	//if (mUsePathTracing) mNumShapes -= 5;

  calcSceneBoundingBox(mBoundingBox.pp[0], mBoundingBox.pp[1]);

	//starttime = clock();
	prepareGrid();
	//endtime = clock();
	//printf("Grid build time: %.2f seconds\n",
	//float(endtime - starttime) / CLOCKS_PER_SEC);

  printf("numshapes: %d\n", mNumShapes);

  // BVH code below ///////////////////////////////////////////////////////
return;
  typedef Shape* ShapePtr;
  Shape** shapes = new ShapePtr[mNumShapes];

  for (int n = 0; n < mNumShapes; n++)
    shapes[n] = mShapes[n];
  mShapes.clear();

  mShapes.push_back(BVH::buildBVH(shapes, mNumShapes, 0) );
	mNumShapes = (int)mShapes.size();
}

void Scene::DestroyScene()
{
	delete mGrid;

	int n;
	for(n = 0; n < (int)mLights.size(); n++)
	{
		delete mLights[n];
	}
	for(n = 0; n < (int)mShapes.size(); n++)
	{
		delete mShapes[n];
	}
}

Material* Scene::getModelMaterial(int modelIdx, const char *materialName)
{
  if (modelIdx >= (int)mModels.size()) return NULL;
  return mModels[modelIdx]->mMaterialMap[materialName];
}

void Scene::calcSceneBoundingBox(Vec3& min, Vec3& max)
{
	Vec3 tmpMin, tmpMax;

	if (mNumShapes == 0) {
		min.assign( FLT_MAX, FLT_MAX, FLT_MAX);
		max.assign(-FLT_MAX,-FLT_MAX,-FLT_MAX);
		return;
	}
	
	mShapes[0]->calcBounds(min, max);
	for (int i = 1; i < mNumShapes; i++)
	{
		mShapes[i]->calcBounds(tmpMin, tmpMax);

		if (tmpMin[0] < min[0]) min[0] = tmpMin[0];
		if (tmpMin[1] < min[1]) min[1] = tmpMin[1];
		if (tmpMin[2] < min[2]) min[2] = tmpMin[2];

		if (tmpMax[0] > max[0]) max[0] = tmpMax[0];
		if (tmpMax[1] > max[1]) max[1] = tmpMax[1];
		if (tmpMax[2] > max[2]) max[2] = tmpMax[2];
	}
}

void Scene::drawScenePreview()
{
	glEnable(GL_CULL_FACE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
	//glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);

  float ambient[4]  = { 0, 0, 0, 0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	
	for (int i = 0; i < (int)mLights.size(); i++)
	{
		if (i >= GL_MAX_LIGHTS) break;
		const LightSource *lightSrc = mLights[i];
		Vec3 pos = mLights[i]->position;
		Vec3 color = mLights[i]->color;
		Vec4 LightPos(pos[0], pos[1], pos[2], 1);
		Vec4 LightCol(color[0], color[1], color[2], 1);
		LightCol.clampVec(0,1);
		glLightfv(GL_LIGHT0+i, GL_POSITION, (float*)&LightPos);
		glLightfv(GL_LIGHT0+i, GL_DIFFUSE, (float*)&LightCol);
		glLighti(GL_LIGHT0+i, GL_SPOT_CUTOFF, 180);
		//glLightfv(GL_LIGHT0+i, GL_SPOT_DIRECTION, lightdir);
		glEnable(GL_LIGHT0+i);
	}

	for (int i = 0; i < mNumShapes; i++) {
		//mShapes[i]->drawBoundingBoxes();
	}

	for (int i = 0; i < mNumShapes; i++) {
		if (mShapes[i]->material->refract > 0) continue;
		mShapes[i]->drawPreview();
	}

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_ONE, GL_ONE);

	glCullFace(GL_FRONT);
	for (int i = 0; i < mNumShapes; i++) {
		if (mShapes[i]->material->refract == 0) continue;
		mShapes[i]->drawPreview();
	}

	glCullFace(GL_BACK);
	for (int i = 0; i < mNumShapes; i++) {
		if (mShapes[i]->material->refract == 0) continue;
		mShapes[i]->drawPreview();
	}

	glPopAttrib();
}

void Scene::prepareGrid()
{
  Vec3 eps(cEpsilon,cEpsilon,cEpsilon);

	mGrid = new Grid(50, mBoundingBox.pp[0]-eps, mBoundingBox.pp[1]+eps);
	
	for (int i = 0; i < mNumShapes; i++) {
		mShapes[i]->rasterize(mGrid);
	}

	mGrid->printGridStatistics();
}

void Scene::dbgDrawGrid()
{
	if (mDrawGrid) {
		mGrid->dbgDrawGrid();
		mGrid->dbgDrawStoredVoxels();
	}
}
