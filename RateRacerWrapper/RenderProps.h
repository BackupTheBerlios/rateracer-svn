#pragma once

#include "rateracerlib/CameraControl.h"
#include "rateracerlib/RayEngine.h"
#include "rateracerlib/ImagePlane.h"

#using <system.drawing.dll>
using namespace System::Drawing;
using namespace System::Drawing::Drawing2D;

#define STD_PROPERTY(type, name, variable) \
	__property type get_##name()	{	return variable; } \
	__property void set_##name(type value)	{	variable = value;	}

namespace Gweronimo
{
	public __gc class RenderProps
	{
	public:
		RenderProps()
		{
		}

		~RenderProps()
		{
		}

		__property bool get_TrackballMode()
		{
			return mCamControl->mUseTrackballControl;
		}
		__property void set_TrackballMode(bool value)
		{
			mCamControl->mUseTrackballControl = value;
		}
	
		STD_PROPERTY(float, FieldOfView, mCamControl->mFOV)

    STD_PROPERTY(bool, UseGrid, mScene->mUseGrid)

		STD_PROPERTY(bool, UsePathtracing, mRayEngine->mUsePathTracing)

    STD_PROPERTY(bool, UsePhotonMap, mRayEngine->mUsePhotonMap)
    STD_PROPERTY(float, EstimateRadius, mRayEngine->mEstimateMaxDist)
    STD_PROPERTY(int, EstimateNPhotons, mRayEngine->mEstimateNPhotons)

		/*
		case 'D' : mRayEngine->mUseAttenuation = !mRayEngine->mUseAttenuation; break;
		case 'F' : mRayEngine->mUseFresnel = !mRayEngine->mUseFresnel; break;
		case 'S' : mRayEngine->mUseSchlickApprox = !mRayEngine->mUseSchlickApprox; break;
			if (mMaxRecursionLevel < 5) mMaxRecursionLevel++;
			else mMaxRecursionLevel += 5;
			break;
		case '-' :
			if (mMaxRecursionLevel > 5) mMaxRecursionLevel -= 5;
			else if (mMaxRecursionLevel > 0) mMaxRecursionLevel--;
			break;
		case 'Z' : mRayEngine->mAttC = 1.0f - mRayEngine->mAttC; break;
		mRayEngine->mUseImplicitCosSampling = !mRayEngine->mUseImplicitCosSampling;
		*/

    //STD_PROPERTY(Color, colour, mColor)
    Color mColor;

    //STD_PROPERTY(HatchStyle, hatch, mHatch)
    HatchStyle mHatch;

		CameraControl* mCamControl;
		RayEngine *mRayEngine;
		ImagePlane *mImagePlane;
		Scene *mScene;
	};
} // namespace