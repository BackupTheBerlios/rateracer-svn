#pragma once

#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "MathUtils.h"

class Vec3
{
//protected:
public:
  float v_[3];

public:
  Vec3();
  Vec3(const Vec3&);
  Vec3(float, float, float);
  Vec3(const float[3]);

  // array access
  float& operator [](int);
  const float& operator [](int) const;

  // assignment
	
  Vec3& assign(float, float, float);
	
  void setZero();
    
  void copyFrom(const float [3]);
  void copyTo(float [3]) const;
	
	// operations
	
  Vec3& normalize();
	Vec3& normalizeSafely();
  Vec3& clampVec(float, float);
	
  // math operators
	
  Vec3& operator =(const Vec3&);
	
  Vec3& operator +=(const Vec3&);
  Vec3& operator -=(const Vec3&);
  Vec3& operator *=(float);
  Vec3& operator /=(float);

  Vec3 operator +(const Vec3&) const;
  Vec3 operator -(const Vec3&) const;
  Vec3 operator -() const;
  Vec3 operator *(float) const;
  Vec3 operator /(float) const;

friend Vec3 operator *(float, const Vec3&);

  bool operator ==(const Vec3&) const;
  bool operator !=(const Vec3&) const;

friend float dot(const Vec3&, const Vec3&);
friend Vec3 cross(const Vec3&, const Vec3&);

	float length() const;
	float lengthSquared() const;
	float lengthManhattan() const;

friend float distance(const Vec3&, const Vec3&);
friend float distanceSquared(const Vec3&, const Vec3&);

	// Angle between vectors u and v (radians)
friend float angle( const Vec3 &u, const Vec3 &v );

	// Project vector u onto vector v
friend Vec3 proj_u_on_v( const Vec3 &u, const Vec3 &v);

	// Project vector v onto plane (p0, n)
friend Vec3 projectOnPlane(const Vec3& v,
																const Vec3& planeP0,
																const Vec3& planeN);

	// Linear interpolation between two vectors
friend Vec3 lerp(float, const Vec3&, const Vec3&);

	// Return index of minimum/maximum component
	int minComponent() const;
	int maxComponent() const;

	void print() const; // Debug print to console
};

// CONSTRUCTORS

MATH_INLINE Vec3::Vec3()
{
  v_[0] = v_[1] = v_[2] = 0;
}

MATH_INLINE Vec3::Vec3(const Vec3& v)
{
  v_[0] = v.v_[0]; v_[1] = v.v_[1]; v_[2] = v.v_[2];
}

MATH_INLINE Vec3::Vec3(float x, float y, float z)
{
  v_[0] = x; v_[1] = y; v_[2] = z;
}

MATH_INLINE Vec3::Vec3(const float f[3])
{
  v_[0] = f[0]; v_[1] = f[1]; v_[2] = f[2];
}

// ARRAY ACCESS

MATH_INLINE float& Vec3::operator [](int i) 
{
  assert(i == 0 || i == 1 || i == 2);
  return v_[i];
}

MATH_INLINE const float& Vec3::operator [](int i) const
{
  assert(i == 0 || i == 1 || i == 2);
  return v_[i];
}

// ASSIGNMENT

MATH_INLINE Vec3& Vec3::assign(float x, float y, float z)
{
  v_[0] = x; v_[1] = y; v_[2] = z;
  return *this;
}

MATH_INLINE Vec3& Vec3::operator =(const Vec3& v)
{
  v_[0] = v.v_[0]; v_[1] = v.v_[1]; v_[2] = v.v_[2];
  return *this;
}


MATH_INLINE void Vec3::setZero()
{
  v_[0] = v_[1] = v_[2] = 0.0f;
}

// MATH OPERATORS

MATH_INLINE Vec3& Vec3::operator +=(const Vec3& v)
{
  v_[0] += v.v_[0]; v_[1] += v.v_[1]; v_[2] += v.v_[2];
  return *this;
}

MATH_INLINE Vec3& Vec3::operator -=(const Vec3& v)
{
  v_[0] -= v.v_[0]; v_[1] -= v.v_[1]; v_[2] -= v.v_[2];
  return *this;
}

MATH_INLINE Vec3& Vec3::operator *=(float c)
{
  v_[0] *= c; v_[1] *= c; v_[2] *= c;
  return *this;
}

MATH_INLINE Vec3& Vec3::operator /=(float c)
{
  assert(!isZero(c));
	float ic = 1.0f / c;
  v_[0] *= ic; v_[1] *= ic; v_[2] *= ic;
  return *this;
}

MATH_INLINE Vec3 Vec3::operator +(const Vec3& v) const
{
  return Vec3(v_[0] + v.v_[0], v_[1] + v.v_[1], v_[2] + v.v_[2]);
}

MATH_INLINE Vec3 Vec3::operator -(const Vec3& v) const
{
  return Vec3(v_[0] - v.v_[0], v_[1] - v.v_[1], v_[2] - v.v_[2]);
}

MATH_INLINE Vec3 Vec3::operator -() const
{
  return Vec3(-v_[0], -v_[1], -v_[2]);
}

