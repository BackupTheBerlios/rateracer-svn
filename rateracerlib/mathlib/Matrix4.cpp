#include "Matrix4.h"

// private function: RCD
// - dot product of row i of matrix A and row j of matrix B

MATH_INLINE float RCD(const Matrix4& A, const Matrix4& B, int i, int j)
{
  return A[i][0]*B[0][j] + A[i][1]*B[1][j] + A[i][2]*B[2][j] + A[i][3]*B[3][j];
}

// private function: MINOR
// - MINOR of M[r][c]; r in {0,1,2,3}-{r0,r1,r2}; c in {0,1,2,3}-{c0,c1,c2}

MATH_INLINE float MINOR(const Matrix4& M,
										  int r0, int r1, int r2,
										  int c0, int c1, int c2)
{
  return	M[r0][c0] * (M[r1][c1] * M[r2][c2] - M[r2][c1] * M[r1][c2]) -
					M[r0][c1] * (M[r1][c0] * M[r2][c2] - M[r2][c0] * M[r1][c2]) +
					M[r0][c2] * (M[r1][c0] * M[r2][c1] - M[r2][c0] * M[r1][c1]);
}

////////////////////////////////////////////////////////////////////////////
// OPERATORS

Matrix4& Matrix4::operator +=(const Matrix4& M)
{
  m_[0][0] += M[0][0]; m_[0][1] += M[0][1]; m_[0][2] += M[0][2]; m_[0][3] += M[0][3];
  m_[1][0] += M[1][0]; m_[1][1] += M[1][1]; m_[1][2] += M[1][2]; m_[1][3] += M[1][3];
  m_[2][0] += M[2][0]; m_[2][1] += M[2][1]; m_[2][2] += M[2][2]; m_[2][3] += M[2][3];
  m_[3][0] += M[3][0]; m_[3][1] += M[3][1]; m_[3][2] += M[3][2]; m_[3][3] += M[3][3];
  return *this;
}

Matrix4& Matrix4::operator -=(const Matrix4& M)
{
  m_[0][0] -= M[0][0]; m_[0][1] -= M[0][1]; m_[0][2] -= M[0][2]; m_[0][3] -= M[0][3];
  m_[1][0] -= M[1][0]; m_[1][1] -= M[1][1]; m_[1][2] -= M[1][2]; m_[1][3] -= M[1][3];
  m_[2][0] -= M[2][0]; m_[2][1] -= M[2][1]; m_[2][2] -= M[2][2]; m_[2][3] -= M[2][3];
  m_[3][0] -= M[3][0]; m_[3][1] -= M[3][1]; m_[3][2] -= M[3][2]; m_[3][3] -= M[3][3];
  return *this;
}

Matrix4& Matrix4::operator *=(const Matrix4& M)
{
  assign(
		RCD(*this, M,0,0), RCD(*this, M,1,0), RCD(*this, M,2,0), RCD(*this, M,3,0),
		RCD(*this, M,0,1), RCD(*this, M,1,1), RCD(*this, M,2,1), RCD(*this, M,3,1),
		RCD(*this, M,0,2), RCD(*this, M,1,2), RCD(*this, M,2,2), RCD(*this, M,3,2),
		RCD(*this, M,0,3), RCD(*this, M,1,3), RCD(*this, M,2,3), RCD(*this, M,3,3));
  return *this;
}

Matrix4& Matrix4::operator *=(float d)
{
  m_[0][0] *= d; m_[0][1] *= d; m_[0][2] *= d; m_[0][3] *= d;
  m_[1][0] *= d; m_[1][1] *= d; m_[1][2] *= d; m_[1][3] *= d;
  m_[2][0] *= d; m_[2][1] *= d; m_[2][2] *= d; m_[2][3] *= d;
  m_[3][0] *= d; m_[3][1] *= d; m_[3][2] *= d; m_[3][3] *= d;
  return *this;
}

Matrix4& Matrix4::operator /=(float d)
{
  float di = 1 / d;
  m_[0][0] *= di; m_[0][1] *= di; m_[0][2] *= di; m_[0][3] *= di;
  m_[1][0] *= di; m_[1][1] *= di; m_[1][2] *= di; m_[1][3] *= di;
  m_[2][0] *= di; m_[2][1] *= di; m_[2][2] *= di; m_[2][3] *= di;
  m_[3][0] *= di; m_[3][1] *= di; m_[3][2] *= di; m_[3][3] *= di;
  return *this;
}

