#include <io.h> // For _findfirst() etc

#include "ImagePlane.h"

#include "mathlib.h"
#include "Stratification.h"

#include "DebugDraw.h"

#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus")

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/

ImagePlane::ImagePlane()
{
	mRenderWidth = 200; mRenderHeight = 200;//150

	mDrawPreview = true;
	mDrawRealtime = false;

	mUseGammaCorrection = true;
	mUseAntiAlias = false;

	mRelativeAperture = 0;//0.10f;
	mFocusDistance = 2.0f;
	mNumDOFSamples = 25;//160;

	mPixels = NULL;
	mFatPixels = NULL;
	mBitmapPixels = NULL;

	setRandomSeed();

	mLightPosition.assign(0,0,100);
	//mLightPosition4.assign(0,0,100,1);

	mRenderThreadExit = false;
  mRenderThreadStop = false;
	mRenderThreadRedraw = false;
}

ImagePlane::~ImagePlane()
{
}

/////////////////////////////////////////////////////////////////////////////

/*static*/ DWORD WINAPI ImagePlane::ThreadFunc(LPVOID param)
{
	((ImagePlane *)param)->RenderThreadEntryPoint();
	return 0;
}

void ImagePlane::RenderThreadEntryPoint()
{
	while(!mRenderThreadExit)
	{
		if (mRenderThreadRedraw)
		{
      mRenderThreadRedraw = false;
      mRenderThreadStop = false;

			mCritSecPixels.lock();
			TraceScene();
			PostProcess();
			mCritSecPixels.unlock();

			//RequestRedraw();
		}
		else
		{
			// If we don't need a draw, call sleep() to yield some CPU time for this
			// thread - 1 ms is not even perceptable. (On a P3 500 under win98 a
			// sleep(0) would consume 70% CPU time, while sleep(1) uses just 1% CPU!)
			Sleep(10);
		}
	}
}

void ImagePlane::RequestRenderThreadRedraw()
{
  mRenderThreadStop = true;
	//SingleLock lock(&mCritSecPixels, TRUE);
	mRenderThreadRedraw = true;
}

/////////////////////////////////////////////////////////////////////////////

void ImagePlane::Init()
{
	PerformTests();

	prepareImage(mRenderWidth, mRenderHeight);

	//mRayEngine->init();

  mRenderThreadExit = false;

	DWORD dwThreadId;
	mhRenderThread = CreateThread(NULL, 0, ThreadFunc, (LPVOID)this, 0, &dwThreadId);

	//
	//RequestRenderThreadRedraw();
}

void ImagePlane::Shutdown()
{
  mRenderThreadStop = true;
	mRenderThreadExit = true;
	WaitForSingleObject(mhRenderThread, INFINITE);

	//mRayEngine->shutdown();

	delete [] mPixels;
	delete [] mFatPixels;
}

//int mMaxLevel;

