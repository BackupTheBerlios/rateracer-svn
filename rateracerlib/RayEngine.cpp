#include "RayEngine.h"
#include "Stratification.h"

#include "Grid.h"
#include "BoundVolHier.h"

#include "DebugDraw.h"

RayEngine::RayEngine()
{
  mScene = NULL;

	mMaxRecursionLevel = 15; // TODO: replace with diminishing returns

	mUseFresnel = true;

	mUsePathTracing = false;
	mUseImplicitCosSampling = true;
}

RayEngine::~RayEngine()
{
}

void RayEngine::init()
{
}

void RayEngine::shutdown()
{
}

float accumFunc1(float t, float d, float c)
{
	return c + (1-c)/(1+d*t);
}

float accumFunc2(float t, float d, float c)
{
	return c + (1-c)/(1+d*d*t*t);
}

float accumFunc(float t, float d, float c)
{
	return 1 - 0.5f*t;
}

// FIXME need two levels : totNumBounce and numDiffuseBounce...
Vec3 RayEngine::TraceRay(Ray& ray0, int level, Shape *excludeObject)
{
  // Rays needed: primary, final gather / path, shadow, reflect, refract

	if (level > mMaxRecursionLevel) return mScene->mBottomLevelColor;

	//if (level == 0) mMaxLevel = 0;
	//if (level > mMaxLevel) mMaxLevel = level;

	Shape *hitObject = findHit(ray0, excludeObject);

	dbgStoreRay(ray0);

	if (hitObject == NULL) return mScene->mBackgroundColor; //BackgroundTexture/material

	/////////////////////////////////////////////////////////////////////////////
  // FIXME store lots in ray / hit record...

	Vec3 p = ray0.hitPoint();
	Vec3 N = hitObject->getNormal(p);
	//Vec3 N = hitObject->getPrimitiveNormal(p);
	Vec3 V = -ray0.dir;

	bool hitInsideOfObject = false;
	if (dot(hitObject->getPrimitiveNormal(p), V) < 0) {
		//return Vec3(1,1,0);
		hitInsideOfObject = true;
		N = -N;
	}

	// Interpolated N sometimes makes NdotV become negative! TODO: write this nicer!
	float NdotV = dot(N,V);
	if (NdotV < 0) {
		//return Vec3(1,0,1);
		//N = -N;
		//NdotV = -NdotV;
		NdotV = 0; // Clamp to 90 degrees between N and V...
	}

  /////////////////////////////////////////////////////////////////////////////

  Vec2 uv = hitObject->getUV(p);

  // uvW local coord sys (orthonormal basis)
  // emit() diffuse() specular() subsurface() response + direction,
  // explicitBRDF(r,g,b)

  const Material *material = hitObject->material;

  Vec3 color(0,0,0); // = mScene->mBackgroundColor;

  Vec3 matColor = material->getColor(p, uv);

  // Debug: Early return (no shading)!
  //return matColor;

  //matColor = multiTonePaint(p, N,V);

  /////////////////////////////////////////////////////////////////////////////

  if (material->emitIntensity > 0) {
    return material->emitIntensity * material->emitColor;
  }

	bool allowDirectLighting =
		(material->refract == 0.0f || ray0.refractInsideCounter == 0); // < 1
	// TODO: no diffuse if fresnelTerm > 99% ???

	float fresnelTerm = 0;
	if (mUseFresnel)
	{
		// TODO: add material->useFresnel, possibly add fresnel exponent
		// dielectrics (plastic, glass, water) refr.index n = ca 1.5
		// fresnel = f(n, dot(V,H)) [Note: dot(L,H) can be used equivalently]
		// fa = fresnel refl. at normal incidence (ca 4% for diel.)
		// fresnel = fa + (1-fa) * (1 - dot(V,H))^5; // accurate to within 1%
		// fresnel = (1 - dot(V,H)) ^ 4; // dielectrics
		// fresnel = 1 / ((1 + dot(V,H))^7); // (or ^8) water
		// TODO: use fa or shininess or specular factor for control
		float fresnel = powf((1 - NdotV), 5);
		float fa = material->fresnelAmountAtNormalIncidence;
		fresnelTerm = fa + (1-fa)*fresnel;
		if (fresnelTerm > 1.0f) {
			printf("Fresnel = %f, fa = %f, NdotV = %f\n",
				fresnelTerm, fa, NdotV);
		}
	}

	if (material->refract > 0)
	{
    color = handleRefraction(material, ray0, NdotV, fresnelTerm,
      p, N, V, hitInsideOfObject, level, matColor);
	}

	Vec3 R = 2*NdotV*N - V; R.normalize();

	// TODO: use kDiff below to blend in diffuse even on refractive?
	// TODO: reflective/refractive path tracing (specular lobe sampling)?

	// Indirect light...
	if (material->refract == 0.0f) //allowDiffuse
	{
		if (!mUsePathTracing)
		{
			// Add in global ambient light
			color += compMul(matColor, mScene->mGlobalAmbientLight);
		}
		else if (level < 3)
		{
			// if (ray0.numDiffBounces = 0)
      int numSamples = 1;//(level == 0 ? 16 : 1);
      color += compMul(matColor, tracePaths( numSamples, level, p, N ));
		}
	}

	if (allowDirectLighting)
	{
    color += directIllumination(p, N, V, R, hitObject, material, matColor);
	}

	// FIXME...
	fresnelTerm = max2( fresnelTerm, material->reflect );

	const float cFresnelLimit = 0.0f;//0.001f;
	if (fresnelTerm > cFresnelLimit)
	{
		// Reflection ray
		Ray rayRefl(p, R); rayRefl.color.assign(1,0,0); //rayRefl.lambda = ray0.lambda;
		rayRefl.offset(cEpsilon, N);

		rayRefl.dimRet = ray0.dimRet * fresnelTerm;
		if (rayRefl.dimRet > 0.001f) {
			Vec3 colorRefl = TraceRay(rayRefl, level+1);

			// TODO:
			colorRefl = compMul(material->specColor, colorRefl);

			// TODO:
			//colorRefl += colorSpec;

			color = (1 - fresnelTerm) * color + fresnelTerm * colorRefl;
		}
	}

	return color;
}

