#pragma once

#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "MathUtils.h"

class Vec2
{
protected:
  float v_[2];

public:
  Vec2();
  Vec2(const Vec2&);
  Vec2(float, float);

  // array access
  float& operator [](int);
  const float& operator [](int) const;

  // assignment

  Vec2& assign(float, float);

  void setZero();

  void copyFrom(float [2]);
  void copyTo(float [2]) const;
	
	// operations
	
  Vec2& normalize();
	Vec2& normalizeSafely();
  Vec2& clampVec(float, float);

  // math operators

  Vec2& operator =(const Vec2&);

  Vec2& operator +=(const Vec2&);
  Vec2& operator -=(const Vec2&);
  Vec2& operator *=(float);
  Vec2& operator /=(float);

  Vec2 operator +(const Vec2&) const;
  Vec2 operator -(const Vec2&) const;
  Vec2 operator -() const;
  Vec2 operator *(float) const;
  Vec2 operator /(float) const;

friend Vec2 operator *(float, const Vec2&);

  bool operator ==(const Vec2&) const;
  bool operator !=(const Vec2&) const;

friend float dot(const Vec2&, const Vec2&);
	
	float length() const;
	float lengthSquared() const;
	float lengthManhattan() const;

friend float distance(const Vec2&, const Vec2&);
friend float distanceSquared(const Vec2&, const Vec2&);

	// Angle between vectors u and v (radians)
friend float angle( Vec2 &u, Vec2 &v );
  // angle in unit circle relative to x-axis
friend float angleUnitCircle( Vec2 &u );
	// Project vector u onto vector v
friend Vec2 proj_u_on_v(Vec2 &u, Vec2 &v);

	// Linear interpolation between two vectors
friend Vec2 lerp(float, const Vec2&, const Vec2&);

	int minComponent() const;
	int maxComponent() const;

	// Unary, perpendicular dot product defined in Graphics Gems IV...
	Vec2 perpDot() const;

	void print() const; // Debug print to console
};

// CONSTRUCTORS

MATH_INLINE Vec2::Vec2()
{
	v_[0] = v_[1] = 0;
}

MATH_INLINE Vec2::Vec2(const Vec2& v)
{
  v_[0] = v.v_[0]; v_[1] = v.v_[1];
}

MATH_INLINE Vec2::Vec2(float x, float y)
{
  v_[0] = x; v_[1] = y;
}

// ARRAY ACCESS

MATH_INLINE float& Vec2::operator [](int i) 
{
  assert(i == 0 || i == 1);
  return v_[i];
}

MATH_INLINE const float& Vec2::operator [](int i) const
{
  assert(i == 0 || i == 1);
  return v_[i];
}

// ASSIGNMENT

MATH_INLINE Vec2& Vec2::assign(float x, float y)
{
  v_[0] = x; v_[1] = y;
  return *this;
}

MATH_INLINE Vec2& Vec2::operator =(const Vec2& v)
{
  v_[0] = v.v_[0]; v_[1] = v.v_[1];
  return *this;
}

MATH_INLINE void Vec2::setZero()
{
  v_[0] = v_[1] = 0.0f;
}

// MATH OPERATORS

MATH_INLINE Vec2& Vec2::operator +=(const Vec2& v)
{
  v_[0] += v.v_[0]; v_[1] += v.v_[1];
  return *this;
}

MATH_INLINE Vec2& Vec2::operator -=(const Vec2& v)
{
  v_[0] -= v.v_[0]; v_[1] -= v.v_[1];
  return *this;
}

MATH_INLINE Vec2& Vec2::operator *=(float c)
{
  v_[0] *= c; v_[1] *= c;
  return *this;
}

MATH_INLINE Vec2& Vec2::operator /=(float c)
{
  assert(!isZero(c));
	float ic = 1.0f / c;
  v_[0] *= ic; v_[1] *= ic;
  return *this;
}

MATH_INLINE Vec2 Vec2::operator +(const Vec2& v) const
{
  return Vec2(v_[0] + v.v_[0], v_[1] + v.v_[1]);
}

MATH_INLINE Vec2 Vec2::operator -(const Vec2& v) const
{
  return Vec2(v_[0] - v.v_[0], v_[1] - v.v_[1]);
}