void ImagePlane::TraceScene()
{
	int n;

	//_controlfp( _RC_CHOP, _MCW_RC );

	for (n = 0; n < mRenderHeight * mRenderWidth; n++) {
		mFatPixels[n].setZero();
		mPixels[n].setZero();
	}

	clock_t starttime = clock();
  //clock_t nowtime, lasttime = starttime;

	if (!mDrawRealtime) {
		printf("Rendering started...");
	}

	Shape *excludeObj = NULL;

	// Vertical FOV & aspect ratio
	float renderAspect = float(mRenderWidth) / float(mRenderHeight);
	float ydim = 2.0f * tanf( 0.5f * deg2rad(mCamControl->mFOV) );
	float xdim = ydim * renderAspect;
	float x0 = -0.5f * xdim;
	float y0 = -0.5f * ydim;

	float pixelSize = ydim / float(mRenderHeight - 1);
	float halfPixelSize = 0.5f * pixelSize;

	mFocusDistance = distance(mCamControl->mEyePos, mCamControl->mLookAt); // Cheating!!!
	float aaDiameter = pixelSize * mFocusDistance;
	float dofWeight = 1.0f / float(mNumDOFSamples + 1); // includes initial centered ray...
	//float dofWeight = 1.0f;
	float xRnd, yRnd;

	Vec3 hitPoint;

	Matrix4 view = mCamControl->getViewTransform();
	//view = view.inverseOrtho();
	Ray ray; ray.ori = view.transform(Vec3(0,0,0)); //mEye;

	float px, py;
	int x, y, idx;

	/*
	if (!mDrawRealtime) {
		const cNumPhotons = 100;

		printf("Shooting photons...");
		mRayEngine->InstantRadiosity(cNumPhotons, 0.55f);
		printf("done!\n");

		float lightI = 1;

		mRayEngine->mAttC = lightI;
		mRayEngine->mAttL = 0;//0.25;
		mRayEngine->mAttQ = 1;//0.5;
		mRayEngine->mUseAttenuation = false;

		mRayEngine->mUseFresnel = false;
		mRayEngine->mUseGrid = false;
		mUseAntiAlias = false;

		LightSource PhotonPointLightSrc;
		mRayEngine->mSceneLights.clear();
		mRayEngine->mSceneLights.push_back(&PhotonPointLightSrc);
		for (int np = cNumPhotons-1; np >= 0; np--) {
			PhotonPointLightSrc.position = mRayEngine->mRadioSamples[np].pos;
			PhotonPointLightSrc.color = lightI * mRayEngine->mRadioSamples[np].color;
			float photonWeight = 1.0f / float(cNumPhotons - np);
		idx = 0;
		for (y = 0, py = y0; y < mRenderHeight; y++, py += pixelSize) {
			for (x = 0, px = x0; x < mRenderWidth; x++, px += pixelSize) {

				if (x == mRenderWidth / 2 && y == mRenderHeight / 2) {
					//mRayEngine->dbgBeginStoringRays();
					ray.color.assign(1,1,0);
				}

				// Pixel center ray
				ray.dir.assign(px,py,-1); ray.dir.normalize();
				ray.dir = view.vecTransform(ray.dir);
				mFatPixels[idx].colorSum += mRayEngine->TraceRay(ray, 0, excludeObj);
				mFatPixels[idx].color = mFatPixels[idx].colorSum * photonWeight;

				mPixels[idx] = mFatPixels[idx].color;

				if (mUseAntiAlias && !mRayEngine->mUsePathTracing)
				{
					// Pixel corner ray
					ray.dir.assign(px - halfPixelSize, py - halfPixelSize, -1);
					ray.dir.normalize();
					ray.dir = view.vecTransform(ray.dir);
					mFatPixels[idx].colorExtra = mRayEngine->TraceRay(ray, 0, excludeObj);
				}

				//mRayEngine->dbgEndStoringRays();

				idx++;

        if (mRenderThreadStop) break;
			}
		}
		RequestRedraw();
		}
		mRayEngine->mSceneLights.clear();
		return;
	}
	*/

	int numSamples = mNumDOFSamples;//mRayEngine->mUsePathTracing ? mNumDOFSamples : 1;

	for (n = 0; n < numSamples; n++)
	{
		idx = 0;
		//dofWeight = 1.0f / float(n + 2); // includes initial centered ray...
		//dofWeight = 1.0f / float(n + 1);
		float dofWeight2 = 1.0f / (float(n + 1) * dofWeight);
		// Motion blur...
		//((Sphere *)mSceneObjects[0])->centre[0] = 0.4f * float(n) / float(mNumDOFSamples);
		//
		for (y = 0, py = y0; y < mRenderHeight; y++, py += pixelSize) {
			for (x = 0, px = x0; x < mRenderWidth; x++, px += pixelSize) {

				if (x == mRenderWidth / 2 && y == mRenderHeight / 2) {
					mRayEngine->dbgBeginStoringRays();
					ray.color.assign(1,1,0);
				}

				// TODO: use circle of confusion to determine number of rays?
				// But not if AA or pathtracing...

				// TODO: optimize
				Vec3 ori(0,0,0);
				Vec3 dir(px,py,-1);
				float focusT = -(mFocusDistance + ori[2]) / dir[2];
				Vec3 focusPoint = ori + focusT * dir;

				//float depth = mFatPixels[idx].zHitDepth;

				//mFatPixels[idx].color *= 0; // TODO: dofWeight includes center ray?

				//if (mRayEngine->mUsePathTracing)
				{
					rndCircle(xRnd, yRnd);
					ray.ori.assign(mRelativeAperture * xRnd, mRelativeAperture * yRnd, 0);

					ray.dir = focusPoint - ray.ori;
					ray.dir[0] += aaDiameter * rnd0();
					ray.dir[1] += aaDiameter * rnd0();
					ray.dir.normalize();
				}
				/*
				else
				{
					if (mUseAntiAlias)
					{
						// Pixel corner ray
						ray.ori.setZero();
						ray.ori = view.transform(ray.ori);

						ray.dir.assign(px - halfPixelSize, py - halfPixelSize, -1);
						ray.dir.normalize();
						ray.dir = view.vecTransform(ray.dir);

						mFatPixels[idx].colorExtra = mRayEngine->TraceRay(ray, 0, excludeObj);
					}

					// Pixel center ray
					ray.ori.setZero();
					ray.dir.assign(px,py,-1); ray.dir.normalize();

					//hitPoint = ray.ori + ray.tHit * ray.dir;
					//mFatPixels[idx].zHitDepth = -view.invOrthoTransform(hitPoint)[2];

					//float depth = mFatPixels[idx].zHitDepth / 10;
					//float level = float(mMaxLevel) / float(mMaxRecursionLevel);
					//mFatPixels[idx].color = Vec3(depth,-depth,level);
				}
				*/

				ray.ori = view.transform(ray.ori);
				ray.dir = view.vecTransform(ray.dir);

				mFatPixels[idx].colorSum +=
					dofWeight * mRayEngine->TraceRay(ray, 0, excludeObj);

				mFatPixels[idx].color = mFatPixels[idx].colorSum * dofWeight2;

				// Restore centered ray origin for next pixel...
				//ray.ori = view.transform( Vec3(0,0,0) ); // TODO: optimize

				//mFatPixels[idx].color /= 5.0f;
				mPixels[idx] = mFatPixels[idx].color;

				mRayEngine->dbgEndStoringRays();

				idx++;

        if (mRenderThreadStop) break;
			}

			/*nowtime = clock();
      if (!mDrawRealtime && nowtime - lasttime > CLOCKS_PER_SEC) {
				lasttime = nowtime;
				//RequestRedraw();
			}*/
		}
	}

	clock_t endtime = clock();
	if (!mDrawRealtime) {
		printf("finished! Elapsed time: %.2f seconds\n",
			float(endtime - starttime) / CLOCKS_PER_SEC);
	}
}

