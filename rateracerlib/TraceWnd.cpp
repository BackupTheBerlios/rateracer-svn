/*
TODO:
- Ctrl-F5 to start program in Release!
- pressing 'j' twice deadlocks program!
- Check stack and heap usage!
- fast sqrt etc
- Clamp all dot products where applicable! posDot(a,b)
- Check color bleeding!
- Simulated refractive thickness (for single-sided meshes with occluded edges)
- Use HDRI loader from AL, or cubemaps...
- GIF anim / AVI
- adaptive epsilon (ray length)? multiple epsilons?
- offset epsilon along normal instead (+ ray direction?)
- GetProfileInt/Struct, timing class
- path-branching at deeper levels if ray is specular/refracted...
- StretchDIBBits()
- mark rectangle for subrendering!
- (turn slow stuff off while changing view?)
- rerender when view has changed and stabilized, auto-hide preview
- sync blitting to texture in realtime mode
- better preview: fix all shape types
- render progress/timing in gl window
- camera class (incl ray handling), colorRGB class?
- more lightsrc subclasses! Calc lighting inside lightsrc class?
- parallel/directional light?
- move light around (fix last touches only!) (move to control class?)
- glow/halo, distance functions
- efficient ground/wall/roof plane tests
- hardcoded floor, headlight?
- STD_LOOP etc, or replace STL?
- gloss map, radial fog, fuzzy/fogged refl/refr
*/

#include <io.h> // For _findfirst() etc

//#include "mainlib/GLWindow.h"
#include "TraceWnd.h"

#include "mathlib.h"
#include "Stratification.h"

#include "DebugDraw.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/

TraceWnd::TraceWnd()
{
	//mShowWindowInitially = SW_SHOWMAXIMIZED;

	mRenderWidth = 200; mRenderHeight = 200;//150

	mDrawPreview = true;
	mDrawRealtime = false;

	mUseGammaCorrection = true;
	mUseAntiAlias = true;

	mRelativeAperture = 0;//0.10f;
	mFocusDistance = 2.0f;
	mNumDOFSamples = 25;//160;

	/*
	// TODO: setup camera in scene...
	mCamControl->mLookDistance  = 50.0f;
	mCamControl->mLookAzimuth   =  30;
	mCamControl->mLookElevation = -30;
	mCamControl->mLookAt.assign(0,0,1.5f);
	//mCamControl->mUseTrackballControl = false;
	mCamControl->mFOV = 60;
	*/

	mPixels = NULL;
	mFatPixels = NULL;

	setRandomSeed();

	mLightPosition.assign(0,0,100);
	//mLightPosition4.assign(0,0,100,1);

	mRenderThreadExit = false;
	mRenderThreadRedraw = false;
}

TraceWnd::~TraceWnd()
{
}

/////////////////////////////////////////////////////////////////////////////

/*static*/ DWORD WINAPI TraceWnd::ThreadFunc(LPVOID param)
{
	((TraceWnd *)param)->RenderThreadEntryPoint();
	return 0;
}

