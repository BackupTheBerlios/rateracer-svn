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

#include "GLContext.h"

namespace Gweronimo
{
	//public __delegate String* TestOccuredEventHandler(int, String*);
	//public __delegate void VoidEventHandler( void );

	public __gc class OpenGLControl : public Control, public IDisposable
	{
	public:
		__event EventHandler* ExerciseStopped;
		//__event TestOccuredEventHandler* TestOccured;
		/*void performTestOccured()	{
			String* str = TestOccured(42, "fortytwo");
			Console::WriteLine("Event test successful: {0}", str);
		}*/

	public:
		OpenGLControl()
		{
			//mTimer = new Timers::Timer();
			mTimer = new Windows::Forms::Timer();

			mGLContext = new GLContext();
		}

		void Dispose(bool disposing)
		{
			//Console::WriteLine(S"Dispose({0}) called.", (b ? S"true" : S"false"));

			Control::Dispose(disposing); // HWND is sometimes destroyed here!

			//mGLContext->Dispose();
			delete mGLContext;
			mGLContext = NULL;

			if (disposing) {
				//GC::SuppressFinalize(this);
			}
		}

		__property Windows::Forms::CreateParams * get_CreateParams();

		void OnHandleCreated(EventArgs* e);
		void OnHandleDestroyed(EventArgs* e);

		//void OnNotifyMessage(Message m);

		void OnPaintBackground(PaintEventArgs* e);
		void OnPaint(PaintEventArgs* e);

		void OnResize(EventArgs* e);
		void OnGotFocus(EventArgs* e);

		void TimerEventProcessor(Object *myObject, EventArgs *myEventArgs);

		//void clearViewport(float r, float g, float b);

		virtual void InitGL();
		virtual void FinishGL();

		virtual void PaintGL();

	protected:
		//Timers::Timer* mTimer;
		Windows::Forms::Timer* mTimer;

		GLContext* mGLContext;
	};

} // namespace
