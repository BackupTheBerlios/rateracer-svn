#pragma once

#include <windows.h>

#using <mscorlib.dll>
#using <system.dll>
#using <system.drawing.dll>
#using <system.design.dll>
#using <system.windows.forms.dll>

using namespace System;
using namespace System::Drawing;
using namespace System::Windows::Forms;

using namespace System::Runtime::InteropServices;
using namespace System::Runtime::Remoting::Messaging;

using namespace System::Collections;

namespace Gweronimo
{
	public __gc class BmpControl : public Control, public IDisposable
	{
	public:
		__event EventHandler* PostProcessingDone;

		BmpControl();
		void Dispose(bool disposing);

		//void createBitmap();
		//void destroyBitmap();

		void setResolution(System::Drawing::Size* renderSize);
		void setZoom(float zoom);
		void setInterpolation(bool interpolate);

		//void startRender();
		void updateBitmap();

    void saveBitmap(String *filename);

		int renderingPercentage();
		float renderingTimeSeconds();

		__property Windows::Forms::CreateParams * get_CreateParams();

		void OnHandleCreated(EventArgs* e);
		void OnHandleDestroyed(EventArgs* e);

		void OnMouseEnter(EventArgs* e);
		void OnMouseLeave(EventArgs* e);

		void OnNotifyMessage(Message m);

		void OnPaintBackground(PaintEventArgs* e);
		void OnPaint(PaintEventArgs* e);

		void OnResize(EventArgs* e);
		void OnGotFocus(EventArgs* e);

		//void TimerEventProcessor(Object *myObject, EventArgs *myEventArgs);

	protected:
		//Timers::Timer* mTimer;
		//Windows::Forms::Timer* mTimer;

		float mZoom;
		bool mInterpolate;

		//Bitmap* mBitmap;

		//BYTE * mPixels;
		//BITMAPINFO* mBMI;
		//HBITMAP mHBITMAP;
	};

} // namespace