void TraceWnd::RenderThreadEntryPoint()
{
	SetRenderThreadExit(false);

	while(!GetRenderThreadExit())
	{
		if (GetRenderThreadRedraw())
		{
			SetRenderThreadRedraw(false);

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

bool TraceWnd::GetRenderThreadExit()
{
	SingleLock lock(&mCritSecThreadExit, TRUE);
	return mRenderThreadExit;
}

void TraceWnd::SetRenderThreadExit(bool v)
{
	SingleLock lock(&mCritSecThreadExit, TRUE);
	mRenderThreadExit = v;
}

bool TraceWnd::GetRenderThreadRedraw()
{
	SingleLock lock(&mCritSecPixels, TRUE);
	return mRenderThreadRedraw;
}

void TraceWnd::SetRenderThreadRedraw(bool v)
{
	SingleLock lock(&mCritSecPixels, TRUE);
	mRenderThreadRedraw = v;
}

/////////////////////////////////////////////////////////////////////////////

void TraceWnd::SetupOpenGL()
{
	//TestLabWnd::SetupOpenGL();

	PerformTests();

	prepareImage(mRenderWidth, mRenderHeight);

	//glGenTextures(1, &mTexObj);
	//prepareTexture();

	//mRayEngine->init();

	DWORD dwThreadId;
	mhRenderThread = CreateThread(NULL, 0, ThreadFunc, (LPVOID)this, 0, &dwThreadId);

	//
	//SetRenderThreadRedraw(true);
}

void TraceWnd::ShutdownOpenGL()
{
	SetRenderThreadExit(true);
	WaitForSingleObject(mhRenderThread, INFINITE);

	//mRayEngine->shutdown();

	delete [] mPixels;
	delete [] mFatPixels;

	//glDeleteTextures(1, &mTexObj);

	//TestLabWnd::ShutdownOpenGL();
}

void TraceWnd::RenderScene(int mWindowWidth, int mWindowHeight)
{
	//mCamControl->keepElevationAboveGroundPlane();
	//mCamControl->setView();
	//TestLabWnd::RenderScene();

	mCamControl->SetGLOrthoProjection();

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	//glEnable(GL_COLOR_MATERIAL);

	//if (mDrawRealtime) SetRenderThreadRedraw(true);

	int sizex = mRenderWidth;
	int sizey = mRenderHeight;
	while (sizex > mWindowWidth || sizey > mWindowHeight) {
		sizex /= 2; sizey /= 2;
	}
	while (2*sizex < mWindowWidth && 2*sizey < mWindowHeight) {
		sizex *= 2; sizey *= 2;
	}
	int orix = (mWindowWidth  - sizex) / 2;
	int oriy = (mWindowHeight - sizey) / 2;

	//for (int n = 0; n < mRenderHeight * mRenderWidth; n++) {
	//	mPixels[n].assign(0,0,0);
	//}
	//updateTexture();
	//drawTexture(orix, oriy, sizex, sizey);

	//updateGdiPlusBitmap(mPixels, mRenderWidth, mRenderHeight);

	//glRasterPos2i((mWindowWidth  - mRenderWidth ) / 2,
	//							(mWindowHeight - mRenderHeight) / 2);
	//glDrawPixels(mRenderWidth, mRenderHeight, GL_RGB, GL_FLOAT, mPixels);

	//DrawRandomDistribution();

	//if (mDrawPreview) {
		//drawPreview(orix, oriy, sizex, sizey);
	//}
}

//int mMaxLevel;

void TraceWnd::TraceScene()
{
	int n;

	//_controlfp( _RC_CHOP, _MCW_RC );

	for (n = 0; n < mRenderHeight * mRenderWidth; n++) {
		mFatPixels[n].setZero();
		mPixels[n].setZero();
	}

	clock_t nowtime, lasttime = clock();
	clock_t starttime = clock();
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
			}
			if (GetRenderThreadRedraw()) break;
			if (GetRenderThreadExit()) break;
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
			}
			if (GetRenderThreadRedraw()) break;
			if (GetRenderThreadExit()) break;

			nowtime = clock();
			if (!mDrawRealtime && nowtime - lasttime > CLOCKS_PER_SEC) {
				lasttime = nowtime;
				//RequestRedraw();
			}
		}
	}

	clock_t endtime = clock();
	if (!mDrawRealtime) {
		printf("finished! Elapsed time: %.2f seconds\n",
			float(endtime - starttime) / CLOCKS_PER_SEC);
	}
}