Vec3 RayEngine::directIllumination(Vec3 &p, Vec3& N, Vec3&V, Vec3& R,
                                   Shape *hitObject, const Material* material,
                                   Vec3& matColor)
{
  Vec3 color;

  LightSource *lightSrc;
  float lightDistanceSq;
  Vec3 L;
  float NdotL;

  // Loop over lights...
  //if (!mUsePathTracing || level > 0)
  for (int i = 0; i < (int)mScene->mLights.size(); i++)
  {
    lightSrc = mScene->mLights[i];

    L = lightSrc->position - p;
    lightDistanceSq = dot(L,L);
    //L.normalize();
    L *= 1.0f / sqrtf(lightDistanceSq);

    NdotL = dot(N,L);

    // Check if surface is facing away from light (self-shadowing)...
    if (NdotL <= 0.0f) continue;

    // Shadow ray
    Ray rayShadow(p, L);
    rayShadow.offset(cEpsilon, N); // TODO: Not needed because of excludeObj?!
    Vec3 colorLight = lightSrc->color;
    Shape *hitOccluder = findHit(rayShadow, hitObject);
    if (hitOccluder != NULL &&
      rayShadow.tHit * rayShadow.tHit + cEpsilon < lightDistanceSq)
    {
      //return Vec3(1,1,0);
      colorLight.setZero();
      //continue; // FIXME!
    }
    else {
      if (mScene->mUseAttenuation) {
        colorLight *= 1.0f / (mScene->mAttC + /*mScene->mAttL * lightDistance*/ + mScene->mAttQ * lightDistanceSq);
      }
      if (mStoreRays) rayShadow.tHit = sqrtf(lightDistanceSq);
    }

    //if (hitOccluder != NULL) {
    rayShadow.color.assign(0,1,0);
    dbgStoreRay(rayShadow);
    //}

    color += compMul(colorLight,
      material->directLight(p, N, V, R, L, NdotL, matColor));
  }
  return color;
}

