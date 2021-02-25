#include <iostream>

#include "App.h"
#include "graphics/AnimatedMesh.h"
#include "graphics/AnimatedModelPipeline.h"
#include "graphics/Camera.h"
#include "graphics/Vertex.h"
#include "graphics/Texture.h"
#include "graphics/DirectionalLight.h"
#include "graphics/Animator.h"
#include "math/Math.h"
#include "resources/TextureResource.h"
#include "resources/AnimatedModelResource.h"

#include "stb_image.h"

#include <vector>

using namespace Euler;

class SandboxApp : public App
{
private:
	Graphics::AnimatedModelPipeline _modelPipeline;
	Graphics::DirectionalLight _dirLight;
	Camera _camera;
	Graphics::Texture _texture;
	AnimatedMesh _mesh;
	Graphics::MeshMaterial _meshMaterial;
	AnimatedModel _model;
	AnimatedModelResource _modelResource;
	Animator _animator;
	

public:
	void OnCreate() override
	{
		_modelPipeline.Create(Vulkan, 1920, 1080);

		// setup light
		_dirLight.Direction = Vec3(1, 1, 1);
		_dirLight.Color = Vec3(1, 1, 1);
		_dirLight.Intensity = 0.5f;
		_modelPipeline.DirLight = &_dirLight;
		_modelPipeline.AmbLight.Color = Vec3(1, 1, 1);
		_modelPipeline.AmbLight.Intensity = 0.5f;

		// create camera
		_camera.Init(1920, 1080, 60.0f, 0.01f, 100.0f);
		_camera.Transform.SetPosition(Vec3(0, 0, 2));
		_camera.Transform.SetRotation(Quaternion::Euler(Math::Rad(180.0f), Vec3(0, 1, 0)));

		// load texture
		TextureResource textureResource;
		textureResource.Load("diffuse.png", TEXTURE_CHANNELS_RGBA);

		_texture.Shininess = 2.0f;
		_texture.Create(Vulkan, &textureResource, _modelPipeline.MaterialLayout);

		textureResource.Unload();

		// create mesh
		_modelResource.Load("model.beam");

		_mesh.Vertices = _modelResource.Vertices;
		_mesh.Indices = _modelResource.Indices;
		_mesh.Texture = &_texture;
		_mesh.Create(Vulkan);

		// create mesh material
		_meshMaterial.AnimatedMesh = &_mesh;
		_meshMaterial.Texture = &_texture;

		// create model
		_model.Transform.SetPosition(Vec3(0, -0.5f, 1));
		_model.Transform.SetScale(0.1f);
		_model.Transform.SetRotation(Quaternion::Euler(1.15f * PI, Vec3(0, 1, 0)) * Quaternion::Euler(PI / 2.0f, Vec3(1, 0, 0)));
		_model.Drawables.push_back(&_meshMaterial);
		_modelPipeline.Models.push_back(&_model);

		_animator.Animation = _modelResource.Animations[0];
		_animator.BoneParents = _modelResource.BoneParents;
		_animator.BoneOffsetMatrices = _modelResource.BoneOffsetMatrices;
		_animator.Start();
	}

	void OnUpdate() override
	{

	}

	void OnDraw() override
	{
		_animator.Update(); 
		//_modelPipeline.Update(&_camera, _camera.GetViewProj());
		_modelPipeline.RecordCommands(_camera.GetViewProj(), _animator.BoneMatrices);
	}

	void OnDestroy() override
	{
		_mesh.Destroy(Vulkan);
		_modelResource.Unload();
		_modelPipeline.Destroy();
	}
};

int main(int argc, char** argv)
{	
	SandboxApp app;
	app.Run();

	return 0;
}