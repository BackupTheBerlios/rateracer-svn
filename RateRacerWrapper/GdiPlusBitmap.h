#pragma once

#include <windows.h>

class GdiPlusBitmap
{
public:
	static void displayGdiPlusBitmap( HDC hdc, float zoom, bool smooth,
																		int width, int height, UINT* pixels );

};
