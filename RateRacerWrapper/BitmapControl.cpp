#include <memory.h>
#include <math.h>

#include "BitmapControl.h"
#include "RateRacerCore.h"

#include "GdiPlusBitmap.h"

#include "rateracerlib/RayEngine.h"

using namespace System::Drawing::Imaging;
using namespace System::Drawing::Drawing2D;

using namespace RateRacerCore;

namespace Gweronimo
{
	Windows::Forms::CreateParams* BmpControl::get_CreateParams()
	{
		Windows::Forms::CreateParams * pParams = Control::get_CreateParams();

		pParams->ClassStyle |= CS_OWNDC;
		//pParams->Style |= WS_OVERLAPPEDWINDOW; // WS_POPUP
		//pParams->ExStyle |= WS_EX_NOPARENTNOTIFY; // WS_EX_APPWINDOW WS_EX_TOOLWINDOW;
	      
		return pParams;
	}

	BmpControl::BmpControl()
	{
		//mTimer = new Timers::Timer();
		//mTimer = new Windows::Forms::Timer();

		mZoom = 1;
		mInterpolate = false;
	}

	void BmpControl::Dispose(bool disposing)
	{
		//Console::WriteLine(S"Dispose({0}) called.", (b ? S"true" : S"false"));

		Control::Dispose(disposing); // HWND is sometimes destroyed here!

		if (disposing) {
			//GC::SuppressFinalize(this);
		}
	}

	void BmpControl::OnHandleCreated(EventArgs* e)
	{
		Control::OnHandleCreated(e);

		this->SetStyle(ControlStyles::EnableNotifyMessage, true);

		this->SetStyle(ControlStyles::ResizeRedraw, true);

		this->SetStyle(ControlStyles::DoubleBuffer, true); 
		this->SetStyle(ControlStyles::UserPaint, true);
		this->SetStyle(ControlStyles::AllPaintingInWmPaint, true);

		this->UpdateStyles();

		//mTimer->Tick += new EventHandler( this, TimerEventProcessor );
		//mTimer->Interval = 1000;

		RateRacerEngine::mImagePlane->setListenerWindow(
			(HWND)get_Handle().ToPointer());

		//Console::WriteLine(S"Handle created: {0}", get_Handle().ToInt32().ToString());
	}

	void BmpControl::OnHandleDestroyed(EventArgs* e)
	{
		//Console::WriteLine(S"Handle destroyed: {0}", get_Handle().ToInt32().ToString());

		RateRacerEngine::mImagePlane->clearListenerWindow();
		RateRacerEngine::mImagePlane->RequestRenderThreadStop();

		//mTimer->Stop();

		Control::OnHandleDestroyed(e);
	}

	void BmpControl::OnMouseEnter(EventArgs* e)
	{
		Focus();
		Control::OnMouseEnter(e);
	}

	void BmpControl::OnMouseLeave(EventArgs* e)
	{
		Control::OnMouseLeave(e);
	}

	void BmpControl::OnNotifyMessage(Message m)
	{
		if (m.Msg == WM_USER_POSTPROC_DONE) {
			//updateBitmap();
      //Refresh();
			PostProcessingDone(this, EventArgs::Empty);
		}
	}

  void BmpControl::saveBitmap(String *filename)
  {
    WCHAR* cstr = (WCHAR*)Marshal::StringToHGlobalUni(filename).ToPointer();

    updateBitmap();
    GdiPlusBitmap::saveGdiPlusBitmap(cstr,
      RateRacerEngine::mImagePlane->mRenderWidth,
      RateRacerEngine::mImagePlane->mRenderHeight,
      RateRacerEngine::mImagePlane->mBitmapPixels);

    Marshal::FreeHGlobal(cstr);
  }

	/*
	void BmpControl::createBitmap()
	{
		//mBitmap = new Bitmap(100,100,PixelFormat::Format32bppArgb);

		mBMI = new BITMAPINFO;
		memset(mBMI, 0, sizeof(BITMAPINFO));
    mBMI->bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    mBMI->bmiHeader.biWidth       =  100;
    mBMI->bmiHeader.biHeight      =  100; // top-down image
    mBMI->bmiHeader.biPlanes      = 1;
    mBMI->bmiHeader.biBitCount    = 0;
    mBMI->bmiHeader.biCompression = BI_RGB;
    //mBMI->bmiHeader.biSizeImage = 0;

		BYTE* __pin * ppPixels = &mPixels;

		mHBITMAP = CreateDIBSection(
			(HDC)::GetDC((HWND)this->Handle.ToPointer()),
			mBMI,
			DIB_RGB_COLORS,
			(void**)ppPixels,
			NULL, 0
		);

		//mPixels = new BYTE[100*100*3];
		//memset(mPixels, 0, 100*100*3);
	}

	void BmpControl::destroyBitmap()
	{
		delete mPixels;
		delete mBMI;
	}*/

	void BmpControl::setResolution(System::Drawing::Size* renderSize)
	{
		RateRacerEngine::mImagePlane->prepareImage(
			renderSize->Width, renderSize->Height);
	}

	void BmpControl::setZoom(float zoom)
	{
		mZoom = zoom;
	}

