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

#include "OpenGLControl.h"

#include "rateracerlib/CameraControl.h"

#include "rateracerlib/rateracerlib.h"
#include "rateracerlib/Camera.h"
#include "rateracerlib/RayEngine.h"

namespace Gweronimo
{
	public __gc class GLPreviewControl : public OpenGLControl
	{
	public:
		GLPreviewControl();

		void Dispose(bool disposing);

		void OnMouseDown(MouseEventArgs* e);
		void OnMouseUp(MouseEventArgs* e);
		void OnMouseWheel(MouseEventArgs* e);
		void OnMouseMove(MouseEventArgs* e);

		void OnMouseEnter(EventArgs* e);
		void OnMouseLeave(EventArgs* e);

		//void OnNotifyMessage(Message m);

		//void OnPaint(PaintEventArgs* e);

		//void OnResize(EventArgs* e);
		//void OnGotFocus(EventArgs* e);

		void InitGL();
		void FinishGL();

		void PaintGL();

	protected:
		Point mMousePos;

		//CameraControl* mCamControl;
		//RayEngine *mRayEngine;
	};

} // namespace