MATH_INLINE Vec2 Vec2::operator -() const
{
  return Vec2(-v_[0], -v_[1]);
}

MATH_INLINE Vec2 Vec2::operator *(float c) const
{
  return Vec2(v_[0] * c, v_[1] * c);
}

MATH_INLINE Vec2 Vec2::operator /(float c) const
{
  assert(!isZero(c));
	float ic = 1.0f / c;
  return Vec2(v_[0] * ic, v_[1] * ic);
}

MATH_INLINE Vec2 operator *(float c, const Vec2& v)
{
  return Vec2(c * v.v_[0], c * v.v_[1]);
}

MATH_INLINE bool Vec2::operator ==(const Vec2& v) const
{
  return (fuzzyEQ(v_[0], v.v_[0]) && fuzzyEQ(v_[1], v.v_[1]));
}

MATH_INLINE bool Vec2::operator !=(const Vec2& v) const
{
  return !(fuzzyEQ(v_[0], v.v_[0]) && fuzzyEQ(v_[1], v.v_[1]));
}

// OPERATIONS

MATH_INLINE Vec2& Vec2::clampVec(float lo, float hi)
{
	clamp(v_[0], lo, hi); clamp(v_[1], lo, hi);
  return *this;
}

MATH_INLINE float Vec2::length() const
{
  return sqrtf(square(v_[0]) + square(v_[1]));
}

MATH_INLINE float Vec2::lengthSquared() const
{
  return square(v_[0]) + square(v_[1]);
}

MATH_INLINE float Vec2::lengthManhattan() const
{
  return fabsf(v_[0]) + fabsf(v_[1]);
}

MATH_INLINE Vec2& Vec2::normalize()
{
  float len = length();
  assert(!isZero(len));
  *this *= 1.0f / len;
  return *this;
}

MATH_INLINE Vec2& Vec2::normalizeSafely()
{
  float len = length();
  if (!isZero(len)) {
		*this *= 1.0f / len;
  }
  return *this;
}

MATH_INLINE void Vec2::copyFrom(float f[2])
{
  v_[0] = f[0]; v_[1] = f[1];
}

MATH_INLINE void Vec2::copyTo(float f[2]) const
{
  f[0] = v_[0]; f[1] = v_[1];
}

MATH_INLINE int Vec2::minComponent() const
{
	return (v_[0] < v_[1]) ? 0 : 1;
}

MATH_INLINE int Vec2::maxComponent() const
{
	return (v_[0] > v_[1]) ? 0 : 1;
}

MATH_INLINE float distance(const Vec2& v1, const Vec2& v2)
{
  return sqrtf(square(v1.v_[0] - v2.v_[0]) + square(v1.v_[1] - v2.v_[1]));
}

MATH_INLINE float distanceSquared(const Vec2& v1, const Vec2& v2)
{
  return square(v1.v_[0] - v2.v_[0]) + square(v1.v_[1] - v2.v_[1]);
}

MATH_INLINE float dot(const Vec2& v1, const Vec2& v2)
{
  return v1.v_[0] * v2.v_[0] + v1.v_[1] * v2.v_[1];
}

MATH_INLINE Vec2 lerp(float f, const Vec2& v1, const Vec2& v2)
{
  return v1 + f * (v2 - v1);
}

MATH_INLINE float angle(Vec2 &u, Vec2 &v)
{
  float divider = u.length() * v.length();
	assert(!isZero(divider));
	float dotUV = dot(u, v) / divider;
	// rounding can give incorrect result, thus we need to clamp
	clamp(dotUV, -1, 1); 
  return acosf(dotUV);
}

MATH_INLINE float angleUnitCircle(Vec2 &v)
{
  Vec2 tmp = v;
  tmp.normalize();
  if ( tmp[1] > 0 )
    return acosf(tmp[0]);
  else
    return -acosf(tmp[0]);
}

MATH_INLINE Vec2 proj_u_on_v(Vec2 &u, Vec2 &v)
{
  return dot(u,v) / v.lengthSquared() * v;
}

MATH_INLINE Vec2 Vec2::perpDot() const
{
	return Vec2(-v_[1], v_[0]);
}

MATH_INLINE void Vec2::print() const
{
  printf("<%f %f>\n", v_[0], v_[1] );
}
