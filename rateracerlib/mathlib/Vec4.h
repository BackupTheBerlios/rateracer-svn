#pragma once

#include <stdio.h>
#include <assert.h>

#include "MathUtils.h"

class Vec4
{
protected:
  float v_[4];

public:
  Vec4();
  Vec4(const Vec4&);
  Vec4(float x, float y, float z, float w = 1);

  // array access
  float& operator [](int);
  const float& operator [](int) const;

  // assignment

  Vec4& assign(float x, float y, float z, float w = 1);

  void setZero();
  
  void copyFrom(float [4]);
  void copyTo(float [4]) const;
	
	// operations
	
  Vec4& normalize();
	Vec4& normalizeSafely();
	Vec4& clampVec(float, float);
	
  // math operators
	
  Vec4& operator =(const Vec4&);
	
  Vec4& operator +=(const Vec4&);
  Vec4& operator -=(const Vec4&);
  Vec4& operator *=(float);
  Vec4& operator /=(float);

  Vec4 operator +(const Vec4&) const;
  Vec4 operator -(const Vec4&) const;
  Vec4 operator -() const;
  Vec4 operator *(float) const;
  Vec4 operator /(float) const;

friend Vec4 operator *(float, const Vec4&);
	
  bool operator ==(const Vec4&) const;
  bool operator !=(const Vec4&) const;

friend float dot(const Vec4&, const Vec4&);

	float length() const;
	float lengthSquared() const;
	float lengthManhattan() const;
	
friend float distance(const Vec4&, const Vec4&);
friend float distanceSquared(const Vec4&, const Vec4&);

	// Angle between vectors u and v (radians)
friend float angle( Vec4 &u, Vec4 &v );

	// Project vector u onto vector v
friend Vec4 proj_u_on_v(Vec4 &u, Vec4 &v);

	// Linear interpolation between two vectors
friend Vec4 lerp(float, const Vec4&, const Vec4&);

	int minComponent() const;
	int maxComponent() const;

	void print() const; // Debug print to console
};

// CONSTRUCTORS

MATH_INLINE Vec4::Vec4()
{
   v_[0] = v_[1] = v_[2] = v_[3] = 0;
}

MATH_INLINE Vec4::Vec4(const Vec4& v)
{
  v_[0] = v.v_[0]; v_[1] = v.v_[1]; v_[2] = v.v_[2]; v_[3] = v.v_[3];
}

MATH_INLINE Vec4::Vec4(float x, float y, float z, float w)
{
  v_[0] = x; v_[1] = y; v_[2] = z; v_[3] = w;
}

// ARRAY ACCESS

MATH_INLINE float& Vec4::operator [](int i) 
{
  assert(i == 0 || i == 1 || i == 2 || i == 3);
  return v_[i];
}

MATH_INLINE const float& Vec4::operator [](int i) const
{
  assert(i == 0 || i == 1 || i == 2 || i == 3);
  return v_[i];
}

// ASSIGNMENT

MATH_INLINE Vec4& Vec4::assign(float x, float y, float z, float w)
{
  v_[0] = x; v_[1] = y; v_[2] = z; v_[3] = w;
  return *this;
}

MATH_INLINE Vec4& Vec4::operator =(const Vec4& v)
{
  v_[0] = v.v_[0]; v_[1] = v.v_[1]; v_[2] = v.v_[2]; v_[3] = v.v_[3];
  return *this;
}

MATH_INLINE void Vec4::setZero()
{
  v_[0] = v_[1] = v_[2] = v_[3] = 0.0f;
}

// MATH OPERATORS

MATH_INLINE Vec4& Vec4::operator +=(const Vec4& v)
{
  v_[0] += v.v_[0]; v_[1] += v.v_[1]; v_[2] += v.v_[2]; v_[3] += v.v_[3];
  return *this;
}

MATH_INLINE Vec4& Vec4::operator -=(const Vec4& v)
{
  v_[0] -= v.v_[0]; v_[1] -= v.v_[1]; v_[2] -= v.v_[2]; v_[3] -= v.v_[3];
  return *this;
}

MATH_INLINE Vec4& Vec4::operator *=(float c)
{
  v_[0] *= c; v_[1] *= c; v_[2] *= c; v_[3] *= c;
  return *this;
}

MATH_INLINE Vec4& Vec4::operator /=(float c)
{
  assert(!isZero(c));
	float ic = 1.0f / c;
  v_[0] *= ic; v_[1] *= ic; v_[2] *= ic; v_[3] *= ic;
  return *this;
}

MATH_INLINE Vec4 Vec4::operator +(const Vec4& v) const
{
  return Vec4(v_[0] + v.v_[0], v_[1] + v.v_[1], v_[2] + v.v_[2], v_[3] + v.v_[3]);
}

MATH_INLINE Vec4 Vec4::operator -(const Vec4& v) const
{
  return Vec4(v_[0] - v.v_[0], v_[1] - v.v_[1], v_[2] - v.v_[2], v_[3] - v.v_[3]);
}

MATH_INLINE Vec4 Vec4::operator -() const
{
  return Vec4(-v_[0], -v_[1], -v_[2], -v_[3]);
}

