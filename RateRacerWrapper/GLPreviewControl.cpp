#include "GLPreviewControl.h"

#include "RateRacerCore.h"

#include "rateracerlib/DebugDraw.h"

using namespace RateRacerCore;

namespace Gweronimo
{
	GLPreviewControl::GLPreviewControl()
	{
		//mCamControl = new CameraControl();
		//mRayEngine = new RayEngine();
		//mRayEngine->init();

		// TODO: setup camera in scene...
		RateRacerEngine::mCamControl->mLookDistance  = 7;//50.0f;
		RateRacerEngine::mCamControl->mLookAzimuth   = 120;
		RateRacerEngine::mCamControl->mLookElevation = -30;
		RateRacerEngine::mCamControl->mLookAt.assign(0,0,0);
		//RateRacerEngine::mCamControl->mUseTrackballControl = false;
		RateRacerEngine::mCamControl->mFOV = 60;
	}

	void GLPreviewControl::Dispose(bool disposing)
	{
		//Console::WriteLine(S"Dispose({0}) called.", (b ? S"true" : S"false"));

		OpenGLControl::Dispose(disposing); // HWND is sometimes destroyed here!

		//mRayEngine->shutdown();
		//delete mRayEngine;
		//delete mCamControl;

		if (disposing) {
			//GC::SuppressFinalize(this);
		}
	}

	void GLPreviewControl::OnMouseEnter(EventArgs* e)
	{
		Enabled = true;
		Focus();

		OpenGLControl::OnMouseEnter(e);
	}

	void GLPreviewControl::OnMouseLeave(EventArgs* e)
	{
		OpenGLControl::OnMouseLeave(e);
	}

	void GLPreviewControl::OnMouseDown(MouseEventArgs* e)
	{
		while (ShowCursor(FALSE) >= 0);
		//SetCapture( (HWND)this->Handle.ToPointer() );
		Capture = true;
		ShowCursor(FALSE);
		//GetCursorPos(&mMousePos);
		mMousePos = MousePosition;
		SetCursorPos(320, 240);

		OpenGLControl::OnMouseDown(e);
	}

	void GLPreviewControl::OnMouseUp(MouseEventArgs* e)
	{
		SetCursorPos(mMousePos.X, mMousePos.Y);
		//ReleaseCapture();
		Capture = false;
		while (ShowCursor(TRUE) < 0);

		OpenGLControl::OnMouseUp(e);
	}

	void GLPreviewControl::OnMouseWheel(MouseEventArgs* e)
	{
		if (e->Delta < 0) {
			RateRacerEngine::mCamControl->zoomInOut(0.9f);
		} else {
			RateRacerEngine::mCamControl->zoomInOut(1.1f);
		}

		Invalidate();

		OpenGLControl::OnMouseWheel(e);
	}

	void GLPreviewControl::OnMouseMove(MouseEventArgs* e)
	{
		static POINTS oldp = {320, 240};
		static POINT newp;

		if (Capture)
		{
			GetCursorPos(&newp);
			SetCursorPos(320, 240);

			int dx = newp.x - oldp.x;
			int dy = newp.y - oldp.y;

			if (e->Button & MouseButtons::Left) {
				RateRacerEngine::mCamControl->moveInOut(0.1f * dy);
			}
			else if (e->Button & MouseButtons::Middle) {
				RateRacerEngine::mCamControl->translate(0.1f * dx, -0.1f * dy);
			}
			else if (e->Button & MouseButtons::Right) {
				RateRacerEngine::mCamControl->rotate(-0.1f * dx, -0.1f * dy);
			}

			//Invalidate();
			//Update();
			Refresh();
		}

		OpenGLControl::OnMouseMove(e);
	}

  /*
  void ::onChar(UINT nChar, UINT nRepCnt, UINT nFlags)
  {
    // TODO: Add your message handler code here and/or call default
    switch (toupper(nChar))
    {
      case 'T' :
      mCamControl->mUseTrackballControl = !mCamControl->mUseTrackballControl;
      mCamControl->mLookAt.assign(0,0,0);
      break;
    }
  }
  */

	void GLPreviewControl::InitGL()
	{
	}

	void GLPreviewControl::FinishGL()
	{
	}

	void GLPreviewControl::PaintGL()
	{
		OpenGLControl::PaintGL();

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		RateRacerEngine::mCamControl->SetGLPerspProjection();

		RateRacerEngine::mCamControl->keepElevationAboveGroundPlane();
		RateRacerEngine::mCamControl->setView();

		drawWorldAxes();

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);

		glDisable(GL_LIGHTING);

		// Draw crosshairs
		Matrix4 view = RateRacerEngine::mCamControl->getViewTransform();
		glColor3f(0,1,1);
		glBegin(GL_LINES);
			glVertex3fv(&view.transform(Vec3(-0.1f,0,-1))[0]);
			glVertex3fv(&view.transform(Vec3( 0.1f,0,-1))[0]);
			glVertex3fv(&view.transform(Vec3(0,-0.1f,-1))[0]);
			glVertex3fv(&view.transform(Vec3(0, 0.1f,-1))[0]);
		glEnd();

		RateRacerEngine::mRayEngine->dbgDrawStoredRays();
		//RateRacerEngine::mRayEngine->dbgDrawGrid();

		RateRacerEngine::mRayEngine->drawScenePreview();
	}

} // namespace