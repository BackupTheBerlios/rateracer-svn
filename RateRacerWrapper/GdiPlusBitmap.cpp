#include "GdiPlusBitmap.h"

#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus")

void GdiPlusBitmap::displayGdiPlusBitmap( HDC hdc, float zoom, bool smooth,
																				  int width, int height, UINT* pixels )
{
	Graphics graphics(hdc);

	BitmapData bitmapData;
	bitmapData.Width  = width;
	bitmapData.Height = height;
	bitmapData.Stride = 4*bitmapData.Width;
	bitmapData.PixelFormat = PixelFormat32bppARGB; 
	bitmapData.Scan0 = (VOID*)pixels;
	bitmapData.Reserved = NULL;

	// Create a Bitmap object from a BMP file.
	//Bitmap bitmap(L"LockBitsTest2.bmp");
	// Display the bitmap before locking and altering it.
	//graphics.DrawImage(&bitmap, 10, 10);

	Bitmap bitmap(width, height, &graphics);

	// Lock a rectangular portion of the bitmap for writing.
	Rect rect(0, 0, width, height);
	bitmap.LockBits(
		&rect,
		ImageLockModeWrite | ImageLockModeUserInputBuf,
		PixelFormat32bppARGB,
		&bitmapData);

	// Commit the changes and unlock the portion of the bitmap.  
	bitmap.UnlockBits(&bitmapData);

	// Display the altered bitmap.
	if (smooth)
		graphics.SetInterpolationMode(InterpolationModeBicubic);
	else
		graphics.SetInterpolationMode(InterpolationModeNearestNeighbor); 

	int imgWidth  = (int)(zoom * (float)width)  + (int)zoom;
	int imgHeight = (int)(zoom * (float)height) + (int)zoom;
	graphics.DrawImage(&bitmap, 0, 0, imgWidth, imgHeight);
}
