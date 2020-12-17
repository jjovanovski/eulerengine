#pragma once

#include "../API.h"

#include "Mat4.h"
#include <stdint.h>

namespace Euler
{
	namespace Math
	{
		class EULER_API Matrices
		{
		public:
			static Mat4 Perspective(uint32_t width, uint32_t height, float fieldOfView, float nearZ, float farZ);
		};
	}
}