Matrix4 Matrix4::operator +(const Matrix4& M) const
{
  return Matrix4(
		m_[0][0] + M[0][0], m_[1][0] + M[1][0], m_[2][0] + M[2][0], m_[3][0] + M[3][0],
		m_[0][1] + M[0][1], m_[1][1] + M[1][1], m_[2][1] + M[2][1], m_[3][1] + M[3][1],
		m_[0][2] + M[0][2], m_[1][2] + M[1][2], m_[2][2] + M[2][2], m_[3][2] + M[3][2],
		m_[0][3] + M[0][3], m_[1][3] + M[1][3], m_[2][3] + M[2][3], m_[3][3] + M[3][3]);
}

Matrix4 Matrix4::operator -(const Matrix4& M) const
{
  return Matrix4(
		m_[0][0] - M[0][0], m_[1][0] - M[1][0], m_[2][0] - M[2][0], m_[3][0] - M[3][0],
		m_[0][1] - M[0][1], m_[1][1] - M[1][1], m_[2][1] - M[2][1], m_[3][1] - M[3][1],
		m_[0][2] - M[0][2], m_[1][2] - M[1][2], m_[2][2] - M[2][2], m_[3][2] - M[3][2],
		m_[0][3] - M[0][3], m_[1][3] - M[1][3], m_[2][3] - M[2][3], m_[3][3] - M[3][3]);
}

Matrix4 Matrix4::operator -() const
{
  return Matrix4(
		-m_[0][0], -m_[1][0], -m_[2][0], -m_[3][0],
		-m_[0][1], -m_[1][1], -m_[2][1], -m_[3][1],
		-m_[0][2], -m_[1][2], -m_[2][2], -m_[3][2],
		-m_[0][3], -m_[1][3], -m_[2][3], -m_[3][3]);
}

Matrix4 Matrix4::operator *(const Matrix4& M) const
{
  return Matrix4(
		RCD(*this, M,0,0), RCD(*this, M,1,0), RCD(*this, M,2,0), RCD(*this, M,3,0),
		RCD(*this, M,0,1), RCD(*this, M,1,1), RCD(*this, M,2,1), RCD(*this, M,3,1),
		RCD(*this, M,0,2), RCD(*this, M,1,2), RCD(*this, M,2,2), RCD(*this, M,3,2),
		RCD(*this, M,0,3), RCD(*this, M,1,3), RCD(*this, M,2,3), RCD(*this, M,3,3));
}

Matrix4 Matrix4::operator *(float d) const
{
  return Matrix4(
		m_[0][0] * d, m_[1][0] * d, m_[2][0] * d, m_[3][0] * d,
		m_[0][1] * d, m_[1][1] * d, m_[2][1] * d, m_[3][1] * d,
		m_[0][2] * d, m_[1][2] * d, m_[2][2] * d, m_[3][2] * d,
		m_[0][3] * d, m_[1][3] * d, m_[2][3] * d, m_[3][3] * d);
}

Matrix4 Matrix4::operator /(float d) const
{
  assert(!isZero(d));
  float di = 1 / d;
  return Matrix4(
		m_[0][0] * di, m_[1][0] * di, m_[2][0] * di, m_[3][0] * di,
		m_[0][1] * di, m_[1][1] * di, m_[2][1] * di, m_[3][1] * di,
		m_[0][2] * di, m_[1][2] * di, m_[2][2] * di, m_[3][2] * di,
		m_[0][3] * di, m_[1][3] * di, m_[2][3] * di, m_[3][3] * di);
}

Matrix4 operator *(float d, const Matrix4& M)
{
  return Matrix4(
		M[0][0] * d, M[1][0] * d, M[2][0] * d, M[3][0] * d,
		M[0][1] * d, M[1][1] * d, M[2][1] * d, M[3][1] * d,
		M[0][2] * d, M[1][2] * d, M[2][2] * d, M[3][2] * d,
		M[0][3] * d, M[1][3] * d, M[2][3] * d, M[3][3] * d);
}