Vec3 RayEngine::tracePaths(int numSamples, int level, Vec3 p, Vec3 N)
{
  Vec3 color;

  // Perform path tracing (integrate over hemisphere / final gathering)
  float weightFactor = 1.0f / float(numSamples);

  if (!mUseImplicitCosSampling)
  {
    Vec3 vec;
    float NdotVec;
    for (int i = 0; i < numSamples; i++)
    {
      rndUnitVec(vec);
      NdotVec = dot(N, vec);
      if (NdotVec < 0) { vec = -vec; NdotVec = -NdotVec; }

      Ray rayPath(p, vec); rayPath.color.assign(0.5f,0.5f,0.25f);
      rayPath.offset(cEpsilon, N);
      Vec3 colorPath = TraceRay(rayPath, level+1);
      color += (weightFactor * NdotVec) * colorPath;
    }
  }
  else
  {
    Vec3 vec0, vec;
    Vec3 U, V;
    createONBasis(U,V,N);
    for (int i = 0; i < numSamples; i++)
    {
      rndImplicitCosVec(vec0);
      vec[0] = vec0[0] * U[0] + vec0[1] * V[0] + vec0[2] * N[0];
      vec[1] = vec0[0] * U[1] + vec0[1] * V[1] + vec0[2] * N[1];
      vec[2] = vec0[0] * U[2] + vec0[1] * V[2] + vec0[2] * N[2];

      Ray rayPath(p, vec); rayPath.color.assign(0.5f,0.5f,0.25f);
      rayPath.offset(cEpsilon, N);
      Vec3 colorPath = TraceRay(rayPath, level+1);
      color += weightFactor * colorPath;
    }
  }
  return color;
}

Vec3 RayEngine::handleRefraction(const Material* material, Ray& ray0, float NdotV,
                                 float& fresnelTerm,
                                 Vec3& p, Vec3& N, Vec3& V,
                                 bool hitInsideOfObject, int level,
                                 Vec3& matColor)
{
  Vec3 color(0,0,0);
  bool doRefraction = true;
  Vec3 T;

  if (material->refract == 1.0f && material->refractInv == 1.0f)
  {
    // Special case: non-altered refraction/transmission ray...
    T = ray0.dir;
  }
  else
  {
    //float ni = 1;	float nr = 1; float n = ni / nr;
    float n = material->refract;
    if (hitInsideOfObject) n = material->refractInv;

    float w = n * NdotV;
    float radical = 1 + (w - n)*(w + n);
    if (radical < 0) {
      // Total internal reflection...
      fresnelTerm = 1;
      doRefraction = false;
      //color = mScene->mBackgroundColor;
    }
    else
    {
      float k = sqrtf(radical);
      T = (w - k)*N - n*V;
      //T = (n*NdotV - sqrtf(1 - n*n*(1 - NdotV*NdotV)))*N - n*V;
    }
  }

  if (doRefraction) // fresnelTerm < 1.0f
  {
    Ray rayRefr(p, T);
    rayRefr.offset(cEpsilon, -N);
    //rayRefr.offset(cEpsilon, T);

    rayRefr.dimRet = ray0.dimRet * (1.0f - fresnelTerm);
    if (rayRefr.dimRet > 0.001f)
    {
      //rayRefr.transmitColor = matColor;
      if (hitInsideOfObject) {
        rayRefr.refractInsideCounter = ray0.refractInsideCounter - 1;
        rayRefr.color.assign(0,1,1);
      } else {
        rayRefr.refractInsideCounter = ray0.refractInsideCounter + 1;
        rayRefr.color.assign(1,0,1);
      }

      Vec3 colorRefr = TraceRay(rayRefr, level+1);

      /*Beer's Law:
      if (hitInsideOfObject)
      {
      const float beerFactor[3] = { 0.25f, 0.5f, 1.0f };
      colorRefr[0] *= expf(logf(beerFactor[0]) * ray0.tHit);
      colorRefr[1] *= expf(logf(beerFactor[1]) * ray0.tHit);
      colorRefr[2] *= expf(logf(beerFactor[2]) * ray0.tHit);
      }*/
      /*
      if (ray0.isTransmitting)
      {
      float f = accumFunc1(ray0.tHit, 1.0f/0.25f, 0.0f);
      Vec3 col = f*Vec3(1,1,1) + (1-f)*ray0.transmitColor;
      color = compMul(col, colorRefr);
      //color = colorRefr;
      }	else {
      */
      color = compMul(matColor, colorRefr);
      //color = colorRefr;
    }
  }
  return color;
}

