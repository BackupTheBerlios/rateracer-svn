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
		BmpControl()
		{
			//mTimer = new Timers::Timer();
			mTimer = new Windows::Forms::Timer();

			mPixels = NULL;
			mBMI = NULL;

			mBitmap = NULL;
		}

		void Dispose(bool disposing)
		{
			//Console::WriteLine(S"Dispose({0}) called.", (b ? S"true" : S"false"));

			Control::Dispose(disposing); // HWND is sometimes destroyed here!

			if (disposing) {
				//GC::SuppressFinalize(this);
			}
		}

		void createBitmap();
		void destroyBitmap();

		__property Windows::Forms::CreateParams * get_CreateParams();

		void OnHandleCreated(EventArgs* e);
		void OnHandleDestroyed(EventArgs* e);

		//void OnNotifyMessage(Message m);

		void OnPaintBackground(PaintEventArgs* e);
		void OnPaint(PaintEventArgs* e);

		void OnResize(EventArgs* e);
		void OnGotFocus(EventArgs* e);

		void TimerEventProcessor(Object *myObject, EventArgs *myEventArgs);

	protected:
		//Timers::Timer* mTimer;
		Windows::Forms::Timer* mTimer;

		Bitmap* mBitmap;

		BYTE * mPixels;
		BITMAPINFO* mBMI;
		HBITMAP mHBITMAP;
	};

} // namespace