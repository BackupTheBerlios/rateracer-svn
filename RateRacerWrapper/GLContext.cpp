#include "GLContext.h"

GLContext::GLContext()
{
	mHDc = NULL;
	mHGlrc = NULL;
}

GLContext::~GLContext()
{
}

bool GLContext::create(HWND hWnd)
{
	DWORD errorCode;
	int pixelFormat;

	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,                     // version number 
		PFD_DRAW_TO_WINDOW |   // support window 
		PFD_SUPPORT_OPENGL |   // support OpenGL    
		PFD_DOUBLEBUFFER,      // double buffered 
		PFD_TYPE_RGBA,         // RGBA type 
		32,                    // 24-bit color depth 
		0,0, 0,0, 0,0,         // color bits ignored 
		8,0,                   // alpha bits & shift
		0,                     // no accumulation buffer 
		0, 0, 0, 0,            // accum bits ignored 
		32,                    // z-buffer bitplanes
		8,                     // stencil buffer bitplanes
		0,                     // no auxiliary buffer 
		PFD_MAIN_PLANE,        // main layer 
		0,                     // reserved 
		0, 0, 0                // layer masks ignored 
	};

	mHDc = ::GetDC( hWnd );
	if (mHDc == NULL) {
		//("Extracing device context: %s", getSysErrMessage());
		return false;
	}

	pixelFormat = ::ChoosePixelFormat(mHDc, &pfd);
	if (pixelFormat == 0) {
		//("Obtaining pixelformat: %s", getSysErrMessage());
		return false;
	}

	errorCode = ::SetPixelFormat(mHDc, pixelFormat, &pfd);
	if (errorCode == 0) {
		//("Setting device context pixelformat: %s", getSysErrMessage());
		return false;
	}

	mHGlrc = wglCreateContext(mHDc);
	if (mHGlrc == NULL) {
		//("Creating gl context: %s", getSysErrMessage());
		return false;
	}

	// NOTE: wglMakeCurrent must be run in the thread that does GL calls!!!
	// That's because each thread can have it's own current context...
	return true;
}

bool GLContext::destroy()
{
	BOOL ret = wglDeleteContext( mHGlrc );
	if (!ret) {
		//("wglDeleteContext(): %s", getSysErrMessage());
		return false;
	}
	mHGlrc = NULL;

	// FIXME: ReleaseDC() not needed since CS_OWNDC means DC is private?
	//::ReleaseDC( mHWnd, mHDc );
	mHDc = NULL;

	return true;
}

void GLContext::activate()
{
	wglMakeCurrent(mHDc, mHGlrc);
}

void GLContext::deactivate()
{
	wglMakeCurrent(mHDc, NULL);
}

void GLContext::swapGLBuffers()
{
	SwapBuffers( mHDc );
}
