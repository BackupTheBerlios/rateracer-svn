#pragma once

#include "mathlib.h"

class CameraControl
{
public:
	CameraControl();

	// NOTE: glViewport() should be setup before calling these functions!
	void SetGLPerspProjection();
	void SetGLOrthoProjection();

	void setView();

	// Call setView() before calling this function...
	Matrix4 getViewTransform();

	void rotate(float azimuth, float elevation);
	void translate(float dx, float dy);

	void moveInOut(float distance);
	void zoomInOut(float factor);

	void keepElevationAboveGroundPlane();

//protected:
	float mFOV;

	// View frustum
 	float mClipZNear;
	float mClipZFar;

	bool mUseTrackballControl;
	float mLookDistance, mLookAzimuth, mLookElevation;
	Vec3 mEyePos, mLookAt, mUpVec;

	Matrix4 mViewTrf; 
};
