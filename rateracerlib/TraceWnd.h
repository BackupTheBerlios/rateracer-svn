#pragma once

#pragma comment(lib,"OpenGl32.lib")
#pragma comment(lib,"GLU32.lib")
#pragma comment(lib,"GLAUX.lib")

#pragma comment(lib,"winmm.lib")

//#include "testlab/TestLabWnd.h"

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

class TraceWnd //: public TestLabWnd
{
// Construction
public:
	TraceWnd();
	virtual ~TraceWnd();

//protected:
public:
	void onChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	bool onCommand(UINT id, UINT ctrlNotifyCode, LONG_PTR pCtrl);

	void onMouseButton(int btn, int event, UINT flags, POINTS point);
	void onMouseWheel(UINT nFlags, short zDelta, POINTS pt);
	void onMouseMove(UINT nFlags, POINTS point);

	virtual void SetupOpenGL();
	//virtual bool PreRenderScene();
	virtual void RenderScene(int mWindowWidth, int mWindowHeight);
	//virtual bool PostRenderScene();
	virtual void ShutdownOpenGL();

	void TraceScene();
	void PostProcess();

	void drawPreview(int orix, int oriy, int sizex, int sizey);

	void saveImage(const char *filename = NULL);

	void drawCoordSysAxes();

	void DrawRandomDistribution();

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
