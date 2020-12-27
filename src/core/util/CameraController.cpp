#include "CameraController.h"

#include "../input/Input.h"
#include <math.h>

using namespace Euler;

void CameraController::Init(Camera* camera, float mouseSensitivityX, float mouseSensitivityY, float movementSpeed)
{
	_camera = camera;
	_mouseSensitivityX = mouseSensitivityX;
	_mouseSensitivityY = mouseSensitivityY;
	_movementSpeed = movementSpeed;
}

void CameraController::Update()
{
	// wasd movement
	Vec3 forward = _camera->Transform.Forward();
	Vec3 right = _camera->Transform.Right();

	float speedMultiplier = 1.0f;
	if (Input::GetKeyDown(Key::SHIFT))
	{
		speedMultiplier = 2.0f;
	}
	else
	{
		speedMultiplier = 1.0f;
	}

	if (Input::GetKeyDown(Key::A))
	{
		_camera->Transform.SetPosition(
			_camera->Transform.GetPosition().x - right.x * _movementSpeed * speedMultiplier,
			_camera->Transform.GetPosition().y - right.y * _movementSpeed * speedMultiplier,
			_camera->Transform.GetPosition().z - right.z * _movementSpeed * speedMultiplier
		);
	}
	else if (Input::GetKeyDown(Key::D))
	{
		_camera->Transform.SetPosition(
			_camera->Transform.GetPosition().x + right.x * _movementSpeed * speedMultiplier,
			_camera->Transform.GetPosition().y + right.y * _movementSpeed * speedMultiplier,
			_camera->Transform.GetPosition().z + right.z * _movementSpeed * speedMultiplier
		);
	}

	if (Input::GetKeyDown(Key::W))
	{
		_camera->Transform.SetPosition(
			_camera->Transform.GetPosition().x + forward.x * _movementSpeed * speedMultiplier,
			_camera->Transform.GetPosition().y + forward.y * _movementSpeed * speedMultiplier,
			_camera->Transform.GetPosition().z + forward.z * _movementSpeed * speedMultiplier
		);
	}
	else if (Input::GetKeyDown(Key::S))
	{
		_camera->Transform.SetPosition(
			_camera->Transform.GetPosition().x - forward.x * _movementSpeed * speedMultiplier,
			_camera->Transform.GetPosition().y - forward.y * _movementSpeed * speedMultiplier,
			_camera->Transform.GetPosition().z - forward.z * _movementSpeed * speedMultiplier
		);
	}
	_camera->Transform.CheckModelMatrix();

	// mouse movement
	float mouseDeltaX = _mouseX - Input::GetMouseX();
	float mouseDeltaY = _mouseY - Input::GetMouseY();

	_mouseX = Input::GetMouseX();
	_mouseY = Input::GetMouseY();

	_yaw += mouseDeltaX * _mouseSensitivityX;
	_pitch -= mouseDeltaY * _mouseSensitivityY;
	if (_pitch > 89.0f * (3.14159265359f / 180.0f))
		_pitch = 89.0f * (3.14159265359f / 180.0f);
	if (_pitch < -89.0f * (3.14159265359f / 180.0f))
		_pitch = -89.0f * (3.14159265359f / 180.0f);

	Vec3 direction = _camera->Transform.GetPosition();
	direction.x += cosf(_yaw) * cosf(_pitch);
	direction.y += sinf(_pitch);
	direction.z += sinf(_yaw) * cosf(_pitch);

	_camera->Transform.LookAt(direction);
}