void ImagePlane::PostProcess()
{
	const float cInvGamma = 1.0f / 2.2f;

	int x, y, idx = 0;
	// TODO: fix border handling etc, test alternating rotated quincunx methods
	for (y = 0; y < mRenderHeight; y++) {
		for (x = 0; x < mRenderWidth; x++) {
			//idx = x + y*mRenderWidth;

			if (mUseAntiAlias && !mRayEngine->mUsePathTracing) {
				if (y == mRenderHeight - 1) continue;
				mPixels[idx] = 0.5f * mFatPixels[idx].color +
											 0.125f * (mFatPixels[idx].colorExtra +
																 mFatPixels[idx+1].colorExtra +
																 mFatPixels[idx+mRenderWidth].colorExtra +
																 mFatPixels[idx+mRenderWidth+1].colorExtra);
			}
			else {
				mPixels[idx] = mFatPixels[idx].color;
			}

			bool mUsePostProcDOF = false;
			if (mUsePostProcDOF)
			{
				float radius = mFatPixels[idx].zHitDepth - mFocusDistance;
				if (radius < 0)
					radius *= -0.9f;
				else
					radius *= 0.02f;
				int r = 1 + int(radius);
				mPixels[idx].setZero();
				int xx, yy, idx2;
				float density, densitySum = 0;
				for (yy = y-r; yy < y+r; yy++) {
					for (xx = x-r; xx < x+r; xx++) {
						idx2 = xx + yy*mRenderWidth;
						if (idx2 < 0 || idx2 >= mRenderWidth*mRenderHeight) continue;
						//if (idx2 != idx) continue;
						density = radius*radius - (xx-x)*(xx-x) + (yy-y)*(yy-y);
						if (density < 0) density = 0; else density = sqrtf(density);
						densitySum += density;
						mPixels[idx] += density * mFatPixels[idx2].color;
					}
				}
				mPixels[idx] /= densitySum;
			}

			if (mUseGammaCorrection) {
				mPixels[idx] = Vec3(powf(mPixels[idx][0], cInvGamma),
														powf(mPixels[idx][1], cInvGamma),
														powf(mPixels[idx][2], cInvGamma));
			}

			mPixels[idx].clampVec(0,1);

			idx++;
		}
	}
}

