#pragma once

#pragma comment(lib,"OpenGl32.lib")
#pragma comment(lib,"GLU32.lib")

#include "mathlib.h"

void drawWorldAxes(float scale = 1.0f);

void drawMatrix(const Matrix4 *mat = NULL, float scale = 1, bool drawrings = true );

void drawAAWireBox(float xside = 1, float yside = 1, float zside = 1,
									 const Vec3& origin = Vec3(0,0,0));

void drawAAWireBox(const Vec3& size, const Vec3& origin = Vec3(0,0,0));


void drawTransparentSphere(const Vec3& centre, float radius);