void TraceWnd::PostProcess()
{
	const float cInvGamma = 1.0f / 2.2f;

	int x, y, idx = 0;
	// TODO: fix border handling etc, test alternating rotated quincunx methods
	for (y = 0; y < mRenderHeight-1; y++) {
		for (x = 0; x < mRenderWidth; x++) {
			//idx = x + y*mRenderWidth;

			if (mUseAntiAlias && !mRayEngine->mUsePathTracing) {
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

void TraceWnd::drawCoordSysAxes()
{
	// Draw coord sys axes at origo
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
	glBegin(GL_LINES);
		glColor3f(1,0,0); glVertex3f(0,0,0); glVertex3f(1,0,0);
		glColor3f(0,1,0); glVertex3f(0,0,0); glVertex3f(0,1,0);
		glColor3f(0,0,1); glVertex3f(0,0,0); glVertex3f(0,0,1);
	glEnd();
	glPopAttrib();
}

void TraceWnd::onMouseButton(int btn, int event, UINT flags, POINTS point)
{
	//if (event == EVENT_DOWN)
	{
		while (ShowCursor(FALSE) >= 0);
		//SetCapture(mhWnd);
		ShowCursor(FALSE);
		GetCursorPos(&mMousePos);
		SetCursorPos(320, 240);
	}
	//else if (event == EVENT_UP)
	{
		SetCursorPos(mMousePos.x, mMousePos.y);
		//ReleaseCapture();
		while (ShowCursor(TRUE) < 0);
	}
	//GLWindow::onMouseButton(btn, event, flags, point);
}

void TraceWnd::onMouseWheel(UINT nFlags, short zDelta, POINTS pt) 
{
	if (zDelta < 0) {
		mCamControl->zoomInOut(0.9f);
	} else {
		mCamControl->zoomInOut(1.1f);
	}

	//InvalidateRect(mhWnd, NULL, FALSE);

	//GLWindow::onMouseWheel(nFlags, zDelta, pt);
}

void TraceWnd::onMouseMove(UINT nFlags, POINTS point) 
{
	static POINTS oldp = {320, 240};
	static POINT newp;

	//mDbgWinMgr->mouseEvent( int mMouseX, int mWinHeight - mMouseY, int mMouseButton );

	if (GetCapture())
	{
		GetCursorPos(&newp);
		SetCursorPos(320, 240);

		int dx = newp.x - oldp.x;
		int dy = newp.y - oldp.y;

		if (nFlags & MK_LBUTTON) {
			mCamControl->moveInOut(0.1f * dy);
		}
		else if (nFlags & MK_MBUTTON) {
			mCamControl->translate(0.1f * dx, -0.1f * dy);
		}
		else if (nFlags & MK_RBUTTON) {
			mCamControl->rotate(-0.1f * dx, -0.1f * dy);
		}

		//onPaint();

		//Invalidate(FALSE);
	}
	
	//GLWindow::onMouseMove(nFlags, point);
}

void TraceWnd::DrawRandomDistribution()
{
	int size = 300;

  glViewport(0, 0, size, size);

  glScissor(0, 0, size, size);
	glEnable(GL_SCISSOR_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);

	mCamControl->SetGLOrthoProjection();

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

	/*
	float xRnd, yRnd;
	for (int n = 0; n < 1000; n++) {
		rndCircle(xRnd,yRnd);
		glVertex2f(size*(xRnd + 0.5f), size*(yRnd + 0.5f));
	}
	*/

	glEnd();
}

void TraceWnd::drawPreview(int orix, int oriy, int sizex, int sizey)
{
  //SetupViewport(0, 3*mWindowHeight/4, mWindowWidth/4, mWindowHeight/4);
	//SetupViewport(orix, oriy, sizex, sizey);
	//ClearViewport();

	mCamControl->SetGLPerspProjection();
	mCamControl->setView();

	drawCoordSysAxes();

	glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);

	glDisable(GL_LIGHTING);

	// Draw crosshairs
	Matrix4 view = mCamControl->getViewTransform();
	glColor3f(0,1,1);
	glBegin(GL_LINES);
		glVertex3fv(&view.transform(Vec3(-0.1f,0,-1))[0]);
		glVertex3fv(&view.transform(Vec3( 0.1f,0,-1))[0]);
		glVertex3fv(&view.transform(Vec3(0,-0.1f,-1))[0]);
		glVertex3fv(&view.transform(Vec3(0, 0.1f,-1))[0]);
	glEnd();

	mRayEngine->dbgDrawStoredRays();
	//mRayEngine->dbgDrawGrid();

	mRayEngine->drawScenePreview();
}

/*
void TraceWnd::prepareTexture()
{
	Vec3 *tmpbuf = new Vec3[mTexWidth * mTexHeight];
	memset(tmpbuf, 0, mTexWidth * mTexHeight);
	glBindTexture(GL_TEXTURE_2D, mTexObj);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Needed for GL_RGB, default is 4 for GL_RGBA
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mTexWidth,mTexHeight, 0,
							 GL_RGB, GL_FLOAT, tmpbuf);
	delete [] tmpbuf;
}

void TraceWnd::updateTexture()
{
	glBindTexture(GL_TEXTURE_2D, mTexObj);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0,0, mRenderWidth,mRenderHeight, GL_RGB, GL_FLOAT, mPixels);
}

void TraceWnd::drawTexture(int orix, int oriy, int sizex, int sizey)
{
	GLint filter = GL_NEAREST; //GL_LINEAR
	float umax = float(mRenderWidth -1) / float(mTexWidth);
	float vmax = float(mRenderHeight-1) / float(mTexHeight);

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glBegin(GL_QUADS);
		glTexCoord2f(   0,   0); glVertex2i(orix - 1,     oriy - 1);
		glTexCoord2f(umax,   0); glVertex2i(orix + sizex, oriy - 1);
		glTexCoord2f(umax,vmax); glVertex2i(orix + sizex, oriy + sizey);
		glTexCoord2f(   0,vmax); glVertex2i(orix - 1,     oriy + sizey);
	glEnd();
	glPopAttrib();
}
*/

void TraceWnd::prepareImage(int width, int height)
{
	mCritSecPixels.lock();

	mRenderWidth  = width;
	mRenderHeight = height;

	delete [] mPixels;
	mPixels = new Vec3[mRenderWidth * mRenderHeight];

	delete [] mFatPixels;
	mFatPixels = new FatPixel[mRenderWidth * mRenderHeight];

	//for (mTexWidth  = 1; mTexWidth  < mRenderWidth;  mTexWidth  *= 2);
	//for (mTexHeight = 1; mTexHeight < mRenderHeight; mTexHeight *= 2);

	mCritSecPixels.unlock();
}

void TraceWnd::saveImage(const char *filename)
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

void TraceWnd::PerformTests()
{
	/*
	int numIters = 100000;

	Timer timer;
	volatile double nowtime;
	volatile clock_t nowt, startt;

	timer.resetAndStart();
	startt = clock();
	for (int n = 0; n < numIters; n++) {
		nowtime = timer.getSeconds();
	}
	nowt = clock();
	nowtime = timer.getSeconds();
	printf("Time to do %d calls to Timer.getSeconds(): %f / %.3f seconds.\n",
		numIters, nowtime, float(nowt - startt) / float(CLOCKS_PER_SEC));
	printf("\n");

	timer.resetAndStart();
	startt = clock();
	for (int n = 0; n < numIters; n++) {
		nowt = clock();
	}
	nowt = clock();
	nowtime = timer.getSeconds();
	printf("Time to do %d calls to clock(): %f / %.3f seconds.\n",
		numIters, nowtime, float(nowt - startt) / float(CLOCKS_PER_SEC));
	printf("\n");

	volatile float xRnd;

	numIters = 10000000;

	srand(clock());
	startt = clock();
	for (int n = 0; n < numIters; n++) {
		xRnd = rnd();
	}
	nowt = clock();
	printf("Time to do %d calls to rnd(): %.3f seconds.\n",
		numIters, float(nowt - startt) / float(CLOCKS_PER_SEC));

	nseed(clock());
	startt = clock();
	for (int n = 0; n < numIters; n++) {
		xRnd = nrand();
	}
	nowt = clock();
	printf("Time to do %d calls to nrand(): %.3f seconds.\n",
		numIters, float(nowt - startt) / float(CLOCKS_PER_SEC));
	*/
}

void TraceWnd::onChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	switch (toupper(nChar))
	{
		case 'T' :
			// FIXME!
			mCamControl->mUseTrackballControl = !mCamControl->mUseTrackballControl;
			mCamControl->mLookAt.assign(0,0,0);
			break;

		// 'F1', 'T', 'L' are occupied...
		case 'W' : mDrawRealtime = !mDrawRealtime; break;
		/*
		case 'E' :
			if (mNumSceneObjs == (int)mSceneObjects.size())
				mNumSceneObjs -= 5;
			else
				mNumSceneObjs = (int)mSceneObjects.size();
			break;
		*/
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
		/*
		case '+' :
			if (mMaxRecursionLevel < 5) mMaxRecursionLevel++;
			else mMaxRecursionLevel += 5;
			break;
		case '-' :
			if (mMaxRecursionLevel > 5) mMaxRecursionLevel -= 5;
			else if (mMaxRecursionLevel > 0) mMaxRecursionLevel--;
			break;
		*/
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
		/*
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
		*/
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
			//prepareTexture();
			SetRenderThreadRedraw(true);
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

void TraceWnd::createMenus()
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

bool TraceWnd::onCommand(UINT id, UINT ctrlNotifyCode, LONG_PTR pCtrl)
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
