#pragma once
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GLU32.lib")

public __gc class GLContext
{
public:
  GLContext();
  virtual ~GLContext();

	bool create(HWND hWnd);
	bool destroy();

	bool isCreated() { return mHDc != NULL && mHGlrc != NULL; }

	void activate();
	void swapGLBuffers();
	void deactivate();

protected:
	HDC   mHDc;
	HGLRC mHGlrc;
};