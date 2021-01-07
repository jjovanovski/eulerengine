#pragma once

#include "../API.h"

#include <stdint.h>
#include <math.h>

namespace Euler
{
	namespace Math
	{
		template<typename T>
		struct EULER_API Vector4Template
		{
			T x;
			T y;
			T z;
			T w;

			Vector4Template() : x(0), y(0), z(0), w(0) {}

			Vector4Template(T px, T py, T pz, T pw) : x(px), y(py), z(pz), w(pw) {}

			float LengthSquared() const
			{
				return x * x + y * y + z * z + w * w;
			}

			float Length() const
			{
				return sqrtf(LengthSquared());
			}

			void Normalize()
			{
				float length = Length();
				x /= length;
				y /= length;
				z /= length;
				w /= length;
			}

			Vector4Template Normalized() const
			{
				Vector3Template normalized(x, y, z, w);
				normalized.Normalize();
				return normalized;
			}

			float Dot(const Vector4Template& other) const
			{
				return x * other.x + y * other.y + z * other.z + w * other.w;
			}
		};
	}
}

typedef Euler::Math::Vector4Template<float_t> Vec4;
typedef Euler::Math::Vector4Template<double_t> Vec4d;
typedef Euler::Math::Vector4Template<int32_t> Vec4i;
typedef Euler::Math::Vector4Template<uint32_t> Vec4ui;
typedef Euler::Math::Vector4Template<int64_t> Vec4i64;
typedef Euler::Math::Vector4Template<uint64_t> Vec4ui64;