#include <iostream>

#include "App.h"
#include "graphics/Mesh.h"
#include "graphics/AnimatedMesh.h"
#include "graphics/ModelPipeline.h"
#include "graphics/AnimatedModelPipeline.h"
#include "graphics/Camera.h"
#include "graphics/Vertex.h"
#include "graphics/Texture.h"
#include "graphics/Material.h"
#include "graphics/DirectionalLight.h"
#include "math/Math.h"
#include "resources/TextureResource.h"
#include "resources/ModelResource.h"
#include "input/Input.h"
#include "math/Math.h"
#include "graphics/ModelRenderer.h"
#include "graphics/Shadows.h"
#include "graphics/AnimatedShadows.h"
#include "graphics/Animator.h"
#include "resources/AnimatedModelResource.h"

#include "stb_image.h"

#include <vector>

using namespace Euler;

class SandboxApp : public App
{
private:
	// TODO: Add floor
	// TODO: Add walls
	// TODO: Add animated character
	// TODO: Add character movement
	// TODO: Add pickup
	// TODO: Add collisions

	Graphics::ModelPipeline _modelPipeline;
	Graphics::DirectionalLight _dirLight;
	Camera _camera;

	Graphics::AnimatedModelPipeline _animatedPipeline;

	Graphics::Shadows _shadows;
	Graphics::AnimatedShadows _animatedShadows;

	// lookaround
	float _cameraYaw = 0.0f;
	float _cameraPitch = 0.0f;
	float _lastMouseX = 0;
	float _lastMouseY = 0;
	bool _firstMouse = true;
	Quaternion _originalCameraRotation;

	// Floor
	Mesh _floorMesh;
	Graphics::Texture _floorTexture, _floorNormalMap;
	Graphics::Material _floorMaterial;
	Graphics::MeshMaterial _floorMeshMaterial;
	Model _floorModel;

	// Wall
	Mesh _wallMesh;
	Graphics::Texture _wallTexture, _wallNormalMap;
	Graphics::Material _wallMaterial;
	Graphics::MeshMaterial _wallMeshMaterial;
	Model _wallModel;

	// Char
	Animator _animator;
	Graphics::Texture _charTexture;
	AnimatedMesh _charMesh;
	Graphics::MeshMaterial _animatedMeshMaterial;
	AnimatedModel _charModel;

public:
	void OnCreate() override
	{
		_modelPipeline.Create(Vulkan, 1920, 1080);
		_animatedPipeline.Create(Vulkan, 1920, 1080);

		// setup light
		_dirLight.Direction = Vec3(0, -1, 1).Normalized();
		_dirLight.Color = Vec3(1, 1, 1);
		_dirLight.Intensity = 1.0f;
		_modelPipeline.DirLight = &_dirLight;
		_modelPipeline.AmbLight.Color = Vec3(1, 1, 1);
		_modelPipeline.AmbLight.Intensity = 0.1f;
		_animatedPipeline.DirLight = &_dirLight;
		_animatedPipeline.AmbLight.Color = Vec3(1, 1, 1);
		_animatedPipeline.AmbLight.Intensity = 0.1f;

		// create camera
		_camera.Init(1920, 1080, 60.0f, 0.01f, 100.0f);
		_camera.Transform.SetPosition(Vec3(0, 1, 2));
		_camera.Transform.SetRotation(Quaternion::Euler(Math::Rad(180.0f), Vec3(0, 1, 0)));
		_originalCameraRotation = _camera.Transform.GetRotation();

		SetupFloor();
		SetupWall();
		SetupChar();

		// setup shadows
		_shadows.Create(Vulkan, &_modelPipeline, 1920, 1080);
		_animatedShadows.Create(Vulkan, &_animatedPipeline, 1920, 1080, _shadows._shadowRenderPass);
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

		if (Input::GetKeyDown(Key::Q))
		{
			_dirLight.Direction.y += 0.001f;
			_dirLight.Direction.Normalize();
		}
		if (Input::GetKeyDown(Key::E))
		{
			_dirLight.Direction.y -= 0.001f;
			_dirLight.Direction.Normalize();
		}

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
		_animator.Update();
		_modelPipeline.Update(&_camera, _camera.GetViewProj());
		_animatedPipeline.Update(&_camera, _camera.GetViewProj(), _animator.BoneMatrices);

		_shadows.RecordCommands(_camera);
		_animatedShadows.RecordCommands(_camera);
		_modelPipeline.RecordCommands(_camera.GetViewProj());
		_animatedPipeline.RecordCommands(_camera.GetViewProj(), _animator.BoneMatrices);
	}

	void OnDestroy() override
	{
		_floorMesh.Destroy(Vulkan);

		_shadows.Destroy();

		_animatedPipeline.Destroy();
		_modelPipeline.Destroy();
	}

