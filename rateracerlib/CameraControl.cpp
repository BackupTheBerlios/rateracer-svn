#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "CameraControl.h"

CameraControl::CameraControl()
{
	mFOV       =   45.0f;

	mClipZNear =    0.1f; // zNear > 0
	mClipZFar  = 2000.0f; // zFar  > 0

	mEyePos.assign(1, 1, 1);
	mLookAt.assign(0,0,0);
	//mUpVec.assign(0,0,1);
	mUpVec.assign(0,1,0);

	mLookDistance = 20;
	mLookAzimuth = 30;
	mLookElevation = 10;

	mUseTrackballControl = true;

	mViewTrf.setIdentity();
}

// NOTE: glViewport() should be setup before calling this function!
void CameraControl::SetGLPerspProjection()
{
	GLint dims[4];
	glGetIntegerv(GL_VIEWPORT, dims);

	float aspect = float(dims[2]) / float(dims[3]);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(mFOV,				// Y-axis field of vision (degrees)
								aspect,			// width/height ratio
								mClipZNear,	// zNear ( > 0 )
								mClipZFar);	// zFar  ( > 0 )

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// NOTE: glViewport() should be setup before calling this function!
void CameraControl::SetGLOrthoProjection()
{
	GLint dims[4];
	glGetIntegerv(GL_VIEWPORT, dims);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, dims[2], 0, dims[3], -1, 1 );

	// switch back to the modelview matrix and clear it
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void CameraControl::setView()
{
	//Vec3 lookDir = (Matrix4::rotateX(mLookElevation) *
	//										 Matrix4::rotateZ(mLookAzimuth))
	//										 .vecTransform(Vec3(0, 1, 0));

	Vec3 lookDir = (Matrix4::rotateX(-mLookElevation) *
												Matrix4::rotateY(mLookAzimuth))
												.vecTransform(Vec3(0, 0, 1));

	if (mUseTrackballControl) {
		mEyePos = mLookAt - mLookDistance * lookDir;
	} else {
		mLookAt = mEyePos + mLookDistance * lookDir;
	}

	/*
	Vec3 forward = (mLookAt - mEyePos).normalizeSafely();
	Vec3 side    = cross(forward, mUpVec).normalizeSafely();
	mEyePos += 0.1f * (mUpVec * mLookElevation + side * mLookAzimuth);
	forward = (mLookAt - mEyePos).normalizeSafely();
	mEyePos = mLookAt - mLookDistance * forward;
	mUpVec  = cross(side, forward);
	mLookAzimuth = mLookElevation = 0;
	*/

	mViewTrf = Matrix4::pointAt(mEyePos, mLookAt, mUpVec);
	glMultMatrixf(&mViewTrf.inverseOrtho()[0][0]);

	//gluLookAt(mEyePos[0], mEyePos[1], mEyePos[2],
	//					mLookAt[0], mLookAt[1], mLookAt[2],
	//					mUpVec[0],  mUpVec[1],  mUpVec[2]);
}

// Call setView() before calling this function...
Matrix4 CameraControl::getViewTransform()
{
	return mViewTrf;
}

// FIXME use (forward/side/up) for look radius etc.

void CameraControl::rotate(float azimuth, float elevation)
{
	mLookAzimuth   += azimuth;
	mLookElevation += elevation;

	const float elevationLimit = 89;
	clamp(mLookElevation, -elevationLimit, elevationLimit);
}

void CameraControl::translate(float dx, float dy)
{
	if (mUseTrackballControl)
	{
		// Translate lookat point?
	}
	else
	{
		Vec3 forward = mLookAt - mEyePos;
		forward.normalizeSafely();

		Vec3 side = cross(forward, mUpVec);
		side.normalizeSafely();

		Vec3 up = cross(side, forward);
		up.normalizeSafely();

		side *= dx;
		mEyePos += side;

		up *= dy;
		mEyePos += up;
	}
}

void CameraControl::moveInOut(float distance)
{
	if (mUseTrackballControl)
	{
		mLookDistance += distance;
		if (mLookDistance < 0.01f) mLookDistance = 0.01f;
	}
	else
	{
		Vec3 forward = mLookAt - mEyePos;
		forward.normalizeSafely();

		Vec3 side = cross(forward, mUpVec);
		side.normalizeSafely();

		Vec3 up = cross(side, forward);
		up.normalizeSafely();

		forward *= -distance;

		mEyePos += forward;
	}
}

void CameraControl::zoomInOut(float factor)
{
	mLookDistance *= factor;
}

/* Aeroplane mode
newLookDir = Matrix4::rotate(-float(dx) * 0.1f, mUpVec ).transform(forward);
newLookDir = Matrix4::rotate(-float(dy) * 0.1f, side).transform(newLookDir);
mLookAt = mEyePos + newLookDir;
// + Roll + forward movement (only?)
//mUpVec = up;
*/

void CameraControl::keepElevationAboveGroundPlane()
{
	if (mUseTrackballControl) {
		if (mLookElevation >= -1) mLookElevation = -1;
	} else {
		if (mEyePos[1] < 0.1f) mEyePos[1] = 0.1f;
	}
}

