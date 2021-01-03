#pragma once

#include "../API.h"
#include "../math/Vec3.h"

namespace Euler
{
	namespace Graphics
	{
		class EULER_API AmbientLight
		{
		public:
			alignas(16) Vec3 CameraPosition;
			alignas(16) Vec3 Color;
			float Intensity;

			AmbientLight();
			AmbientLight(Vec3 cameraPosition, Vec3 color, float intensity);
		};
	}
}