/*
void ImagePlane::DrawRandomDistribution()
{
	int size = 300;

  glViewport(0, 0, size, size);

  glScissor(0, 0, size, size);
	glEnable(GL_SCISSOR_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);

	SetGLOrthoProjection();

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	// Draw border...
	glColor3f(0.5f,0.5f,0.5f);
	glBegin(GL_LINE_LOOP);
		glVertex2i(0, 0); glVertex2i(0, size-1);
		glVertex2i(size-1, size-1); glVertex2i(size-1, 0);
	glEnd();

	glColor3f(1,1,0);
	glBegin(GL_POINTS);

	Vec3 vec; float NdotVec;
	for (int n = 0; n < 1000; n++) {
		rndImplicitCosVec(vec);
		NdotVec = vec[2];
		if (NdotVec < 0) { vec = -vec; NdotVec = -NdotVec; }
		//vec *= NdotVec;
		glVertex2f(size*(0.5f*vec[0] + 0.5f), size*(0.5f*vec[2] + 0.5f));

		rndUnitVec(vec);
		NdotVec = vec[2];
		if (NdotVec < 0) { vec = -vec; NdotVec = -NdotVec; }
		//vec *= NdotVec;
		glVertex2f(size*(0.5f*vec[0] + 0.5f), size*(-0.5f*vec[2] + 0.5f));
	}

	//float xRnd, yRnd;
	//for (int n = 0; n < 1000; n++) {
	//	rndCircle(xRnd,yRnd);
	//	glVertex2f(size*(xRnd + 0.5f), size*(yRnd + 0.5f));
	//}

	glEnd();
}
*/

void ImagePlane::prepareImage(int width, int height)
{
	mCritSecPixels.lock();

	mRenderWidth  = width;
	mRenderHeight = height;

	delete [] mPixels;
	mPixels = new Vec3[mRenderWidth * mRenderHeight];

	delete [] mFatPixels;
	mFatPixels = new FatPixel[mRenderWidth * mRenderHeight];

	delete [] mBitmapPixels;
	mBitmapPixels = new UINT[mRenderWidth * mRenderHeight];

	//for (mTexWidth  = 1; mTexWidth  < mRenderWidth;  mTexWidth  *= 2);
	//for (mTexHeight = 1; mTexHeight < mRenderHeight; mTexHeight *= 2);

	mCritSecPixels.unlock();
}

void ImagePlane::saveImage(const char *filename)
{
	/*
	corona::Image* image = corona::CreateImage(
		mRenderWidth, mRenderHeight, corona::PF_R8G8B8);
	if (image == NULL) return;

	//typedef unsigned char byte;
	byte *pixels = (byte*)image->getPixels();
	byte *p = pixels;
	for (int i = 0; i < mRenderWidth * mRenderHeight; ++i)
	{
		*p++ = byte(mPixels[i][0] * 255.0f); // red
		*p++ = byte(mPixels[i][1] * 255.0f); // green
		*p++ = byte(mPixels[i][2] * 255.0f); // blue
		//*p++ = byte(mPixels[i][3] * 255.0f); //alpha;
	}
	corona::FlipImage(image, corona::CA_X);

	char pathname[255];
	if (filename != NULL) {
		strcpy(pathname, filename);
	}
	else {
		char *basepath = "screenshots/img";
		char *extension = ".png";
		int maxNumber = -1;
		std::string filter = basepath; filter += "*"; filter += extension;
		struct _finddata_t findinfo;
		intptr_t hFile = _findfirst(filter.c_str(), &findinfo);
		if (hFile != -1) {
			do {
				std::string name = findinfo.name;
				int number = atoi( name.substr(name.length()-8, 4).c_str() );
				if (number > maxNumber) maxNumber = number;
			} while (_findnext(hFile, &findinfo) == 0);
			_findclose(hFile);
		}
		sprintf(pathname, "%s%04d%s", basepath, maxNumber+1, extension);
	}

	if (!corona::SaveImage(pathname, corona::FF_AUTODETECT, image)) {
		// TODO: print errors in OpenGL console...
		//MessageBox(mhWnd, "Failed to save screenshot!", "", MB_OK);
	}
	delete image;
	*/
}