Vec3 RayEngine::multiTonePaint(Vec3& p, Vec3& N, Vec3& V)
{
  Vec3 color;
  /*
  Multi-tone base paint layer + two-coat metallic sparkle layer
  //
  View-dependent lerp between 3 paint colors
  Uses subtractive tone to control overall color accumulation
  saturate(NdotV);
  colorShift = c0*NdotV + c1*NdotV^2 + c2*NdotV^4;
  //
  Simulate two layers of microflake deposits:
  k1 = 0.1; k2 = 1;
  N1 = (N + k1 * Nnoise).normalize();
  N2 = (N + k2 * Nnoise).normalize();
  sparkles = cFlakes * (N1dotV + N1dotV^2 + N1dotV^4 + N2dotV^16);
  //
  Combined approach:
  N = Normal
  Nn = Hi Freq Normalized Vector Noise
  Ns  = (a*Nn + b*N).normalize() // a << b
  Nss = (c*Nn + d*N).normalize() // c = d
  Ctwo_tone = c0*dot(Ns,V) + c1*dot(Ns,V)^2 + c2*dot(Ns,V)^4
  Cmicroflakes = c3*dot(Nss,V)^16
  //
  Add clear coat (HDR) reflective layer: (blur reflections by closs map)
  reflection * (1 - 0.5 * NdotV)
  */
  Vec3 Nn = Vec3(rnd0(),rnd0(),rnd0()).normalize();
  Vec3 perturbN1 = (N + 0.03f*Nn).normalize();
  Vec3 perturbN2 = (N + 0.05f*Nn).normalize();
  float base1, base2;
  if (mScene->mLights.size() == 0) {
    base1 = dot(perturbN1,V);
    base2 = dot(perturbN2,V);
  }
  else {
    Vec3 R1 = 2*dot(perturbN1,V)*perturbN1 - V; R1.normalize();
    Vec3 R2 = 2*dot(perturbN2,V)*perturbN2 - V; R2.normalize();
    Vec3 L = mScene->mLights[0]->position - p; L.normalize();
    base1 = dot(L,R1); if (base1 < 0) base1 = 0;
    base2 = dot(L,R2); if (base2 < 0) base2 = 0;
  }
  /*
  Vec3 colorBeg(1.0f,0.0f,0.0f);
  Vec3 colorMid(-1.0f,0,0);
  Vec3 colorEnd(0,1.0f,0);
  Vec3 colorSparkles(0.1f,0.1f,0.1f);
  */
  // These are the exact colors used in ATI's car demo...
  Vec3 colorBeg(0.4f,0.0f,0.35f);
  Vec3 colorMid(0.6f,0.0f,0.0f);
  Vec3 colorEnd(0.0f,0.35f,-0.35f);
  Vec3 colorSparkles(0.5f,0.5f,0.0f);
  //
  color = colorBeg * base1 + colorMid * powf(base1,2) + colorEnd * powf(base1,4);
  color += colorSparkles * powf(base2,16);
  return color;
}

Shape* RayEngine::findHit(Ray& ray0, Shape *excludeObject)
{
	ray0.tHit = FLT_MAX;

	Shape* hitObject = NULL;

	if (mScene->mUseGrid) {
		hitObject = mScene->mGrid->findHitInGrid(ray0, excludeObject);
		// TODO: kludge for unbound ground plane
		float t = -ray0.ori[1] / ray0.dir[1];
		if (t > cEpsilon && t < ray0.tHit) {
			ray0.tHit = t;
			hitObject = mScene->mShapes[mScene->mShapes.size()-1];
		}
    return hitObject;
	}

	Shape *o;
	float t;
	for (int i = 0; i < mScene->mNumShapes; i++) {
		o = mScene->mShapes[i];
		if (o == excludeObject) continue;
		t = o->collideRay(ray0);
		if (t > cEpsilon && t < ray0.tHit) {
			ray0.tHit = t;
			hitObject = o->getPrimitiveObject();
		}
	}
	return hitObject;
}