	void BmpControl::setInterpolation(bool interpolate)
	{
		mInterpolate = interpolate;
	}


	/*
	void BmpControl::startRender()
	{
		//Console::WriteLine(S"Render started");
		mTimer->Start();
	}
	*/

	void BmpControl::updateBitmap()
	{
		RateRacerEngine::mImagePlane->updateBitmapPixels();
		//this->Invalidate(false);
	}

	int BmpControl::renderingPercentage()
	{
		return RateRacerEngine::mImagePlane->renderingPercentage();
	}

	float BmpControl::renderingTimeSeconds()
	{
		return RateRacerEngine::mImagePlane->renderingTimeSeconds();
	}

	/*
	void BmpControl::TimerEventProcessor(Object* myObject, EventArgs* myEventArgs)
	{
		updateBitmap();
		//Console::WriteLine( RateRacerEngine::mImagePlane->renderingPercentage() );
		if (RateRacerEngine::mImagePlane->renderingPercentage() == 100)
		{
			mTimer->Stop();
		}
	}
	*/

	void BmpControl::OnPaintBackground(PaintEventArgs* e)
	{
		// Do nothing...
		//Control::OnPaintBackground(e);
	}

	void BmpControl::OnPaint(PaintEventArgs* e)
	{
		//Control::OnPaint(e);

		//SolidBrush *b = new SolidBrush( Color::Red );
    HatchBrush *b = new HatchBrush( //RateRacerEngine::mRenderProps->mHatch,
      HatchStyle::SolidDiamond,
      //RateRacerEngine::mRenderProps->mColor,
      Color::LightCoral,
      Color::White
    );
		e->Graphics->FillRectangle(b, this->ClientRectangle);

		//if (mPixels != NULL)
		//if (mBitmap != NULL)
		{
			//Console::WriteLine(S"Painting...");

			IntPtr hdc = e->Graphics->GetHdc();
			HDC realHDC = (HDC)hdc.ToPointer();
			//realHDC = (HDC)::GetDC((HWND)this->get_Handle().ToPointer());
			//Ellipse(realHDC, 0,0, Width-1, Height-1);
			//::StretchDIBits(realHDC, 0,0, Width, Height,
			//								0,0, 100,100, mPixels, mBMI, DIB_RGB_COLORS, SRCCOPY);

      //Console::WriteLine(e->Graphics->ClipBounds.ToString());

			GdiPlusBitmap::displayGdiPlusBitmap(
				realHDC, mZoom, mInterpolate,
				RateRacerEngine::mImagePlane->mRenderWidth,
				RateRacerEngine::mImagePlane->mRenderHeight,
				RateRacerEngine::mImagePlane->mBitmapPixels);

			e->Graphics->ReleaseHdc(hdc);
/*
#if 0
			BitmapData* bmd = mBitmap->LockBits(
				System::Drawing::Rectangle(0,0,mBitmap->Width, mBitmap->Height),
				//System::Drawing::Rectangle(10,10, 1,1),
				ImageLockMode::WriteOnly,
				//ImageLockMode::ReadWrite,
				PixelFormat::Format32bppArgb);

			UINT* pixels = (UINT*)bmd->Scan0.ToPointer();

//			pixels[0001] = 0xFF000000;
//			pixels[5050] = 0xFFFF0000;
//			pixels[5051] = 0xFF00FF00;
//			pixels[5052] = 0xFF0000FF;
//			pixels[9999] = 0xFF000000;
//
//			static int n = 0;
//			pixels[n] = 0xFF000000;
//			n = (n + 1) % 10000;

//			static bool b = true;
//			static int v = 0;
//			for (int n = 0; n < 10000; n++) {
//				//pixels[n] = b ? 0xFF000000 : 0xFFFFFFFF;
//				pixels[n] = 0xFF000000 + v*0x10000;
//			}
//			b = !b;
//			v = (v + 5) % 256;
			for (int x = 0; x < 100; x++) {
				for (int y = 0; y < 100; y++) {
					float value = 0.5f * (1 + sinf(0.1f * (x*x + y*y)));
					int val = int(255.0f * value);
					pixels[y*100 + x] = 0xFF000000 + (val << 16) + (val << 8) + val;
				}
			}
			mBitmap->UnlockBits(bmd);

			e->Graphics->InterpolationMode = InterpolationMode::Bicubic;//NearestNeighbor;
			e->Graphics->DrawImage(mBitmap, 0, 0, 201, 201);
#endif
*/
		}
	}

	/*
	public struct PixelData
	{
			public byte alpha;
			public byte b;
			public byte g;
			public byte r;
	}

	public PixelData* PixelAt(int x, int y)
	{
    return (PixelData*) (pBase + y * width + x * sizeof(PixelData));
	}
	*/

	void BmpControl::OnResize(EventArgs* e)
	{
		//Console::WriteLine(S"BmpControl got resize: {0} x {1}",
		//	this->Width.ToString(), this->Height.ToString());

		Control::OnResize(e);
	}

	void BmpControl::OnGotFocus(EventArgs* e)
	{
		//Console::WriteLine("BmpControl got focus!");

		Control::OnGotFocus(e);
	}

} // namespace
