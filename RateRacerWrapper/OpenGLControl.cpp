#include "OpenGLControl.h"

namespace Gweronimo
{
	Windows::Forms::CreateParams* OpenGLControl::get_CreateParams()
	{
		Windows::Forms::CreateParams * pParams = Control::get_CreateParams();

		pParams->ClassStyle |= CS_OWNDC;
		//pParams->Style |= WS_OVERLAPPEDWINDOW; // WS_POPUP
		//pParams->ExStyle |= WS_EX_NOPARENTNOTIFY; // WS_EX_APPWINDOW WS_EX_TOOLWINDOW;
	      
		return pParams;
	}

	void OpenGLControl::OnHandleCreated(EventArgs* e)
	{
		Control::OnHandleCreated(e);

		//this->SetStyle(ControlStyles::EnableNotifyMessage, true);

		mGLContext->create( (HWND)get_Handle().ToPointer() );

		mTimer->Tick += new EventHandler( this, TimerEventProcessor );
		mTimer->Interval = 100;
		//mTimer->Start();

		//Console::WriteLine(S"Handle created: {0}", get_Handle().ToInt32().ToString());

		InitGL();
	}

	void OpenGLControl::OnHandleDestroyed(EventArgs* e)
	{
		FinishGL();

		//Console::WriteLine(S"Handle destroyed: {0}", get_Handle().ToInt32().ToString());

		mTimer->Stop();

		if (mGLContext != NULL && mGLContext->isCreated()) {
			mGLContext->destroy();
		}

		Control::OnHandleDestroyed(e);
	}

	/*void OpenGLControl::OnNotifyMessage(Message m) {
		//if (m.Msg == WM_USER_MYMESSAGE) ...
	}*/

	void OpenGLControl::TimerEventProcessor(Object* /*myObject*/, EventArgs* /*myEventArgs*/)
	{
		this->Invalidate(false);
	}

	void OpenGLControl::OnPaintBackground(PaintEventArgs* /*e*/)
	{
		// Do nothing...
		//Control::OnPaintBackground(e);
	}

	void OpenGLControl::OnPaint(PaintEventArgs* e)
	{
		//Control::OnPaint(e);

		if (mGLContext->isCreated())
		{
			mGLContext->activate();
			glViewport(0, 0, this->Width, this->Height);

			PaintGL();

			mGLContext->swapGLBuffers();
			mGLContext->deactivate();
		}
		else {
			SolidBrush *b = new SolidBrush( Color::Black );
			e->Graphics->FillRectangle(b, this->ClientRectangle);
		}
	}

	void OpenGLControl::OnResize(EventArgs* e)
	{
		//Console::WriteLine(S"OpenGLControl got resize: {0} x {1}",
		//	this->Width.ToString(), this->Height.ToString());

		Control::OnResize(e);
	}

	void OpenGLControl::OnGotFocus(EventArgs* e)
	{
		//Console::WriteLine("OpenGLControl got focus!");

		Control::OnGotFocus(e);
	}

	/*
	void OpenGLControl::ClearViewport()
	{
		GLint dims[4];
		glGetIntegerv(GL_VIEWPORT, dims);

		// FIXME inefficient if using many small viewports!
		glScissor(dims[0], dims[1], dims[2], dims[3]);
		glEnable(GL_SCISSOR_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glClear(GL_STENCIL_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);
	}*/

	void OpenGLControl::InitGL()
	{
	}

	void OpenGLControl::FinishGL()
	{
	}

	void OpenGLControl::PaintGL()
	{
		glClearColor(0,0,0,0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

} // namespace
