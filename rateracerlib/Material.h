#pragma once

#include "Ray.h"

#include "Texture.h"

//#pragma comment(lib, "../3rdparty/corona.lib")
//#include "3rdparty/corona.h"

class Vec3;
class Material;

extern Material gDefaultMaterial;

// Fresnel amounts
const float cFresnelGlass = 0.04f;

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
