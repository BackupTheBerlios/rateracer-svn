#pragma once

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "mathlib.h"

class Camera
{
public:
	/*
	Camera( Vec3 Position = Vec3(0,0,0), Vec3 Color = Vec3(0.5f,0.5f,0.5f) )
	{
		position = Position;
		color = Color;
	}

	virtual ~Camera() {}
	*/

	Vec3 position;
	Vec3 color;
};
