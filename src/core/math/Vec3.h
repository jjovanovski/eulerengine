#pragma once

#include <stdint.h>
#include <math.h>

namespace Euler
{
	namespace Math
	{
		template<typename T>
		struct Vector3Template
		{
			T x;
			T y;
			T z;

			Vector3Template() : x(0), y(0), z(0) {}

			Vector3Template(T px, T py, T pz) : x(px), y(py), z(pz) {}
		};
	}
}

typedef Euler::Math::Vector3Template<float_t> Vec3;
typedef Euler::Math::Vector3Template<double_t> Vec3d;
typedef Euler::Math::Vector3Template<int32_t> Vec3i;
typedef Euler::Math::Vector3Template<uint32_t> Vec3ui;
typedef Euler::Math::Vector3Template<int64_t> Vec3i64;
typedef Euler::Math::Vector3Template<uint64_t> Vec3ui64;