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

	Vec3 direction = _camera->Position;
	direction.x += cosf(_yaw) * cosf(_pitch);
	direction.y += sinf(_pitch);
	direction.z += sinf(_yaw) * cosf(_pitch);
	
	_camera->LookAt(direction);

	// wasd movement
	Vec3 forward = Vec3(direction.x - _camera->Position.x, direction.y - _camera->Position.y, direction.z - _camera->Position.z).Normalized();
	Vec3 right = Vec3(0, 1, 0).Cross(forward).Normalized();

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
		_camera->Position.x -= right.x * _movementSpeed * speedMultiplier;
		_camera->Position.y -= right.y * _movementSpeed * speedMultiplier;
		_camera->Position.z -= right.z * _movementSpeed * speedMultiplier;
	}
	else if (Input::GetKeyDown(Key::D))
	{
		_camera->Position.x += right.x * _movementSpeed * speedMultiplier;
		_camera->Position.y += right.y * _movementSpeed * speedMultiplier;
		_camera->Position.z += right.z * _movementSpeed * speedMultiplier;
	}

	if (Input::GetKeyDown(Key::W))
	{
		_camera->Position.x += forward.x * _movementSpeed * speedMultiplier;
		_camera->Position.y += forward.y * _movementSpeed * speedMultiplier;
		_camera->Position.z += forward.z * _movementSpeed * speedMultiplier;
	}
	else if (Input::GetKeyDown(Key::S))
	{
		_camera->Position.x -= forward.x * _movementSpeed * speedMultiplier;
		_camera->Position.y -= forward.y * _movementSpeed * speedMultiplier;
		_camera->Position.z -= forward.z * _movementSpeed * speedMultiplier;
	}
}