bool Matrix4::operator ==(const Matrix4& M) const
{
  return (fuzzyEQ(m_[0][0], M[0][0]) &&
					fuzzyEQ(m_[0][1], M[0][1]) &&
					fuzzyEQ(m_[0][2], M[0][2]) &&
					fuzzyEQ(m_[0][3], M[0][3]) &&
				 
					fuzzyEQ(m_[1][0], M[1][0]) &&
					fuzzyEQ(m_[1][1], M[1][1]) &&
					fuzzyEQ(m_[1][2], M[1][2]) &&
					fuzzyEQ(m_[1][3], M[1][3]) &&

 					fuzzyEQ(m_[2][0], M[2][0]) &&
					fuzzyEQ(m_[2][1], M[2][1]) &&
					fuzzyEQ(m_[2][2], M[2][2]) &&
					fuzzyEQ(m_[2][3], M[2][3]) &&

					fuzzyEQ(m_[3][0], M[3][0]) &&
					fuzzyEQ(m_[3][1], M[3][1]) &&
					fuzzyEQ(m_[3][2], M[3][2]) &&
					fuzzyEQ(m_[3][3], M[3][3]));
}

bool Matrix4::operator !=(const Matrix4& M) const
{
  return (!(*this == M));
}

// OPERATIONS

Matrix4 Matrix4::transpose() const
{
  return Matrix4(
		m_[0][0], m_[0][1], m_[0][2], m_[0][3],
		m_[1][0], m_[1][1], m_[1][2], m_[1][3],
		m_[2][0], m_[2][1], m_[2][2], m_[2][3],
		m_[3][0], m_[3][1], m_[3][2], m_[3][3]);
}

Matrix4 Matrix4::inverse() const
{
  assert(!isSingular());
  return adjoint() / determinant();
}

Matrix4 Matrix4::inverseOrtho() const
{
  float a = -m_[3][0];
  float b = -m_[3][1];
  float c = -m_[3][2];
	
  return Matrix4(
		m_[0][0], m_[0][1], m_[0][2], a * m_[0][0] + b * m_[0][1] + c * m_[0][2],
		m_[1][0], m_[1][1], m_[1][2], a * m_[1][0] + b * m_[1][1] + c * m_[1][2],
		m_[2][0], m_[2][1], m_[2][2], a * m_[2][0] + b * m_[2][1] + c * m_[2][2],
		0,        0,        0,        1);
}

Matrix4 Matrix4::adjoint() const
{
	return Matrix4(
		 MINOR(*this, 1, 2, 3, 1, 2, 3),
		-MINOR(*this, 1, 2, 3, 0, 2, 3),
		 MINOR(*this, 1, 2, 3, 0, 1, 3),
		-MINOR(*this, 1, 2, 3, 0, 1, 2),

		-MINOR(*this, 0, 2, 3, 1, 2, 3),
		 MINOR(*this, 0, 2, 3, 0, 2, 3),
		-MINOR(*this, 0, 2, 3, 0, 1, 3),
		 MINOR(*this, 0, 2, 3, 0, 1, 2),

		 MINOR(*this, 0, 1, 3, 1, 2, 3),
		-MINOR(*this, 0, 1, 3, 0, 2, 3),
		 MINOR(*this, 0, 1, 3, 0, 1, 3),
		-MINOR(*this, 0, 1, 3, 0, 1, 2),

		-MINOR(*this, 0, 1, 2, 1, 2, 3),
		 MINOR(*this, 0, 1, 2, 0, 2, 3),
		-MINOR(*this, 0, 1, 2, 0, 1, 3),
		 MINOR(*this, 0, 1, 2, 0, 1, 2)
	);
}

float Matrix4::determinant() const
{
  return
		m_[0][0] * MINOR(*this, 1, 2, 3, 1, 2, 3) -
		m_[0][1] * MINOR(*this, 1, 2, 3, 0, 2, 3) +
		m_[0][2] * MINOR(*this, 1, 2, 3, 0, 1, 3) -
		m_[0][3] * MINOR(*this, 1, 2, 3, 0, 1, 2);
}

bool Matrix4::isSingular() const
{
  return isZero(determinant());
}

////////////////////////////////////////////////////////////////////////////
// TRANSFORMATION MATRICES

