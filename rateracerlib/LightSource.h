#pragma once

#include "mathlib.h"

// TODO: amb, diff, spec, attenuation
// TODO: spot light - direction, exponent, cutoffAngle, (inner + outer cone)
// TODO: (global ambient)

class LightSource
{
public:
	LightSource( Vec3 Position = Vec3(0,0,0), Vec3 Color = Vec3(0.5f,0.5f,0.5f) )
	{
		position = Position;
		color = Color;
	}

	virtual ~LightSource() {}

	Vec3 position;
	Vec3 color;
};
