#include "ImagePlane.h"

#include "LightSource.h"

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

void RayEngine::InitScene()
{
	LightSource *src;
	Material *mat;
	Shape *obj;

	//mBottomLevelColor.assign(1,1,0);
	mBottomLevelColor.assign(0,0,0);

	//mSceneBackgroundColor.assign(0,0,0);
	//mSceneBackgroundColor.assign(0.5f,0.5f,0.6f);
	mSceneBackgroundColor.assign(0.5f,0.5f,1);
	//mSceneBackgroundColor *= 0.0f;

	float ambientI = 0.05f;
	mSceneAmbientLight.assign(ambientI,ambientI,ambientI);

	float lightI = 10000.0f;

	mUseAttenuation = true;
	// Attenuation from Voley van Dam: C = L = 0.25, Q = 0.5
	mAttC = lightI; //1;
	mAttL = 0;
	mAttQ = 1;

	src = new LightSource(Vec3(1,8,8), Vec3(lightI, lightI, lightI));
	//src = new LightSource(10*Vec3(1,8,-8), Vec3(lightI, lightI, lightI));
	mSceneLights.push_back(src);

	//src = new LightSource(Vec3(8,8,8), Vec3(lightI, 0, 0));
	//mSceneLights.push_back(src);

	//src = new LightSource(Vec3(-8,8,8), Vec3(0, lightI, 0));
	//mSceneLights.push_back(src);

	//src = new LightSource(Vec3(8,-8,8), Vec3(0, 0, lightI));
	//mSceneLights.push_back(src);
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
				mSceneObjects.push_back(obj);
			}
		}
	}

	obj = new Sphere(Vec3(0,0,2), 0.4f);
	obj->diffColor.assign(rnd(),rnd(),rnd());
	obj->shininess = 0;
	//obj->setRefractionIndex(cRefrWater, cRefrAir);
	mSceneObjects.push_back(obj);
*/
/*
	obj = new Sphere(Vec3(0,3,4), 2.0f);
	//obj->diffColor = mLightColor;
	obj->emitIntensity = 10.0f;
	obj->emitColor.assign(1,0.8f,0.3f);
	mSceneObjects.push_back(obj);

	// Red diffuse (Lambert) sphere
	obj = new Sphere(Vec3(0,0, 1.01f), 1);
	obj->diffColor.assign(0.7f,0.2f,0.2f);
	mSceneObjects.push_back(obj);
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
	mSceneObjects.push_back(obj);
*/

// Load some mesh object

#if 0
	//obj = new Model("models/plant.gw");
	//((Model*)obj)->mMaterials[0]->diffColor.assign(0.0f,0.8f,0.0f);
	//((Model*)obj)->mMaterials[1]->diffColor.assign(0.8f,0.0f,0.0f);
	//((Model*)obj)->mMaterials[2]->diffColor.assign(0.1f,0.0f,0.0f);
	//((Model*)obj)->mMaterials[0]->shininess = 0;
	//((Model*)obj)->mMaterials[1]->shininess = 0;
	//((Model*)obj)->mMaterials[2]->shininess = 0;
	//mSceneObjects.push_back(obj);
#endif

#if 0
	obj = new Model("models/cow.gw");
	mat = ((Model*)obj)->mMaterials[0];
	mat->diffColor.assign(0.1f, 0, 0.2f);
	mat->specColor.assign(0.1f, 0, 0.2f);
	mat->shininess = 50;
	mat->anisotropy = Material::AnisotropyTangent;
	//mat->anisotropy = Material::AnisotropyBinormal;
	//mat->anisotropy = Material::AnisotropyTangentBinormal;
	mSceneObjects.push_back(obj);
#endif

#if 0
	//obj = new Model("models/teapotpure.gw");
	obj = new Model("models/snaps.gw");
	//obj = new Model("models/simplehouse.gw");
	//obj = new Model("models/f5.gw");
	//obj = new Model("models/legoracer.gw");
	//obj = new Model("models/fiat.gw");
	mat = ((Model*)obj)->mMaterials[0];
	mat->diffColor.assign(1,1,1);
	mat->shininess = 500.0f;
	mat->fresnelAmountAtNormalIncidence = cFresnelGlass;
	mat->setRefractionIndex(cRefrAir, cRefrGlass);
	mSceneObjects.push_back(obj);
