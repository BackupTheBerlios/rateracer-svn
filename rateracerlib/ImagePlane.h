#pragma once

#pragma comment(lib,"OpenGl32.lib")
#pragma comment(lib,"GLU32.lib")
#pragma comment(lib,"GLAUX.lib")

#pragma comment(lib,"winmm.lib")

#include <vector>

#include "Synchronization.h"
#include "CameraControl.h"

#include "Camera.h"

#include "RayEngine.h"

#include "LightSource.h"
#include "shapes/Shape.h"

struct FatPixel
{
	void setZero()
	{
		color.setZero();
		colorExtra.setZero();
		colorSum.setZero();
	}
	Vec3 color;
	Vec3 colorSum;
	Vec3 colorExtra;
	float zHitDepth;
};

class ImagePlane
{
public:
	ImagePlane();
	virtual ~ImagePlane();

//protected:
public:
	//void onChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//bool onCommand(UINT id, UINT ctrlNotifyCode, LONG_PTR pCtrl);

	virtual void Init();
	virtual void Shutdown();

	void TraceScene();
	void PostProcess();

	void saveImage(const char *filename = NULL);

	//void DrawRandomDistribution();

	bool mDrawPreview;
	bool mDrawRealtime;

	void prepareImage(int width, int height);
	int mRenderWidth, mRenderHeight;
	Vec3 *mPixels;
	FatPixel *mFatPixels;

	bool mUseGammaCorrection;
	bool mUseAntiAlias;

	float mRelativeAperture;
	float mFocusDistance;
	int mNumDOFSamples; // TODO: more general!

	RayEngine *mRayEngine;
	CameraControl *mCamControl;

	bool mUseTrackballControl;
	//bool mMoveLight;

	//Camera mDefaultCam;

	// TODO: do not move mousepos away, use current!
	POINT mMousePos;

	Vec3 mLightPosition;

	void PerformTests();

	int mWindowWidth, mWindowHeight;

	// Render thread stuff...

	static DWORD WINAPI ThreadFunc(LPVOID param);
	void RenderThreadEntryPoint();

	bool GetRenderThreadRedraw();
	void SetRenderThreadRedraw(bool v);

	bool GetRenderThreadExit();
	void SetRenderThreadExit(bool v);

	HANDLE mhRenderThread;
	CriticalSection mCritSecThreadExit;
	CriticalSection mCritSecPixels;
	bool mRenderThreadExit;
	bool mRenderThreadRedraw;
};