void ImagePlane::PerformTests()
{
/*
  volatile clock_t nowt, startt;
	int numIters = 100000000;

  volatile unsigned long xRnd;
  unsigned long seed = clock();
  printf("Seed = %d\n", seed);

	srand( seed );
	startt = clock();
	for (int n = 0; n < numIters; n++) {
		xRnd = rand();
	}
	nowt = clock();
	printf("Time to do %d calls to rand(): %.3f seconds.\n",
		numIters, float(nowt - startt) / float(CLOCKS_PER_SEC));

	init_genrand( seed );
	startt = clock();
	for (int n = 0; n < numIters; n++) {
		xRnd = genrand_int32();
	}
	nowt = clock();
	printf("Time to do %d calls to genrand_int32(): %.3f seconds.\n",
		numIters, float(nowt - startt) / float(CLOCKS_PER_SEC));
*/
}

/*
void ImagePlane::onChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	switch (toupper(nChar))
	{
		// 'F1', 'T', 'L' are occupied...
		case 'W' : mDrawRealtime = !mDrawRealtime; break;
		case 'A' :
			mCritSecPixels.lock();
			mUseAntiAlias = !mUseAntiAlias;
			PostProcess();
			mCritSecPixels.unlock();
			break;
		case 'D' : mRayEngine->mUseAttenuation = !mRayEngine->mUseAttenuation; break;
		case 'F' : mRayEngine->mUseFresnel = !mRayEngine->mUseFresnel; break;
		case 'S' : mRayEngine->mUseSchlickApprox = !mRayEngine->mUseSchlickApprox; break;
		//case 'H' : mRayEngine->mUseGrid = !mRayEngine->mUseGrid; break;
		case '+' :
			if (mMaxRecursionLevel < 5) mMaxRecursionLevel++;
			else mMaxRecursionLevel += 5;
			break;
		case '-' :
			if (mMaxRecursionLevel > 5) mMaxRecursionLevel -= 5;
			else if (mMaxRecursionLevel > 0) mMaxRecursionLevel--;
			break;
		case 'Z' : mRayEngine->mAttC = 1.0f - mRayEngine->mAttC; break;
		case 'X' : mRayEngine->mAttL = 1.0f - mRayEngine->mAttL; break;
		case 'C' : mRayEngine->mAttQ = 1.0f - mRayEngine->mAttQ; break;
		case 'V' :
		{
			// Shoot crosshair ray
			Matrix4 view = mCamControl->getViewTransform();
			Ray rayCrossHair; rayCrossHair.ori = view.transform(Vec3(0,0,0)); //mEye;
			float px = 0, py = 0;
			rayCrossHair.dir.assign(px,py,-1); rayCrossHair.dir.normalize();
			rayCrossHair.dir = view.vecTransform(rayCrossHair.dir);
			mRayEngine->dbgBeginStoringRays();
			rayCrossHair.color.assign(1,1,0);
			mRayEngine->TraceRay(rayCrossHair, 0);
			mRayEngine->dbgEndStoringRays();
			break;
		}
		case 'V' :
		{
			float lightI = mSceneLights[0]->color[0];
			lightI *= 0.9f;
			mSceneLights[0]->color.assign(lightI,lightI,lightI);
			mAttC = lightI;
			break;
		}
		case 'B' :
		{
			float lightI = mSceneLights[0]->color[0];
			lightI *= 1.1f;
			mSceneLights[0]->color.assign(lightI,lightI,lightI);
			mAttC = lightI;
			break;
		}
		case 'K' :
			mCritSecPixels.lock();
			saveImage();
			mCritSecPixels.unlock();
			break;
		case 'J' :
			if (mRenderWidth == 200) {
				prepareImage(800, 600);
			} else {
				prepareImage(200, 150);
			}
			RequestRenderThreadRedraw();
			break;
		case 'O' :
			mRayEngine->mUsePathTracing = !mRayEngine->mUsePathTracing;
			break;
		case 'Q' :
			mRayEngine->mUseImplicitCosSampling = !mRayEngine->mUseImplicitCosSampling;
			break;
		// TODO: Maybe call base class onChar() in default case?
	}
	//TestLabWnd::onChar(nChar, nRepCnt, nFlags);
}
*/
/////////////////////////////////////////////////////////////////////////////
// Menus and keyboard shortcuts
/*
enum RateRacerCommands
{
	cmdRender = 10,
	cmdToggleMenuBar,
	cmdShowPreview,
	cmdUseGammaCorr,
};

void ImagePlane::createMenus()
{
	SubMenu drop1("&Test", &mKeyTable);
		drop1.addMenuItem("&Render", cmdRender, 0, 'R');
		drop1.addMenuItem("Menu Bar", cmdToggleMenuBar, 0, VK_F2);
		drop1.addMenuItem("Show Preview", cmdShowPreview, 0, 'P');
		drop1.addSeparator();
		drop1.addMenuItem("Use Gamma Correction", cmdUseGammaCorr, 0, 'G');
	mMenuBar.addDropdownMenu(drop1);

	mMenuBar.refresh();
	//mMenuBar.hide();
	//mMenuBar.show();
}

bool ImagePlane::onCommand(UINT id, UINT ctrlNotifyCode, LONG_PTR pCtrl)
{
	// Return true if processing this command!
	switch (id)
	{
		case cmdToggleMenuBar :
			if (mMenuBar.isHidden()) mMenuBar.show(); else mMenuBar.hide();
			break;
		case cmdShowPreview : mDrawPreview = !mDrawPreview; break;
		case cmdRender : mRenderThreadRedraw = true; break;
		case cmdUseGammaCorr :
			mCritSecPixels.lock();
			mUseGammaCorrection = !mUseGammaCorrection;
			PostProcess();
			mCritSecPixels.unlock();
			break;
		// TODO: Maybe return base class onCommand() in default case?
	}
	InvalidateRect(mhWnd, NULL, FALSE);

	return TestLabWnd::onCommand(id, ctrlNotifyCode, pCtrl);
}
*/

