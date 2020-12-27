#pragma once

#include "../API.h"
#include "Vec3.h"
#include "Quaternion.h"
#include "Matrices.h"

namespace Euler
{
	class EULER_API Transform
	{
	private:
		Vec3 _position;
		Quaternion _rotation;
		Vec3 _scale;

		bool _dirty = false;
		Mat4 _modelMatrix = Math::Matrices::Identity();
		Vec3 _forward;
		Vec3 _right;
		Vec3 _top;

	public:
		Vec3 GetPosition();
		void SetPosition(Vec3 position);
		void SetPosition(float x, float y, float z);

		Quaternion GetRotation();
		void SetRotation(Quaternion quaternion);

		Vec3 GetScale();
		void SetScale(Vec3 scale);
		void SetScale(float x, float y, float z);
		void SetScale(float s);

		Mat4 GetModelMatrix();
		void CheckModelMatrix();

		Vec3 Forward();
		Vec3 Bacward();
		Vec3 Right();
		Vec3 Left();
		Vec3 Top();
		Vec3 Bottom();

		void LookAt(Vec3 point, Vec3 up = Vec3(0, 1, 0));
	};
}