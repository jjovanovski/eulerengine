#pragma once

#include "../API.h"

#include <stdint.h>
#include <math.h>

namespace Euler
{
	namespace Math
	{
		template<typename T>
		struct EULER_API Vector2Template
		{
			T x;
			T y;

			Vector2Template() : x(0), y(0) {}

			Vector2Template(T px, T py) : x(px), y(py){}
		};
	}
}

typedef Euler::Math::Vector2Template<float_t> Vec2;
typedef Euler::Math::Vector2Template<double_t> Vec2d;
typedef Euler::Math::Vector2Template<int32_t> Vec2i;
typedef Euler::Math::Vector2Template<uint32_t> Vec2ui;
typedef Euler::Math::Vector2Template<int64_t> Vec2i64;
typedef Euler::Math::Vector2Template<uint64_t> Vec2ui64;