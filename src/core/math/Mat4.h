#pragma once

#include "../API.h"
#include "Vec3.h"
#include "Vec4.h"

#include <stdint.h>
#include <math.h>

namespace Euler
{
	namespace Math
	{
		template<typename T>
		struct EULER_API Mat4t
		{
			T m[4][4] = { 0 };

			T Get(int i, int j) const
			{
				return m[i][j];
			}

			void Set(int i, int j, T value)
			{
				m[i][j] = value;
			}

			void Transpose()
			{
				// TODO: Manually unroll this statement
				for (int i = 0; i < 4; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						if (i > j)
						{
							T tmp = m[i][j];
							m[i][j] = m[j][i];
							m[j][i] = tmp;
						}
					}
				}
			}

			Mat4t Multiply(const Mat4t& other)
			{
				Mat4t m;
				for (int i = 0; i < 4; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						for (int k = 0; k < 4; k++)
						{
							m.Set(i, j, m.Get(i, j) + Get(i, k) * other.Get(k, j));
						}
					}
				}

				return m;
			}

			Vec3 Multiply(const Vec3& vec)
			{
				return Vec3(
					Get(0, 0) * vec.x + Get(0, 1) * vec.y + Get(0, 2) * vec.z,
					Get(1, 0) * vec.x + Get(1, 1) * vec.y + Get(1, 2) * vec.z,
					Get(2, 0) * vec.x + Get(2, 1) * vec.y + Get(2, 2) * vec.z
				);
			}

			Vec4 Multiply(const Vec4& vec)
			{
				return Vec4(
					Get(0, 0) * vec.x + Get(0, 1) * vec.y + Get(0, 2) * vec.z + Get(0, 3) * vec.w,
					Get(1, 0) * vec.x + Get(1, 1) * vec.y + Get(1, 2) * vec.z + Get(1, 3) * vec.w,
					Get(2, 0) * vec.x + Get(2, 1) * vec.y + Get(2, 2) * vec.z + Get(2, 3) * vec.w,
					Get(3, 0) * vec.x + Get(3, 1) * vec.y + Get(3, 2) * vec.z + Get(3, 3) * vec.w
				);
			}

			friend Vec3 operator*(Mat4t lhs, const Vec3& rhs)
			{
				return lhs.Multiply(rhs);
			}

			friend Vec4 operator*(Mat4t lhs, const Vec4& rhs)
			{
				return lhs.Multiply(rhs);
			}
		};
	}
}

typedef Euler::Math::Mat4t<float_t> Mat4;
typedef Euler::Math::Mat4t<double_t> Mat4d;
typedef Euler::Math::Mat4t<int32_t> Mat4i;
typedef Euler::Math::Mat4t<uint32_t> Mat4ui;
typedef Euler::Math::Mat4t<int64_t> Mat4i64;
typedef Euler::Math::Mat4t<uint64_t> Mat4ui64;