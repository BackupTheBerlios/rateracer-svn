#pragma once

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "_vcclrit.h"

#include "RenderProps.h"

#include "rateracerlib/rateracerlib.h"
#include "rateracerlib/CameraControl.h"
#include "rateracerlib/RayEngine.h"
#include "rateracerlib/ImagePlane.h"

using namespace System;
using namespace Gweronimo;

namespace RateRacerCore
{
	public __gc class RateRacerEngine
	{
	public:
		static void init()
		{
			__crt_dll_initialize();

			//HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
			//SetConsoleTextAttribute(hOut,
			//	BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY);

			mCamControl = new CameraControl();

			mScene = new Scene();
			mScene->InitScene();

	    //starttime = clock();
	    mScene->prepareGrid();
	    //endtime = clock();
	    //printf("Grid build time: %.2f seconds\n",
	    //float(endtime - starttime) / CLOCKS_PER_SEC);

			mRayEngine = new RayEngine();
			mRayEngine->mScene = mScene;
			mRayEngine->init();

			mImagePlane = new ImagePlane();
			mImagePlane->mCamControl = mCamControl;
			mImagePlane->mRayEngine = mRayEngine;
      mImagePlane->Init();

			mRenderProps = new RenderProps();
			mRenderProps->mCamControl = mCamControl;
			mRenderProps->mRayEngine  = mRayEngine;
			mRenderProps->mImagePlane = mImagePlane;
      mRenderProps->mScene      = mScene;
		}

		static void finish()
		{
			//mRenderProps->Dispose();
			delete mRenderProps;

      mImagePlane->Shutdown();
			delete mImagePlane;

			mRayEngine->shutdown();
			delete mRayEngine;

			mScene->DestroyScene();
			delete mScene;

			delete mCamControl;

			__crt_dll_terminate();
		}

		static void render()
		{
      // Transfer camera settings here!!!
			mImagePlane->RequestRenderThreadRedraw();
		}

		static RenderProps* mRenderProps;

		static CameraControl* mCamControl;

		static Scene *mScene;
		static RayEngine *mRayEngine;
		static ImagePlane *mImagePlane;
	};
}

/*
BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpvReserved)
{
  //Console::WriteLine(S"DllMain is called...");
  return TRUE;
}
*/