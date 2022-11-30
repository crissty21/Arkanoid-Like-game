#include "Vector2d.h"

Vector2d::Vector2d()
{
	X = 0; Y = 0;
}

Vector2d::Vector2d(float x, float y)
{
	X = x; Y = y;
}

Vector2d::Vector2d(Vector2d* theNewOne)
{
	Vector2d(theNewOne->X, theNewOne->Y);
}

Vector2d* Vector2d::Add(Vector2d* second)
{
	return new Vector2d(X + second->X, Y + second->Y);
}

double Vector2d::Dot(Vector2d* second)
{
	return X * second->X + Y * second->Y;
}

Vector2d* Vector2d::Multiply(double value)
{
	return new Vector2d(X * value, Y * value);
}

Vector2d* Vector2d::Normalize()
{
	double dis = Dist();
	
	return new Vector2d(X / dis, Y / dis);
}

Vector2d* Vector2d::GetLookAtPosition(Vector2d* Position)
{
	//subtract form position current Location and normalize
	Vector2d* direction = Position->Add(Negate());
	return direction->Normalize();
}

Vector2d* Vector2d::Negate()
{
	return new Vector2d(-X, -Y);
}

float Vector2d::Dist()
{
	return sqrt(X * X + Y * Y);
}


