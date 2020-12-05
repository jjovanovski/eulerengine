#pragma once

#include "../API.h"

#include <stdint.h>
#include <math.h>

namespace Euler
{
	namespace Math
	{
		template<typename T>
		struct EULER_API Mat4
		{
			T m[4][4] = { 0 };

			T Get(int i, int j)
			{
				return m[i][j];
			}

			void Set(int i, int j, T value)
			{
				m[i][j] = value;
			}

			void Transpose()
			{
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
		};
	}
}

typedef Euler::Math::Mat4<float_t> Mat4;
typedef Euler::Math::Mat4<double_t> Mat4d;
typedef Euler::Math::Mat4<int32_t> Mat4i;
typedef Euler::Math::Mat4<uint32_t> Mat4ui;
typedef Euler::Math::Mat4<int64_t> Mat4i64;
typedef Euler::Math::Mat4<uint64_t> Mat4ui64;