#endif

#if 0
	obj = new Model("models/texture_sphere.gw");
	mat = ((Model*)obj)->mMaterials[0];
	//mat->fresnelAmountAtNormalIncidence = 2*cFresnelGlass;
	mat->loadTexture("textures/earth.bmp");
	mSceneObjects.push_back(obj);
#endif

	// White diffuse (Lambert) sphere
	//obj = new Sphere(Vec3(0,0, 5), 1);
	//obj->diffColor.assign(1,1,1);
	//mSceneObjects.push_back(obj);

	/*
	// DOF test...
	obj = new Sphere(Vec3(0,2.5f-2.5f,0.5f), 0.5f);
	obj->diffColor.assign(1,0,0);
	mSceneObjects.push_back(obj);

	obj = new Sphere(Vec3(0,5-2.5f,0.5f), 0.5f);
	obj->diffColor.assign(0,1,0);
	mSceneObjects.push_back(obj);

	obj = new Sphere(Vec3(0,10-2.5f,0.5f), 0.5f);
	obj->diffColor.assign(0,0,1);
	mSceneObjects.push_back(obj);

	//obj = new Sphere(Vec3(0,500-2.5f,0.5f), 120);
	//mSceneObjects.push_back(obj);
	*/

#if 1
	// diffuse (Lambert) spheres

	obj = new Sphere(Vec3(0,1.01f,1.5f), 1);
	obj->material = mat = new Material();
	mat->diffColor.assign(1,1,1);
	mat->shininess = 0;
	mSceneObjects.push_back(obj);

	obj = new Sphere(Vec3(0,1.01f,-1.5f), 1);
	obj->material = mat = new Material();
	mat->diffColor.assign(1,0,0);
	mat->shininess = 0;
	mSceneObjects.push_back(obj);

	obj = new Sphere(Vec3(1.5f,1.01f,0), 1);
	obj->material = mat = new Material();
	mat->diffColor.assign(0,1,0);
	mat->shininess = 0;
	mSceneObjects.push_back(obj);

	obj = new Sphere(Vec3(-1.5f,1.01f,0), 1);
	obj->material = mat = new Material();
	mat->diffColor.assign(0,0,1);
	mat->shininess = 0;
	mSceneObjects.push_back(obj);
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
	mSceneObjects.push_back(obj);

	// Hollow glass sphere
	obj = new Sphere(Vec3(-1.5f,1.01f,0), 1);
	((Sphere*)obj)->centre += pos;
	obj->material = mat = new Material();
	//mat->diffColor.assign(1,0,1);
	//mat->diffColor.assign(0.5f,0.5f,1);
	mat->diffColor.assign(1,1,1);
	//mat->diffColor *= 0.5f;
	//mat->diffColor.assign(0,0,0);
	mat->shininess = 50.0f;
	//mat->shininess = 0.0f;
	mat->fresnelAmountAtNormalIncidence = cFresnelGlass;
	//mat->lambdaShift = 0.005f;
	mat->setRefractionIndex(cRefrAir, cRefrGlass);
	//mat->setRefractionIndex(1,1);
	mSceneObjects.push_back(obj);

	// Massive glass sphere
	obj = new Sphere(Vec3(0,1.01f,-1.5f), 1);
	((Sphere*)obj)->centre += pos;
	obj->material = mat = new Material();
	//mat->diffColor.assign(0,1,1);
	mat->diffColor.assign(1,1,1);
	mat->fresnelAmountAtNormalIncidence = 2*cFresnelGlass;
	//mat->shininess = 500.0f;
	mat->shininess = 10.0f;
	//mat->shininess = 0.0f;
	mat->setRefractionIndex(cRefrAir, cRefrGlass);
	//mat->anisotropy = Material::AnisotropyTangent;
	//mat->anisotropy = Material::AnisotropyTangentBinormal;
	mSceneObjects.push_back(obj);

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
	mSceneObjects.push_back(obj);

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
	mSceneObjects.push_back(obj);
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
	mSceneObjects.push_back(obj);
#endif

