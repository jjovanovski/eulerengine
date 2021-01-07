#pragma once

#include "../API.h"
#include "../math/Vec3.h"

namespace Euler
{
	namespace Graphics
	{
		class EULER_API DirectionalLight
		{
		public:
			alignas(16) Vec3 Direction;
			alignas(16) Vec3 Color;
			float Intensity;

			DirectionalLight();
			DirectionalLight(Vec3 direction, Vec3 color, float intensity);
		};
	}
}