MATH_INLINE Vec4 Vec4::operator *(float c) const
{
  return Vec4(v_[0] * c, v_[1] * c, v_[2] * c, v_[3] * c);
}

MATH_INLINE Vec4 Vec4::operator /(float c) const
{
  assert(!isZero(c));
	float ic = 1.0f / c;
  return Vec4(v_[0] * ic, v_[1] * ic, v_[2] * ic, v_[3] * ic);
}

MATH_INLINE Vec4 operator *(float c, const Vec4& v)
{
  return Vec4(c * v.v_[0], c * v.v_[1], c * v.v_[2], c * v.v_[3]);
}

MATH_INLINE bool Vec4::operator ==(const Vec4& v) const
{
  return (fuzzyEQ(v_[0], v.v_[0]) && fuzzyEQ(v_[1], v.v_[1]) &&
          fuzzyEQ(v_[2], v.v_[2]) && fuzzyEQ(v_[3], v.v_[3]));
}

MATH_INLINE bool Vec4::operator !=(const Vec4& v) const
{
  return !(fuzzyEQ(v_[0], v.v_[0]) && fuzzyEQ(v_[1], v.v_[1]) &&
					 fuzzyEQ(v_[2], v.v_[2]) && fuzzyEQ(v_[3], v.v_[3]));
}

// OPERATIONS

MATH_INLINE Vec4& Vec4::clampVec(float lo, float hi)
{
  clamp(v_[0], lo, hi); clamp(v_[1], lo, hi);
  clamp(v_[2], lo, hi); clamp(v_[3], lo, hi);
  return *this;
}

MATH_INLINE float Vec4::length() const
{
  return sqrtf(square(v_[0]) + square(v_[1]) + square(v_[2]) + square(v_[3]));
}

MATH_INLINE float Vec4::lengthSquared() const
{
  return square(v_[0]) + square(v_[1]) + square(v_[2]) + square(v_[3]);
}

MATH_INLINE float Vec4::lengthManhattan() const
{
  return fabsf(v_[0]) + fabsf(v_[1]) + fabsf(v_[2]) + fabsf(v_[3]);
}

MATH_INLINE Vec4& Vec4::normalize()
{
  float len = length();
  assert(!isZero(len));
  *this *= 1.0f / len;
  return *this;
}

MATH_INLINE Vec4& Vec4::normalizeSafely()
{
  float len = length();
  if (!isZero(len)) {
		*this *= 1.0f / len;
  }
  return *this;
}

MATH_INLINE void Vec4::copyFrom(float f[4])
{
  v_[0] = f[0]; v_[1] = f[1]; v_[2] = f[2]; v_[3] = f[3];
}

MATH_INLINE void Vec4::copyTo(float f[4]) const
{
  f[0] = v_[0]; f[1] = v_[1]; f[2] = v_[2]; f[3] = v_[3];
}

MATH_INLINE int Vec4::minComponent() const
{
	int index = 0;
	float min = v_[0];
	if (v_[1] < min) {
		min = v_[1];
		index = 1;
	}
	if (v_[2] < min) {
		min = v_[2];
		index = 2;
	}
	if (v_[3] < min) {
		min = v_[3];
		index = 3;
	}
	return index;
}

MATH_INLINE int Vec4::maxComponent() const
{
	int index = 0;
	float max = v_[0];
	if (v_[1] > max) {
		max = v_[1];
		index = 1;
	}
	if (v_[2] > max) {
		max = v_[2];
		index = 2;
	}
	if (v_[3] > max) {
		max = v_[3];
		index = 3;
	}
	return index;
}

MATH_INLINE float distance(const Vec4& v1, const Vec4& v2)
{
  return sqrtf(square(v1.v_[0] - v2.v_[0]) + square(v1.v_[1] - v2.v_[1]) +
							 square(v1.v_[2] - v2.v_[2]) + square(v1.v_[3] - v2.v_[3]));
}

MATH_INLINE float distanceSquared(const Vec4& v1, const Vec4& v2)
{
  return square(v1.v_[0] - v2.v_[0]) + square(v1.v_[1] - v2.v_[1]) +
				 square(v1.v_[2] - v2.v_[2]) + square(v1.v_[3] - v2.v_[3]);
}

MATH_INLINE float dot(const Vec4& v1, const Vec4& v2)
{
  return v1.v_[0] * v2.v_[0] + v1.v_[1] * v2.v_[1] + 
				 v1.v_[2] * v2.v_[2] + v1.v_[3] * v2.v_[3];
}

MATH_INLINE float angle(Vec4 &u, Vec4&v)
{
  float divider = u.length() * v.length();
	assert(!isZero(divider));
	float dotUV = dot(u, v) / divider;
	// rounding can give incorrect result, thus we need to clamp
	clamp(dotUV, -1, 1); 
  return acosf(dotUV);
}

MATH_INLINE Vec4 proj_u_on_v(Vec4 &u, Vec4 &v)
{
  return dot(u,v) / v.lengthSquared() * v;
}

MATH_INLINE Vec4 lerp(float f, const Vec4& v1, const Vec4& v2)
{
  return v1 + f * (v2 - v1);
}

MATH_INLINE void Vec4::print() const
{
  printf("<%f %f %f %f>\n", v_[0], v_[1], v_[2], v_[3] );
}