	void SetupFloor()
	{
		ModelResource modelResource;
		modelResource.Load("res/floor/floor.bem");

		_floorMesh.Vertices = modelResource.Vertices;
		_floorMesh.Indices = modelResource.Indices;
		_floorMesh.Create(Vulkan);

		modelResource.Unload();

		TextureResource textureResource;

		textureResource.Load("res/floor/floorTexture.png", TEXTURE_CHANNELS_RGBA);
		_floorTexture.Create(Vulkan, &textureResource, _modelPipeline.MaterialLayout);
		textureResource.Unload();

		textureResource.Load("res/floor/floorNormalMap.png", TEXTURE_CHANNELS_RGBA);
		_floorNormalMap .Create(Vulkan, &textureResource, _modelPipeline.MaterialLayout);
		textureResource.Unload();

		_floorMaterial.ColorMap = &_floorTexture;
		_floorMaterial.NormalMap = &_floorNormalMap;
		_floorMaterial.Properties.Shininess = 1.0f;
		_floorMaterial.Properties.UseNormalMap = 1.0f;
		_floorMaterial.Create(Vulkan, _modelPipeline.MaterialPropertiesLayout);
		
		_floorMeshMaterial.Material = &_floorMaterial;
		_floorMeshMaterial.Mesh = &_floorMesh;

		_floorModel.Drawables.push_back(&_floorMeshMaterial);
		_floorModel.Transform.SetRotation(Quaternion::Euler(Math::Rad(90.0f), Vec3(1, 0, 0)));

		_modelPipeline.Models.push_back(&_floorModel);
	}

	void SetupWall()
	{
		ModelResource modelResource;
		modelResource.Load("res/walls/walls.bem");

		_wallMesh.Vertices = modelResource.Vertices;
		_wallMesh.Indices = modelResource.Indices;
		_wallMesh.Create(Vulkan);

		modelResource.Unload();

		TextureResource textureResource;

		textureResource.Load("res/walls/wallsTexture.png", TEXTURE_CHANNELS_RGBA);
		_wallTexture.Create(Vulkan, &textureResource, _modelPipeline.MaterialLayout);
		textureResource.Unload();

		textureResource.Load("res/walls/wallsNormalMap.png", TEXTURE_CHANNELS_RGBA);
		_wallNormalMap.Create(Vulkan, &textureResource, _modelPipeline.MaterialLayout);
		textureResource.Unload();

		_wallMaterial.ColorMap = &_wallTexture;
		_wallMaterial.NormalMap = &_wallNormalMap;
		_wallMaterial.Properties.Shininess = 1.0f;
		_wallMaterial.Properties.UseNormalMap = 1.0f;
		_wallMaterial.Create(Vulkan, _modelPipeline.MaterialPropertiesLayout);

		_wallMeshMaterial.Material = &_wallMaterial;
		_wallMeshMaterial.Mesh = &_wallMesh;

		_wallModel.Drawables.push_back(&_wallMeshMaterial);
		_wallModel.Transform.SetRotation(Quaternion::Euler(Math::Rad(90.0f), Vec3(1, 0, 0)));

		_modelPipeline.Models.push_back(&_wallModel);
	}

	void SetupChar()
	{
		TextureResource textureResource;
		textureResource.Load("res/char/charTexture.png", TEXTURE_CHANNELS_RGBA);

		_charTexture.Shininess = 2.0f;
		_charTexture.Create(Vulkan, &textureResource, _animatedPipeline.MaterialLayout);

		textureResource.Unload();

		AnimatedModelResource modelResource;
		modelResource.Load("res/char/char.beam");

		_charMesh.Vertices = modelResource.Vertices;
		_charMesh.Indices = modelResource.Indices;
		_charMesh.Texture = &_charTexture;
		_charMesh.Create(Vulkan);

		_animatedMeshMaterial.AnimatedMesh = &_charMesh;
		_animatedMeshMaterial.ColorTexture = &_charTexture;

		_charModel.Transform.SetPosition(Vec3(0, 0.03f, 1));
		_charModel.Transform.SetScale(0.05f);
		_charModel.Transform.SetRotation(Quaternion::Euler(1.15f * PI, Vec3(0, 1, 0)) * Quaternion::Euler(PI / 2.0f, Vec3(1, 0, 0)));
		_charModel.Drawables.push_back(&_animatedMeshMaterial);

		_animatedPipeline.Models.push_back(&_charModel);

		_animator.Animation = modelResource.Animations[0];
		_animator.BoneParents = modelResource.BoneParents;
		_animator.BoneOffsetMatrices = modelResource.BoneOffsetMatrices;
		_animator.Start();
	}
};

int main(int argc, char** argv)
{	
	SandboxApp app;
	app.Run();

	return 0;
}