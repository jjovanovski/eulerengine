#pragma once

#include <stdint.h>
#include <math.h>

namespace Euler
{
	namespace Math
	{

		template<typename T>
		struct Vector2Template
		{
			T x;
			T y;
			T z;

			Vector2Template() : x(0), y(0), z(0) {}
		};
	}
}

typedef Euler::Math::Vector2Template<float_t> Vec2;
typedef Euler::Math::Vector2Template<double_t> Vec2d;
typedef Euler::Math::Vector2Template<int32_t> Vec2i;
typedef Euler::Math::Vector2Template<uint32_t> Vec2ui;
typedef Euler::Math::Vector2Template<int64_t> Vec2i64;
typedef Euler::Math::Vector2Template<uint64_t> Vec2ui64;