void RayEngine::InstantRadiosity(int N, float rho)
{
	float w; int reflections = 1;
	float start; int end;
	Vec3 L;
	//Vec3 p, dir;
	Ray r; r.color = Vec3(1,1,0);

	Vec3 normal;
	Photon photon;

	start = (float)N;
	end = N;

	mRadioSamples.clear();

	while (end > 0)
	{
		start *= rho;
		for (int i = (int)start; i < end; i++)
		{
			const float spread = 15.0f;
			const Vec3 lpos(0,10,3);
			r.ori = Vec3(lpos[0]+spread*rnd0(),lpos[1],lpos[2]+spread*rnd0());
			normal.assign(0,-1,0);
		  // Use an isometry to map unitsquare onto lightsource...
			//r.ori = pointonlightsrc( phi2(i), phi3(i) );

			L = Vec3(1,1,1);
			//L = Le(p) * suppLe;

			w = (float)N;

			for (int j = 1; j <= reflections; j++)
			{
				//printf("w = %f\n", w);
				photon.pos = r.ori;// + 0.1f*normal;
				photon.color = (N/w)*L;
				mRadioSamples.push_back(photon);
				//renderShadowed(L * (N/w), r.ori);
				//scaleAccum(1/N);

				//spherical coords: arcsin(sqrtf(phiB[2j+2](i))) , 2*PI*phiB[2j+3](i)
				//r.dir = wd( phiB[2j+2](i), phiB[2j+3](i) );
				Vec3 vec0;
				rndImplicitCosVec(vec0);
				Vec3 U, V; createONBasis(U,V,normal);
				r.dir[0] = vec0[0] * U[0] + vec0[1] * V[0] + vec0[2] * normal[0];
				r.dir[1] = vec0[0] * U[1] + vec0[1] * V[1] + vec0[2] * normal[1];
				r.dir[2] = vec0[0] * U[2] + vec0[1] * V[2] + vec0[2] * normal[2];
				r.offset(cEpsilon, r.dir);

				Shape *hitObject = findHit(r);
				if (hitObject == NULL) continue;
				/*
				while (!(hitObject != NULL && hitObject->material->refract > 0.0f)) {
					rndImplicitCosVec(vec0);
					createONBasis(U,V,normal);
					r.dir[0] = vec0[0] * U[0] + vec0[1] * V[0] + vec0[2] * normal[0];
					r.dir[1] = vec0[0] * U[1] + vec0[1] * V[1] + vec0[2] * normal[1];
					r.dir[2] = vec0[0] * U[2] + vec0[1] * V[2] + vec0[2] * normal[2];
					r.offset(cEpsilon, r.dir);
					hitObject = findHit(r);
				}
				*/
				
				dbgStoreRay(r);

				if (hitObject->material->refract > 0.0f) continue;
				/*
				while (hitObject->material->refract > 0.0f) {
					Vec3 T;
					r.ori = r.hitPoint();
					normal = hitObject->getNormal(r.ori);
					bool hitInsideOfObject = false;
					if (dot(hitObject->getPrimitiveNormal(r.ori), -r.dir) < 0) {
						hitInsideOfObject = true;
						normal = -normal;
					}
					float n = hitObject->material->refract;
					if (hitInsideOfObject) n = hitObject->material->refractInv;
					float w = n * posDot(normal,-r.dir);
					float radical = 1 + (w - n)*(w + n);
					if (radical < 0) {
						// Total internal reflection...
						hitObject = NULL;
						break; 
					}
					float k = sqrtf(radical);
					T = (w - k)*normal - n*(-r.dir);
					r.dir = T;
					r.offset(cEpsilon, -normal);
					hitObject = findHit(r);
					dbgStoreRay(r);
					if (hitObject == NULL) break;
				}
				if (hitObject == NULL) continue;
				*/
				if (hitObject->material->reflect > 0.0f) continue;
				r.ori = r.hitPoint();
				normal = hitObject->getNormal(r.ori);

				L = compMul(hitObject->material->diffColor, L) * posDot(normal,-r.dir);
				//L *= fd(p); // Diffuse BRDF

				w *= rho;
			}
		}
		reflections++;
		end = (int)start;
	}
}

void RayEngine::dbgBeginStoringRays()
{
	mStoreRays = true;
	mStoredRays.clear();
	//mScene->mGrid->dbgBeginStoringVoxels();
}

void RayEngine::dbgEndStoringRays()
{
	mStoreRays = false;
	//mScene->mGrid->dbgEndStoringVoxels();
}

void RayEngine::dbgStoreRay(const Ray& r)
{
	if (mStoreRays) {
		mStoredRays.push_back(r);
	}
}

void RayEngine::dbgDrawStoredRays()
{
	glLineWidth(1);
	glBegin(GL_LINES);
	Vec3 start, end;
	float t;
	for (int i = 0; i < (int)mStoredRays.size(); i++)	{
		start = mStoredRays[i].ori;
		t = mStoredRays[i].tHit; if (t > 1000) t = 1000;
		end = start + t * mStoredRays[i].dir;
		glColor3fv(&mStoredRays[i].color[0]);
		glVertex3fv(&start[0]);
		glVertex3fv(&end[0]);
	}
	glEnd();

	glPointSize(2);
	glColor3f(1,1,1);
	glBegin(GL_POINTS);
	for (int i = 0; i < (int)mRadioSamples.size(); i++)	{
		glVertex3fv(&mRadioSamples[i].pos[0]);
	}
	glEnd();
}