void ImagePlane::updateGdiPlusBitmap()
{
	if (mPixels == NULL) return;
	int idx = 0;
	for(int y = mRenderHeight-1; y >= 0; y--) {
		for(int x = 0; x < mRenderWidth; x++) {
			byte r = byte(255.0f * min2(mPixels[idx][0], 1.0f));
			byte g = byte(255.0f * min2(mPixels[idx][1], 1.0f));
			byte b = byte(255.0f * min2(mPixels[idx][2], 1.0f));
			mBitmapPixels[x + y*mRenderWidth] = 0xFF000000 + (r << 16) + (g << 8) + b;
			idx++;
		}
	}
}

void ImagePlane::displayGdiPlusBitmap(HDC hdc)
{
	Graphics graphics(hdc);
	//INT row, col;

	/*
	static float scale = 0.00001f;
	for(int y = 0; y < N; y++) {
		for(int x = 0; x < N; x++) {
			float value = 0.5f * (1 + sinf(scale * (x*x + y*y)));
			int val = int(255.0f * value);
			mBitmapPixels[y][x] = 0xFF000000 + (val << 16) + (val << 8) + val;
		}
	}
	scale += 0.00001f; if (scale > 10.0f) scale = 0.1f;
	*/

	BitmapData bitmapData;
	bitmapData.Width = mRenderWidth;
	bitmapData.Height = mRenderHeight;
	bitmapData.Stride = 4*bitmapData.Width;
	bitmapData.PixelFormat = PixelFormat32bppARGB; 
	bitmapData.Scan0 = (VOID*)mBitmapPixels;
	bitmapData.Reserved = NULL;

	// Create a Bitmap object from a BMP file.
	//Bitmap bitmap(L"LockBitsTest2.bmp");
	// Display the bitmap before locking and altering it.
	//graphics.DrawImage(&bitmap, 10, 10);

	Bitmap bitmap(mRenderWidth, mRenderHeight, &graphics);

	// Lock a rectangular portion of the bitmap for writing.
	Rect rect(0, 0, mRenderWidth, mRenderHeight);

	bitmap.LockBits(
		&rect,
		ImageLockModeWrite | ImageLockModeUserInputBuf,
		PixelFormat32bppARGB,
		&bitmapData);

	// Commit the changes and unlock the portion of the bitmap.  
	bitmap.UnlockBits(&bitmapData);

	// Display the altered bitmap.
	graphics.SetInterpolationMode(InterpolationModeNearestNeighbor); 
	//graphics.SetInterpolationMode(InterpolationModeBicubic); 
	graphics.DrawImage(&bitmap, 0, 0, 401, 401);
	//graphics.DrawImage(&bitmap, 0, 0, 200, 200);
}
