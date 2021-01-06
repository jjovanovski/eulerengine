#pragma once

#include "../API.h"

#define PI 3.14159265359f
#define Rad2Deg 180.0f/PI
#define Deg2Rad PI/180.0f

namespace Euler
{
	namespace Math
	{
		/* === Epsilon and epsilon-comparison */
		float Epsilon();
		bool AlmostEqual(float a, float b);

		/* === Angle unit conversion === */
		float Deg(float radians);
		float Rad(float degrees);

		/* === Math function wrappers === */
		float Sin(float radians);
		float Cos(float radians);
		float Tan(float radians);
		float Pow(float base, float exponent);
		float Sqrt(float x);

		float Abs(float x);
		int Abs(int x);

		float Min(float a, float b);
		int Min(int a, int b);

		float Max(float a, float b);
		int Max(int a, int b);

		float Clamp(float value, float min, float max);
		int Clamp(int value, int min, int max);

		float Lerp(float a, float b, float t);
		float LerpUnclamped(float a, float b, float t);
	}
}