Matrix4 Matrix4::identity()
{
  return Matrix4(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
}

Matrix4 Matrix4::rotate(float angle, const Vec3& axis)
{
  float invLength = 1.0f / axis.length();
  float x = axis[0] * invLength;
  float y = axis[1] * invLength;
  float z = axis[2] * invLength;
	
	float si = sinf(deg2rad(angle));
  float co = cosf(deg2rad(angle));
	
	float sx = x * si;
	float sy = y * si;
	float sz = z * si;
	
	float cx = x * (1 - co);
	float cy = y * (1 - co);
	
	float cxy = cx * y;
	float cxz = cx * z;
	float cyz = cy * z;

	float cx2 = cx * x;
	float cy2 = cy * y;
	float cz2 = (1 - co) - cx2 - cy2;

	return Matrix4(co + cx2, cxy - sz, cxz + sy, 0,
									 cxy + sz, co + cy2, cyz - sx, 0,
									 cxz - sy, cyz + sx, co + cz2, 0,
									 0,        0,        0,        1);
}

Matrix4 Matrix4::rotateX(float angle)
{
	float si = sinf(deg2rad(angle));
  float co = cosf(deg2rad(angle));

 	return Matrix4( 1,  0,   0,   0,
										0,  co, -si,  0,
										0,  si,  co,  0,
										0,  0,   0,   1 );
}

Matrix4 Matrix4::rotateY(float angle)
{
	float si = sinf(deg2rad(angle));
  float co = cosf(deg2rad(angle));
	
 	return Matrix4( co,  0,  si,  0,
										0,   1,  0,   0,
									 -si,  0,  co,  0,
										0,   0,  0,   1 );
}

Matrix4 Matrix4::rotateZ(float angle)
{
	float si = sinf(deg2rad(angle));
  float co = cosf(deg2rad(angle));
	
 	return Matrix4( co, -si,  0,  0,
										si,  co,  0,  0,
										0,   0,   1,  0,
										0,   0,   0,  1 );
}

Matrix4 Matrix4::scale(float sx, float sy, float sz)
{
  return Matrix4(sx, 0,  0,  0,
									 0,  sy, 0,  0,
									 0,  0,  sz, 0,
									 0,  0,  0,  1);
}

Matrix4 Matrix4::scaleUniform(float s)
{
  return Matrix4(s, 0, 0, 0,
									 0, s, 0, 0,
									 0, 0, s, 0,
									 0, 0, 0, 1);
}

Matrix4 Matrix4::translate( const Vec3& pos )
{
  return Matrix4(1, 0, 0, pos[0],
		               0, 1, 0, pos[1],
		               0, 0, 1, pos[2],
		               0, 0, 0, 1);
}

Matrix4 Matrix4::translate(float x, float y, float z)
{
  return Matrix4(1, 0, 0, x,
									 0, 1, 0, y,
									 0, 0, 1, z,
									 0, 0, 0, 1);
}

void Matrix4::print( char *str ) const
{
  if( str ) {
    printf("%s\n", str );
  }
  printf("%.2f %.2f %.2f %.2f\n"
	       "%.2f %.2f %.2f %.2f\n"
	       "%.2f %.2f %.2f %.2f\n"
	       "%.2f %.2f %.2f %.2f\n",
	 m_[0][0], m_[1][0], m_[2][0], m_[3][0],
	 m_[0][1], m_[1][1], m_[2][1], m_[3][1],
	 m_[0][2], m_[1][2], m_[2][2], m_[3][2],
	 m_[0][3], m_[1][3], m_[2][3], m_[3][3] );
}

Matrix4 Matrix4::matFromVec( const Vec3 &vec,
			                           const Vec3 &origin )
{
  Matrix4 mat = mat.identity();

	mat.setColumnVec3(2, vec);

  int imax = 0;
  Vec3 ortho(0.707107f, 0.707107f, 0.707107f);

  if (fabs(vec[1]) > fabs(vec[imax]))
    imax = 1;
  if (fabs(vec[2]) > fabs(vec[imax]))
    imax = 2;

  ortho[imax] = 0.0f;

  ortho = cross( vec, ortho ).normalize();
	mat.setColumnVec3(0, ortho);

  ortho = cross( vec, ortho );
	mat.setColumnVec3(1, ortho);

	mat.setColumnVec3(3, origin);

  return mat;
}

Matrix4 Matrix4::pointAt(const Vec3 &p,
														 const Vec3 &pointat,
														 const Vec3 &up)
{
	Vec3	xv, yv, zv;
	
	zv = p - pointat;
  if (zv.lengthManhattan() > MathConstants::EPSILON)
  	zv.normalize();

	xv = cross(up, zv);
	if (xv.lengthManhattan() > MathConstants::EPSILON)
    xv.normalize();
	
	yv = cross(zv, xv);
	if (yv.lengthManhattan() > MathConstants::EPSILON)
    yv.normalize();
	
  return Matrix4( xv, yv, zv, p );
}