#if 0
	// Inner sphere (air bubble in base)
	obj = new Sphere(Vec3(-2,0.31f, 3), 0.25f);
	obj->material = mat = new Material();
	//mat->diffColor.assign(1,0,1);
	mat->diffColor.assign(1,1,1);
	mat->setRefractionIndex(cRefrGlass, cRefrAir);
	mSceneObjects.push_back(obj);

	// Inner spheres (water/lemonade air bubbles)
	Vec3 middle = Vec3(-2,3.51f, 3);
	mat = new Material();
	mat->diffColor.assign(1,1,1);
	mat->setRefractionIndex(cRefrWater, cRefrAir);
	for (int i = 0; i < 25; i++) {
		obj = new Sphere(middle + Vec3(1.0f*rnd0(), 4*rnd0(), 1.0f*rnd0()),
										 0.02f + 0.05f*nrand());
		obj->material = mat;
		mSceneObjects.push_back(obj);
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
	mSceneObjects.push_back(obj);

	// Inner cylinder (air canal)
	obj = new Cylinder(Vec3(-2,1.01f,3), 3, 0.9f, 0.9f);
	obj->material = mat = new Material();
	//mat->diffColor.assign(0,1,1);
	mat->diffColor.assign(1,1,1);
	mat->setRefractionIndex(cRefrGlass, cRefrAir);
	//mat->setRefractionIndex(1,1);
	mSceneObjects.push_back(obj);

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
	mSceneObjects.push_back(obj);
#endif

#if 1
	// Floor disc
	obj = new Disc( Vec3(0,0,0), 0, 15, Vec3(0,1,0));
	obj->material = mat = new Material();
	//mat->diffColor.assign(1,1,1);
	mat->diffColor.assign(0.5f,0.5f,0.5f);
	mat->shininess = 60.0f;
	mat->fresnelAmountAtNormalIncidence = 0;
	//mat->loadTexture("textures/glass.bmp");
	//mat->reflect = 0.5f;
	mSceneObjects.push_back(obj);

	// Floor ////////////////////////////////////////////////////////////////////
	/*
	obj = new Plane( Vec3(0,0,1), 0 );
	obj->material = mat = new Material();
	//mat->diffColor.assign(1,1,1);
	mat->diffColor.assign(0.5f,0.5f,0.5f);
	mat->shininess = 60.0f;
	mat->fresnelAmountAtNormalIncidence = 0;
	//mat->loadTexture("textures/glass.bmp");
	mSceneObjects.push_back(obj);
	*/
#endif

#if 1
	// Roof ////////////////////////////////////////////////////////////////////
	obj = new Plane( Vec3(0,-1,0), -10 );
	obj->material = mat = new Material();
	mat->diffColor.assign(0,0,1);
	//mat->shininess = 1000.0f;
	//mat->reflect = 1.0f;
	mSceneObjects.push_back(obj);

	// Walls ////////////////////////////////////////////////////////////////////

	obj = new Plane( Vec3(0,0,1), -10 );
	obj->material = mat = new Material();
	mat->diffColor.assign(0,1,0);
	//mat->shininess = 1000.0f;
	//mat->reflect = 1.0f;
	mSceneObjects.push_back(obj);

	obj = new Plane( Vec3(0,0,-1), -10 );
	obj->material = mat = new Material();
	mat->diffColor.assign(1,1,0);
	//mat->shininess = 1000.0f;
	//mat->emitIntensity = 10.0f;
	//mat->emitColor.assign(1,1,1);
	//mat->reflect = 1.0f;
	mSceneObjects.push_back(obj);

	obj = new Plane( Vec3(1,0,0), -10 );
	obj->material = mat = new Material();
	mat->diffColor.assign(1,0,0);
	//obj->shininess = 1000.0f;
	//mat->reflect = 1.0f;
	mSceneObjects.push_back(obj);

	obj = new Plane( Vec3(-1,0,0), -10 );
	obj->material = mat = new Material();
	mat->diffColor.assign(1,0,1);
	//obj->shininess = 1000.0f;
	//mat->reflect = 1.0f;
	mSceneObjects.push_back(obj);
#endif

	mNumSceneObjs = (int)mSceneObjects.size();

	//if (mUsePathTracing) mNumSceneObjs -= 5;
}

void RayEngine::DestroyScene()
{
	int n;
	for(n = 0; n < (int)mSceneLights.size(); n++)
	{
		delete mSceneLights[n];
	}
	for(n = 0; n < (int)mSceneObjects.size(); n++)
	{
		delete mSceneObjects[n];
	}
}