MATH_INLINE Vec3 Vec3::operator *(float c) const
{
  return Vec3(v_[0] * c, v_[1] * c, v_[2] * c);
}

MATH_INLINE Vec3 Vec3::operator /(float c) const
{
  assert(!isZero(c));
	float ic = 1.0f / c;
  return Vec3(v_[0] * ic, v_[1] * ic, v_[2] * ic);
}

MATH_INLINE Vec3 operator *(float c, const Vec3& v)
{
  return Vec3(c * v.v_[0], c * v[1], c * v.v_[2]);
}

MATH_INLINE bool Vec3::operator ==(const Vec3& v) const
{
  return
    (fuzzyEQ(v_[0], v.v_[0]) && fuzzyEQ(v_[1], v.v_[1]) && fuzzyEQ(v_[2], v.v_[2]));
}

MATH_INLINE bool Vec3::operator !=(const Vec3& v) const
{
  return
    !(fuzzyEQ(v_[0], v.v_[0]) && fuzzyEQ(v_[1], v.v_[1]) && fuzzyEQ(v_[2], v.v_[2]));
}

// OPERATIONS

MATH_INLINE Vec3& Vec3::clampVec(float lo, float hi)
{
  clamp(v_[0], lo, hi); clamp(v_[1], lo, hi); clamp(v_[2], lo, hi);
  return *this;
}

MATH_INLINE float Vec3::length() const
{
  return sqrtf(square(v_[0]) + square(v_[1]) + square(v_[2]));
}

MATH_INLINE float Vec3::lengthManhattan() const
{
  return fabsf(v_[0]) + fabsf(v_[1]) + fabsf(v_[2]);
}

MATH_INLINE float Vec3::lengthSquared() const
{
  return square(v_[0]) + square(v_[1]) + square(v_[2]);
}

MATH_INLINE Vec3& Vec3::normalize()
{
  float len = length();
  assert(!isZero(len));
  *this *= 1.0f / len;
  return *this;
}

MATH_INLINE Vec3& Vec3::normalizeSafely()
{
  float len = length();
  if (!isZero(len)) {
		*this *= 1.0f / len;
  }
  return *this;
}

MATH_INLINE void Vec3::copyFrom(const float f[3])
{
  v_[0] = f[0]; v_[1] = f[1]; v_[2] = f[2];
}

MATH_INLINE void Vec3::copyTo(float f[3]) const
{
  f[0] = v_[0]; f[1] = v_[1]; f[2] = v_[2];
}

MATH_INLINE int Vec3::minComponent() const
{
	return (v_[0] < v_[1]) ?
				((v_[0] < v_[2]) ? 0 : 2) :
				((v_[1] < v_[2]) ? 1 : 2);
}

MATH_INLINE int Vec3::maxComponent() const
{
	return (v_[0] > v_[1]) ?
				((v_[0] > v_[2]) ? 0 : 2) :
				((v_[1] > v_[2]) ? 1 : 2);
}

MATH_INLINE Vec3 cross(const Vec3& v1, const Vec3& v2)
{
  return Vec3(v1.v_[1] * v2.v_[2] - v1.v_[2] * v2.v_[1],
                   v1.v_[2] * v2.v_[0] - v1.v_[0] * v2.v_[2],
                   v1.v_[0] * v2.v_[1] - v1.v_[1] * v2.v_[0]);
}

MATH_INLINE float distance(const Vec3& v1, const Vec3& v2)
{
  return
		sqrtf(square(v1.v_[0] - v2.v_[0]) +
				  square(v1.v_[1] - v2.v_[1]) +
				  square(v1.v_[2] - v2.v_[2]));
}

MATH_INLINE float distanceSquared(const Vec3& v1, const Vec3& v2)
{
  return square(v1.v_[0] - v2.v_[0]) +
				 square(v1.v_[1] - v2.v_[1]) +
				 square(v1.v_[2] - v2.v_[2]);
}

MATH_INLINE float dot(const Vec3& v1, const Vec3& v2)
{
  return v1.v_[0] * v2.v_[0] +
				 v1.v_[1] * v2.v_[1] + 
				 v1.v_[2] * v2.v_[2];
}

MATH_INLINE Vec3 lerp(float f, const Vec3& v1, const Vec3& v2)
{
  return v1 + f * (v2 - v1);
}

MATH_INLINE Vec3 proj_u_on_v(const Vec3 &u, const Vec3 &v)
{
  return dot(u,v) / v.lengthSquared() * v;
}

MATH_INLINE float angle(const Vec3 &u, const Vec3&v)
{
  float divider = u.length() * v.length();
	assert(!isZero(divider));
	float dotUV = dot(u, v) / divider;
	// rounding can give incorrect result, thus we need to clamp
	clamp(dotUV, -1, 1); 
  return acosf(dotUV);
}

MATH_INLINE Vec3 projectOnPlane(const Vec3& v,
																const Vec3& planeP0, 
                                const Vec3& planeN) 
{
  return v - planeN * dot( v - planeP0, planeN );
}

MATH_INLINE void Vec3::print() const
{
  printf("<%f %f %f>\n", v_[0], v_[1], v_[2] );
}
