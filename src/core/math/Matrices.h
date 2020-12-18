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
			static Mat4 Identity();

			static Mat4 Translate(float x, float y, float z);

			static Mat4 Scale(float s);
			static Mat4 Scale(float x, float y, float z);

			static Mat4 RotateX(float deg);
			static Mat4 RotateY(float deg);
			static Mat4 RotateZ(float deg);

			static Mat4 Perspective(uint32_t width, uint32_t height, float fieldOfView, float nearZ, float farZ);
		};
	}
}