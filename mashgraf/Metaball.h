#pragma once
#include "Vector.h"
class Metaball
{
public:
	double3 position;
	float squaredRadius;

	Metaball(double3 _position = double3(0, 0, 0), float _radius = 0)
	{
		position = _position;
		squaredRadius = _radius;
	}
};