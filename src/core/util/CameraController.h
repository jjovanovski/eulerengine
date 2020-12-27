#pragma once

#include "../API.h"
#include "../graphics/Camera.h"

namespace Euler
{
	class EULER_API CameraController
	{
	private:
		Camera* _camera;

		float _mouseX = 0, _mouseY = 0;
		float _cameraRotX = 3.14159265359f, _cameraRotY = 3.14159265359f/2.0f;
		float _movementSpeed, _mouseSensitivityX, _mouseSensitivityY;
		float _yaw = 3.14159265359f/2.0f, _pitch = (3.14159265359f/4.0f);

	public:
		void Init(Camera* camera, float mouseSensitivityX = 0.0025f, float mouseSensitivityY = 0.0025f, float movementSpeed = 0.0002f);
		void Update();
	};
}