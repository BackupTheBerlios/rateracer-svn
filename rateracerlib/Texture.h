#pragma once

#include "Ray.h"

class Texture
{
public:
  Texture()
  {
    width = height = 0;
    pixels = NULL;
    backColor.assign(0,0,0);
  }

  virtual ~Texture()
  {
    delete pixels;
  }

  void createFromBitmapPixels(int w, int h, UINT *bitmapPixels)
  {
    if (bitmapPixels == NULL) return;

    width = w; height = h;
    if (pixels != NULL) delete pixels;
    pixels = new Vec3[width * height];

    byte *rgbaBytes = (byte*)bitmapPixels;
    // FIXME store 3-byte pixels to save memory?
    const float f = 1.0f / 255.0f;
    float r, g, b;

    int idx = 0;
    for(int y = height-1; y >= 0; y--) {
      for(int x = 0; x < width; x++) {
        //byte alpha = (b > 250) ? 0 : 255; // 0xFF000000
        b = f * float(rgbaBytes[idx++]);
        g = f * float(rgbaBytes[idx++]);
        r = f * float(rgbaBytes[idx++]);
        idx++; // alpha
        pixels[x + y*width].assign(r,g,b);
      }
    }
    //printf("%06d: %f %f %f\n", idx, r,g,b);
  }

  virtual Vec3 getColor(const Vec3& p, const Vec2& uv) const
  {
    if (pixels != NULL) {
      //byte* pixels = (byte*)texture->getPixels();
      int xi = int(uv[0] * width) % width;
      int yi = int(uv[1] * height) % height;
      clamp(xi, 0, width-1);
      clamp(yi, 0, height-1);
      int idx = int(xi + yi*width); // * 3
      //const float f = 1.0f / 255.0f;
      Vec3 c = pixels[idx];
      //c[0] = f * float(pixels[idx++]);
      //c[1] = f * float(pixels[idx++]);
      //c[2] = f * float(pixels[idx++]);
      //printf("Texture lookup: (%f, %f) -> (%d, %d) color %f %f %f\n",
      //  uv[0], uv[1], width, height, c[0],c[1],c[2]);
      return c;
    }
    return backColor;
  }

  int width, height;
  Vec3 *pixels;

  Vec3 backColor;
};