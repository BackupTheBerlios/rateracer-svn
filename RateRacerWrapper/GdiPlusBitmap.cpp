#include "GdiPlusBitmap.h"

#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus")

#include <stdio.h>

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

  int x = 0;
  int y = 0;
	int imgWidth  = (int)(zoom * (float)width);
	int imgHeight = (int)(zoom * (float)height);

  // Compensate for DrawImage() scaling imperfection...
  if (zoom > 1.0f) {
    imgWidth  += (int)(0.5f*zoom);
    imgHeight += (int)(0.5f*zoom);
  }

  /*
  RectF boundRect; graphics.GetVisibleClipBounds(&boundRect);
  printf("%f x %f\n", boundRect.Width, boundRect.Height);
  if (boundRect.Width  > imgWidth)  x = (boundRect.Width  - imgWidth) / 2;
  if (boundRect.Height > imgHeight) y = (boundRect.Height - imgHeight) / 2;
  */

	graphics.DrawImage(&bitmap, x, y, imgWidth, imgHeight);

  //Rect destRect;
  //int srcX,srcY,srcW,srcH;
  //graphics.DrawImage(&bitmap, destRect, srcX,srcY,srcW,srcH, UnitPixel);
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   ImageCodecInfo* pImageCodecInfo = NULL;

   GetImageEncodersSize(&num, &size);
   if(size == 0) return -1; // Failure

   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL) return -1; // Failure

   GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }    
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}

void GdiPlusBitmap::saveGdiPlusBitmap(const WCHAR* filename,
                                      int width, int height, UINT* pixels )
{
	Bitmap bitmap(width, height, 4*width,
                PixelFormat32bppARGB, (BYTE*)pixels);

  CLSID pngClsid;
  if (GetEncoderClsid(L"image/png", &pngClsid) != -1)
  {
    bitmap.Save(filename, &pngClsid, NULL);
  }
}
