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

#include "rateracerlib/Texture.h"

using namespace System;
using namespace Gweronimo;

//using namespace System::Drawing;
using namespace System::Drawing::Imaging;

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

/*
      // Create a Bitmap object from a BMP file.
      Gdiplus::Bitmap bitmap(L"models/textures/Earth.bmp");
      // Display the bitmap before locking and altering it.
      //graphics.DrawImage(&bitmap, 10, 10);

      Gdiplus::BitmapData bitmapData;
//      bitmapData.Width  = width;
//      bitmapData.Height = height;
//      bitmapData.Stride = 4*bitmapData.Width;
//      bitmapData.PixelFormat = PixelFormat32bppARGB; 
//      bitmapData.Scan0 = (VOID*)pixels;
//      bitmapData.Reserved = NULL;
      Rect rect(0, 0, bitmap.Width, bitmap.Height);
      bitmap.LockBits(
        &rect,
        ImageLockModeRead,
        PixelFormat32bppARGB,
        &bitmapData);
*/
      Bitmap *bmp = new Bitmap(L"models/textures/Earth.bmp");
      BitmapData* bmpData = bmp->LockBits(
        System::Drawing::Rectangle(0,0,bmp->Width, bmp->Height),
        //System::Drawing::Rectangle(10,10, 1,1),
        ImageLockMode::ReadOnly,
        //ImageLockMode::ReadWrite,
        PixelFormat::Format32bppArgb);

      Texture *tex = new Texture();
      mScene->getModelMaterial(0, "lambert1")->mTexture = tex;
      //mScene->mModels[0]->mMaterialMap["lambert1"]->mTexture
      tex->createFromBitmapPixels(
        bmpData->Width, bmpData->Height, (UINT*)bmpData->Scan0.ToPointer());

      bmp->UnlockBits(bmpData);
//      bitmap.UnlockBits(&bitmapData);
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

		static void startRendering()
		{
      // Transfer camera settings here!!!
			mImagePlane->RequestRenderThreadRedraw();
		}

    static void stopRendering()
    {
      mImagePlane->RequestRenderThreadStop();
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