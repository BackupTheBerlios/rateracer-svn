#pragma once

#include <math.h>

#include "MathConstants.h"

#define MATH_INLINE __forceinline

MATH_INLINE float rad2deg(float f)
{
	return f * MathConstants::RADTODEG;
}

MATH_INLINE float deg2rad(float f)
{
	return f * MathConstants::DEGTORAD;
}

MATH_INLINE bool isZero(float f)
{
  return (fabsf(f) <= MathConstants::EPSILON);
}

MATH_INLINE bool fuzzyEQ(float f, float g)
{
  //return (f <= g) ? (f >= g - epsilon) : (f <= g + epsilon);
	return (fabsf( f - g ) <= MathConstants::EPSILON);
}

MATH_INLINE float max2(float f, float g)
{
  return (f > g) ? f : g;
}

MATH_INLINE float min2(float f, float g)
{
  return (f < g) ? f : g;
}

MATH_INLINE float max3(float f, float g, float h)
{
  return (f > g) ?
		((f > h) ? f : h) : 
		((g > h) ? g : h);
}

MATH_INLINE float min3(float f, float g, float h)
{
  return (f < g) ? 
		((f < h) ? f : h) : 
		((g < h) ? g : h);
}

MATH_INLINE void swap(float& f, float& g)
{
  float gmTmp = f; f = g; g = gmTmp;
}

MATH_INLINE void swap(int& i, int& j)
{
  int gmTmp = i; i = j; j = gmTmp;
}

MATH_INLINE void clamp(float &f, float l, float h)
{
  if (f < l) f = l;
  if (f > h) f = h;
}

MATH_INLINE void clamp(int &f, int l, int h)
{
  if (f < l) f = l;
  if (f > h) f = h;
}

MATH_INLINE float square(float f)
{
  return f * f;
}

/*
MATH_INLINE float cube(float f)
{
  return f * f * f;
}

MATH_INLINE float round(float f)
{
  return (f >= 0) ? float(int(f + 0.5)) : float(- int(0.5 - f));
}

MATH_INLINE float sign(float f)
{
  return ((f < 0) ? -1.0f : 1.0f);
}

MATH_INLINE float lerp(float f, float l, float h)
{
  return l + ((h - l) * f );
}

MATH_INLINE float smooth(float f)
{
  return ((3.0f - 2.0f * f) * f * f);
}

MATH_INLINE float slide(float f, float l, float h)
{
  return (f < 0) ? l : (f > 1) ? h : lerp(gmSmooth(f), l, h);
}
*/