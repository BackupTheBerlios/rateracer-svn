#pragma once

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "_vcclrit.h"

#include "rateracerlib/rateracerlib.h"
#include "rateracerlib/CameraControl.h"
#include "rateracerlib/RayEngine.h"
#include "rateracerlib/TraceWnd.h"

using namespace System;

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
			mRayEngine = new RayEngine();
			mRayEngine->init();
			mTraceWnd = new TraceWnd();
			mTraceWnd->mCamControl = mCamControl;
			mTraceWnd->mRayEngine = mRayEngine;
		}

		static void finish()
		{
			delete mTraceWnd;
			mRayEngine->shutdown();
			delete mRayEngine;
			delete mCamControl;

			__crt_dll_terminate();
		}

		static void render()
		{
			mTraceWnd->SetRenderThreadRedraw(true);
		}

		static CameraControl* mCamControl;
		static RayEngine *mRayEngine;
		static TraceWnd *mTraceWnd;
	};
}

/*
BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpvReserved)
{
  //Console::WriteLine(S"DllMain is called...");
  return TRUE;
}
*/