#include <iostream>

#include "App.h"
#include "graphics/Mesh.h"
#include "graphics/ModelPipeline.h"
#include "graphics/Camera.h"
#include "graphics/Vertex.h"
#include "graphics/Texture.h"
#include "graphics/DirectionalLight.h"
#include "math/Math.h"
#include "resources/TextureResource.h"
#include "resources/ModelResource.h"
#include "input/Input.h"
#include "math/Math.h"

#include "stb_image.h"

#include <vector>

using namespace Euler;

class SandboxApp : public App
{
private:
	Graphics::ModelPipeline _modelPipeline;
	Graphics::DirectionalLight _dirLight;
	Camera _camera;
	Graphics::Texture _texture;
	Mesh _mesh;
	Graphics::MeshMaterial _meshMaterial;
	Model _model;

	float _cameraYaw = 0.0f;
	float _cameraPitch = 0.0f;
	float _lastMouseX = 0;
	float _lastMouseY = 0;
	bool _firstMouse = true;
	Quaternion _originalCameraRotation;
	

public:
	void OnCreate() override
	{
		_modelPipeline.Create(Vulkan, 1920, 1080);

		// setup light
		_dirLight.Direction = Vec3(1, 1, -1);
		_dirLight.Color = Vec3(1, 1, 1);
		_dirLight.Intensity = 1.0f;
		_modelPipeline.DirLight = &_dirLight;
		_modelPipeline.AmbLight.Color = Vec3(1, 1, 1);
		_modelPipeline.AmbLight.Intensity = 0.5f;

		// create camera
		_camera.Init(1920, 1080, 60.0f, 0.01f, 100.0f);
		_camera.Transform.SetPosition(Vec3(0, 0, 2));
		_camera.Transform.SetRotation(Quaternion::Euler(Math::Rad(180.0f), Vec3(0, 1, 0)));
		_originalCameraRotation = _camera.Transform.GetRotation();

		// load texture
		TextureResource textureResource;
		textureResource.Load("texture.jpg", TEXTURE_CHANNELS_RGBA);

		_texture.Shininess = 2.0f;
		_texture.Create(Vulkan, &textureResource, _modelPipeline.MaterialLayout);

		textureResource.Unload();

		// create mesh
		std::vector<Vertex> triangleVertices = {
			Vertex(Vec3(-0.5f, -0.5f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(), Vec3(), Vec2(0.0f, 1.0f)),
			Vertex(Vec3(0.0f, 0.5f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(), Vec3(), Vec2(0.5f, 0.0f)),
			Vertex(Vec3(0.5f, -0.5f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(), Vec3(), Vec2(1.0f, 1.0f))
		};

		std::vector<uint32_t> triangleIndices = {
			2, 1, 0
		};

		_mesh.Vertices = triangleVertices;
		_mesh.Indices = triangleIndices;
		_mesh.Texture = &_texture;
		_mesh.Create(Vulkan);

		// create mesh material
		_meshMaterial.Mesh = &_mesh;
		_meshMaterial.Texture = &_texture;

		// create model
		_model.Drawables.push_back(&_meshMaterial);
		_modelPipeline.Models.push_back(&_model);
	}

	void OnUpdate() override
	{
		float mouseX = Input::GetMouseX();
		float mouseY = Input::GetMouseY();
		
		if (_firstMouse && mouseX != 0 && mouseY != 0)
		{
			_lastMouseX = mouseX;
			_lastMouseY = mouseY;
			_firstMouse = false;
		}

		float xoffset = mouseX - _lastMouseX;
		float yoffset = _lastMouseY - mouseY;
		_lastMouseX = mouseX;
		_lastMouseY = mouseY;

		float mouseSensitivity = 0.2f;
		xoffset *= mouseSensitivity;
		yoffset *= mouseSensitivity;

		_cameraYaw -= xoffset;
		_cameraPitch += yoffset;

		_camera.Transform.SetRotation(Quaternion::Euler(Math::Rad(_cameraPitch), Vec3(1, 0, 0)) * Quaternion::Euler(Math::Rad(_cameraYaw), Vec3(0, 1, 0)) * _originalCameraRotation);
		
		float movementSpeed = 0.001f;

		if (Input::GetKeyDown(Key::A))
		{
			_camera.Transform.SetPosition(_camera.Transform.GetPosition() - movementSpeed * _camera.Transform.Right());
		}

		if (Input::GetKeyDown(Key::D))
		{
			_camera.Transform.SetPosition(_camera.Transform.GetPosition() + movementSpeed * _camera.Transform.Right());
		}

		if (Input::GetKeyDown(Key::W))
		{
			_camera.Transform.SetPosition(_camera.Transform.GetPosition() + movementSpeed * _camera.Transform.Forward());
		}

		if (Input::GetKeyDown(Key::S))
		{
			_camera.Transform.SetPosition(_camera.Transform.GetPosition() - movementSpeed * _camera.Transform.Forward());
		}
	}

	void OnDraw() override
	{
		_modelPipeline.Update(&_camera, _camera.GetViewProj());
		_modelPipeline.RecordCommands(_camera.GetViewProj());
	}

	void OnDestroy() override
	{
		_mesh.Destroy(Vulkan);
		_modelPipeline.Destroy();
	}
};

int main(int argc, char** argv)
{	
	SandboxApp app;
	app.Run();

	return 0;
}