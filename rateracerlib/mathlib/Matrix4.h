#pragma once

#pragma warning( disable : 4514 ) // unreferenced inline function deleted
#pragma warning( disable : 4710 ) // function not inlined

#include <stdio.h>
#include <assert.h>

#include "MathUtils.h"
#include "Vec3.h"

class Matrix4
{
protected:
  float m_[4][4]; // [col][row]

public:
  Matrix4();
  Matrix4(const Matrix4&);

	Matrix4(const Vec3& xv,
						const Vec3& yv,
						const Vec3& zv,
						const Vec3& tv = Vec3(0,0,0));

  Matrix4(float, float, float, float,
  	        float, float, float, float,
            float, float, float, float,
            float, float, float, float);
	
  // array access
  float* operator [](int);
  const float* operator [](int) const;

  // assignment
	
	Matrix4& assign(const Vec3& xv,
										const Vec3& yv,
										const Vec3& zv,
										const Vec3& tv = Vec3(0,0,0));

	Matrix4& assign(float, float, float, float,
										float, float, float, float,
										float, float, float, float,
										float, float, float, float);
	
  void setZero();
  void setIdentity();

  void copyFrom(float [4][4]);
  void copyTo(float [4][4]) const;
	
  // math operators

  Matrix4& operator =(const Matrix4&);
	
  Matrix4& operator +=(const Matrix4&);
  Matrix4& operator -=(const Matrix4&);
  Matrix4& operator *=(const Matrix4&);
  Matrix4& operator *=(float);
  Matrix4& operator /=(float);
 
  Matrix4 operator +(const Matrix4&) const;
  Matrix4 operator -(const Matrix4&) const;
  Matrix4 operator -() const;
  Matrix4 operator *(const Matrix4&) const;
  Matrix4 operator *(float) const;
  Matrix4 operator /(float) const;

friend Matrix4 operator *(float, const Matrix4&);

  bool operator ==(const Matrix4&) const;
  bool operator !=(const Matrix4&) const;

	Matrix4 inverse() const;
	Matrix4 inverseOrtho() const;
  Matrix4 transpose() const;
  Matrix4 adjoint() const;
  
  float determinant() const;
  bool isSingular() const;

	// Rescale transformation matrix by scaling translation part
	Matrix4& changeScaleUniform(float ratio);

	// Access to column vectors (first 3 elements)
	Vec3 getColumnVec3(const int column) const;
	void setColumnVec3(const int column, const Vec3& v);
	
	// Transform by matrix
  Vec3 transform(const Vec3&) const;

	// Transform by matrix, rotation only, no translation
  Vec3 vecTransform(const Vec3&) const;

	// Transform by inverse (orthogonal) matrix
  Vec3 invOrthoTransform(const Vec3&) const;

	// Transform by inverse (orthogonal) matrix, rotation only, no translation
  Vec3 invOrthoVecTransform(const Vec3&) const;

  // transformation matrices

  static Matrix4 identity();
  static Matrix4 rotate(float, const Vec3& axis);
  static Matrix4 rotateX(float);
  static Matrix4 rotateY(float);
  static Matrix4 rotateZ(float);
  static Matrix4 scale(float, float, float);
  static Matrix4 scaleUniform(float);
  static Matrix4 translate( const Vec3& pos );
  static Matrix4 translate(float, float, float);

	// utility methods

  static Matrix4 matFromVec(const Vec3& vec, const Vec3& origin ); 
	
	// The following function creates a matrix positioned at (p), with the
	// negative z axis pointing at (pointat) and the y axis aligned with
	// the (up) vector. If this matrix is inverted it can be used as a
	// camera matrix with the same behaviour as gluLookAt().
	static Matrix4 pointAt(const Vec3 &p,
													 const Vec3 &pointat,
													 const Vec3 &up);
	
	void print( char *str = NULL ) const;
};

// CONSTRUCTORS

MATH_INLINE Matrix4::Matrix4()
{
  //setZero();
}

MATH_INLINE Matrix4::Matrix4(const Matrix4& M)
{
	(*this) = M;
}

MATH_INLINE Matrix4::Matrix4(const Vec3& xv,
															 const Vec3& yv,
															 const Vec3& zv,
															 const Vec3& tv)
{
	assign(xv, yv, zv, tv);
}

MATH_INLINE Matrix4::Matrix4(float e00, float e01, float e02, float e03,
															 float e10, float e11, float e12, float e13,
															 float e20, float e21, float e22, float e23,
															 float e30, float e31, float e32, float e33)
{
  m_[0][0] = e00; m_[1][0] = e01; m_[2][0] = e02; m_[3][0] = e03;
  m_[0][1] = e10; m_[1][1] = e11; m_[2][1] = e12; m_[3][1] = e13;
  m_[0][2] = e20; m_[1][2] = e21; m_[2][2] = e22; m_[3][2] = e23;
  m_[0][3] = e30; m_[1][3] = e31; m_[2][3] = e32; m_[3][3] = e33;
}

// ARRAY ACCESS

MATH_INLINE float* Matrix4::operator [](int i)
{
  assert(i == 0 || i == 1 || i == 2 || i == 3);
  return &m_[i][0];
}

MATH_INLINE const float* Matrix4::operator [](int i) const
{
  assert(i == 0 || i == 1 || i == 2 || i == 3);
  return &m_[i][0];
}

// ASSIGNMENT

