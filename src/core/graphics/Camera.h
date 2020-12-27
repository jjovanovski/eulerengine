#pragma once

#include "../API.h"
#include "../math/Vec3.h"
#include "Common.h"

namespace Euler
{
	class EULER_API Camera
	{
	private:
		uint32_t _width;
		uint32_t _height;
		float _fieldOfView;
		float _nearZ;
		float _farZ;

		ViewProj _viewProj;

	public:
		Vec3 Position;
		Vec3 Rotation;

		void Init(uint32_t width, uint32_t height, float fieldOfView, float nearZ, float farZ);
		ViewProj GetViewProj();
		void LookAt(Vec3 point);
	};
}