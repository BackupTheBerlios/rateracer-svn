#pragma once

#include <windows.h>

class GdiPlusBitmap
{
public:
	static void displayGdiPlusBitmap( HDC hdc, float zoom, bool smooth,
																		int width, int height, UINT* pixels );

  static void saveGdiPlusBitmap( const WCHAR* filename,
                                 int width, int height, UINT* pixels );
};
