#pragma once

#include "Ray.h"

#include "Texture.h"

//#pragma comment(lib, "../3rdparty/corona.lib")
//#include "3rdparty/corona.h"

class Vec3;
class Material;

extern Material gDefaultMaterial;

// Fresnel amounts (RGB?)
const float cFresnelGlass = 0.04f;
const float cFresnelStainless = 0.80f;

// Refractive indices
const float cRefrAir   = 1.0f;
const float cRefrGlass = 1.5f;
const float cRefrWater = 1.33f;

// flat paint: purely diffuse (Lambert)
// shiny plastic: light color specular
// metal: light * object color specular, minimized diffuse

class Material
{
public:
	Material()
	{
		diffColor.assign(0.5f, 0.5f, 0.5f);
		specColor.assign(1,1,1);
		shininess = 100.0f;
		anisotropy = AnisotropyNone;
		anisotropyPole.assign(0,1,0);
		fresnelAmountAtNormalIncidence = 0;
		reflect = 0; refract = 0; refractInv = 0;
		emitIntensity = 0.0f;
		//lambdaShift = 0.0f;

    mUseSchlickApprox = true;

		mTexture = NULL;
	}

	virtual ~Material()
	{
		if (mTexture) delete mTexture;
	}

	virtual void setRefractionIndex(float incident, float transmitted)
	{
		refract =	incident / transmitted;
		refractInv = 1.0f / refract;
	}

  /*
	virtual void loadTexture(const char *filepath)
	{
		texture = corona::OpenImage(filepath, corona::PF_R8G8B8);
		corona::FlipImage(texture, corona::CA_X);
		//corona::FlipImage(texture, corona::CA_Y);
	}
  */

	virtual Vec3 getColor(const Vec3& p, const Vec2& uv) const
	{
		if (mTexture != NULL) {
			return mTexture->getColor(p, uv);
		}
		return diffColor;
	}

  virtual Vec3 directLight(Vec3 &p, Vec3& N, Vec3&V, Vec3& R, Vec3& L,
    float NdotL, Vec3& matColor) const
  {
    Vec3 color;
    float diffBase, specBase;

    //float selfShadeFactor = NdotL;
    float selfShadeFactor = sqrtf(NdotL);

    if (anisotropy != Material::AnisotropyNone)
    {
      // TODO: Find some more anisotropic models, split wavelengths
      Vec3 T;
      //if (anisotropy == Material::AnisotropyUV) {
      //  T = hitObject->getTangent(p);
      //}
      //else
      {
        T = cross( N, anisotropyPole ).normalizeSafely();
        if (anisotropy == Material::AnisotropyBinormal) {
          T = cross(N,T);
        }
        else if (anisotropy == Material::AnisotropyTangentBinormal) {
          Vec3 B = cross(N,T);
          T = (T + B).normalizeSafely();
        }
        //else //(anisotropy == Material::AnisotropyTangent)
      }

      float lt = dot(L,T), vt = dot(V,T); // (do not clamp these!)

      float ltRoot = sqrtf(1-lt*lt);
      float vtRoot = sqrtf(1-vt*vt);

      //if (shininess > 0)
      {
        // dot(L,R) = 
        specBase = ltRoot * vtRoot - lt * vt;
      }

      // dot(N,L) = 
      diffBase = ltRoot;

      // TODO: May be desirable to raise diff and spec terms to power 2..4 since
      // most-significant normal does not account for entire lighting
      diffBase = powf(diffBase, 4);
      //diffBase *= diffBase; diffBase *= diffBase;

      // Approximate self-shadowing...
      diffBase *= selfShadeFactor;
      if (diffBase <= 0.0f) {
        diffBase = 0;
        //continue;
      }
    }
    else //(anisotropy == Material::AnisotropyNone)
    {
      diffBase = NdotL;
      //if (shininess > 0)
      {
        specBase = dot(L,R);
      }
    }

    if (refract == 0.0f) // allowDiffuse
    {
      // Diffuse lighting
      color += diffBase * matColor;
    }

    if (shininess > 0)
    {
      // Specular highlight
      if(specBase <= 0.0f) {
        //specBase = 0;
        return color;
      }
      float specPow;
      float n = shininess;
      if (mUseSchlickApprox) {
        specPow = specBase / (n - n * specBase + specBase);
      } else {
        specPow = powf(specBase, n);
      }

      if (anisotropy != Material::AnisotropyNone) {
        // BRDF projected area cosine dependency?!
        specPow *= selfShadeFactor; // Approximate self-shadowing...
      }

      // TODO: fresnel-modulated?
      color += specPow * specColor;
    }
    return color;
  }

	virtual void setPreviewMaterial() const
	{
		glColor4f(diffColor[0], diffColor[1], diffColor[2], 0.2f);
		if (emitIntensity > 0)
			glColor4f(emitColor[0], emitColor[1], emitColor[2], 0.2f);

		//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,
		//						 &Vec4(diffColor[0], diffColor[1], diffColor[2], 0.2f)[0]);
		Vec4 specular(0,0,0,1);
		if (shininess > 0) {
			specular.assign(1,1,1,1);
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
		}
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, &specular[0]);
	}

	// TODO: kDiff, kSpec (reflect?) = W(theta)
	Vec3 diffColor;

	Vec3 specColor;
	float shininess;
  bool mUseSchlickApprox;

	enum AnisotropyType {
		AnisotropyNone,
		AnisotropyUV,
		AnisotropyTangent,
		AnisotropyBinormal,
		AnisotropyTangentBinormal
	};
	AnisotropyType anisotropy;
	Vec3 anisotropyPole;

	float emitIntensity;
	Vec3 emitColor;

	float fresnelAmountAtNormalIncidence;
	float reflect;
	float refract;
	float refractInv;
	//float lambdaShift;

	//corona::Image* texture;
	Texture *mTexture;
};