MATH_INLINE Matrix4& Matrix4::assign(const Vec3& xv,
																			 const Vec3& yv,
																			 const Vec3& zv,
																			 const Vec3& tv)
{
  m_[0][0] = xv[0]; m_[1][0] = yv[0]; m_[2][0] = zv[0]; m_[3][0] = tv[0];
  m_[0][1] = xv[1]; m_[1][1] = yv[1]; m_[2][1] = zv[1]; m_[3][1] = tv[1];
  m_[0][2] = xv[2]; m_[1][2] = yv[2]; m_[2][2] = zv[2]; m_[3][2] = tv[2];
  m_[0][3] = 0;     m_[1][3] = 0;     m_[2][3] = 0;     m_[3][3] = 1;
  return *this;
}

MATH_INLINE Matrix4& Matrix4::assign(float e00, float e01, float e02, float e03,
																			 float e10, float e11, float e12, float e13,
																			 float e20, float e21, float e22, float e23,
																			 float e30, float e31, float e32, float e33)
{
  m_[0][0] = e00; m_[1][0] = e01; m_[2][0] = e02; m_[3][0] = e03;
  m_[0][1] = e10; m_[1][1] = e11; m_[2][1] = e12; m_[3][1] = e13;
  m_[0][2] = e20; m_[1][2] = e21; m_[2][2] = e22; m_[3][2] = e23;
  m_[0][3] = e30; m_[1][3] = e31; m_[2][3] = e32; m_[3][3] = e33;
  return *this;
}

MATH_INLINE Matrix4& Matrix4::operator =(const Matrix4& M)
{
  assign(M[0][0], M[1][0], M[2][0], M[3][0],
				 M[0][1], M[1][1], M[2][1], M[3][1],
				 M[0][2], M[1][2], M[2][2], M[3][2],
				 M[0][3], M[1][3], M[2][3], M[3][3]);
  return *this;
}


MATH_INLINE void Matrix4::setZero()
{
	assign(0,0,0,0,
				 0,0,0,0,
				 0,0,0,0,
				 0,0,0,0);
}

MATH_INLINE void Matrix4::setIdentity()
{
	assign(1,0,0,0,
				 0,1,0,0,
				 0,0,1,0,
				 0,0,0,1);
}

MATH_INLINE void Matrix4::copyFrom(float f[4][4])
{
	assign(f[0][0], f[1][0], f[2][0], f[3][0],
				 f[0][1], f[1][1], f[2][1], f[3][1],
				 f[0][2], f[1][2], f[2][2], f[3][2],
				 f[0][3], f[1][3], f[2][3], f[3][3]);
}

MATH_INLINE void Matrix4::copyTo(float f[4][4]) const
{
  f[0][0] = m_[0][0]; f[0][1] = m_[0][1]; f[0][2] = m_[0][2]; f[0][3] = m_[0][3];
  f[1][0] = m_[1][0]; f[1][1] = m_[1][1]; f[1][2] = m_[1][2]; f[1][3] = m_[1][3];
  f[2][0] = m_[2][0]; f[2][1] = m_[2][1]; f[2][2] = m_[2][2]; f[2][3] = m_[2][3];
  f[3][0] = m_[3][0]; f[3][1] = m_[3][1]; f[3][2] = m_[3][2]; f[3][3] = m_[3][3];
}

MATH_INLINE Matrix4& Matrix4::changeScaleUniform(float ratio)
{
	m_[3][0] *= ratio;
	m_[3][1] *= ratio;
	m_[3][2] *= ratio;
	return *this;
}

MATH_INLINE Vec3 Matrix4::getColumnVec3(const int column) const
{
	return Vec3(m_[column][0], m_[column][1], m_[column][2]);
}

MATH_INLINE void Matrix4::setColumnVec3(const int column, const Vec3& v)
{
	m_[column][0] = v[0]; m_[column][1] = v[1]; m_[column][2] = v[2];
}

MATH_INLINE Vec3 Matrix4::transform(const Vec3& v) const
{
  return Vec3(
    v[0] * m_[0][0] + v[1] * m_[1][0] + v[2] * m_[2][0] + m_[3][0],
    v[0] * m_[0][1] + v[1] * m_[1][1] + v[2] * m_[2][1] + m_[3][1],
    v[0] * m_[0][2] + v[1] * m_[1][2] + v[2] * m_[2][2] + m_[3][2]);
}

MATH_INLINE Vec3 Matrix4::vecTransform(const Vec3& v) const
{
  return Vec3(
    v[0] * m_[0][0] + v[1] * m_[1][0] + v[2] * m_[2][0],
    v[0] * m_[0][1] + v[1] * m_[1][1] + v[2] * m_[2][1],
    v[0] * m_[0][2] + v[1] * m_[1][2] + v[2] * m_[2][2]);
}

MATH_INLINE Vec3 Matrix4::invOrthoTransform(const Vec3&v) const
{
  float a = v[0] - m_[3][0];
  float b = v[1] - m_[3][1];
  float c = v[2] - m_[3][2];
	
  return Vec3(
		a * m_[0][0] + b * m_[0][1] + c * m_[0][2],
		a * m_[1][0] + b * m_[1][1] + c * m_[1][2],
		a * m_[2][0] + b * m_[2][1] + c * m_[2][2]);
}

MATH_INLINE Vec3 Matrix4::invOrthoVecTransform(const Vec3&v) const
{
  return Vec3(
		v[0] * m_[0][0] + v[1] * m_[0][1] + v[2] * m_[0][2],
		v[0] * m_[1][0] + v[1] * m_[1][1] + v[2] * m_[1][2],
		v[0] * m_[2][0] + v[1] * m_[2][1] + v[2] * m_[2][2]);
}
