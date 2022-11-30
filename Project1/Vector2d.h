#pragma once
#include <cmath>

class Vector2d
{
public:
	Vector2d();
	Vector2d(float x, float y);
	Vector2d(Vector2d* theNewOne);
	Vector2d* Add(Vector2d* second);
	double Dot(Vector2d* second);
	Vector2d* Multiply(double value);
	Vector2d* Normalize();
	Vector2d* GetLookAtPosition(Vector2d* Position);
	Vector2d* Negate();
	float Dist();
	float X;
	float Y;
};