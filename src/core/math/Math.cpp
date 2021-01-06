#include "Math.h"

#include <math.h>
#include <algorithm>

float Euler::Math::Epsilon()
{
	return FLT_EPSILON;
}

bool Euler::Math::AlmostEqual(float a, float b)
{
	return Abs(a - b) <= Epsilon();
}

float Euler::Math::Deg(float radians)
{
	return radians * Rad2Deg;
}

float Euler::Math::Rad(float degrees)
{
	return degrees * Deg2Rad;
}

float Euler::Math::Sin(float radians)
{
	return sinf(radians);
}

float Euler::Math::Cos(float radians)
{
	return cosf(radians);
}

float Euler::Math::Tan(float radians)
{
	return tanf(radians);
}

float Euler::Math::Pow(float base, float exponent)
{
	return powf(base, exponent);
}

float Euler::Math::Sqrt(float x)
{
	return sqrtf(x);
}

float Euler::Math::Abs(float x)
{
	return abs(x);
}

int Euler::Math::Abs(int x)
{
	return abs(x);
}

float Euler::Math::Min(float a, float b)
{
	return std::min(a, b);
}

int Euler::Math::Min(int a, int b)
{
	return std::min(a, b);
}

float Euler::Math::Max(float a, float b)
{
	return std::max(a, b);
}

int Euler::Math::Max(int a, int b)
{
	return std::max(a, b);
}

float Euler::Math::Clamp(float value, float min, float max)
{
	return Max(min, Min(value, max));
}

int Euler::Math::Clamp(int value, int min, int max)
{
	return Max(min, Min(value, max));
}

float Euler::Math::Lerp(float a, float b, float t)
{
	return Clamp(LerpUnclamped(a, b, t), a, b);
}

float Euler::Math::LerpUnclamped(float a, float b, float t)
{
	return a * (1.0f - t) + b * t;
}