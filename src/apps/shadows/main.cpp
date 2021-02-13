#include <iostream>

#include "App.h"
#include "graphics/Mesh.h"
#include "graphics/ModelPipeline.h"
#include "graphics/Camera.h"
#include "graphics/Vertex.h"
#include "graphics/Texture.h"
#include "graphics/DirectionalLight.h"
#include "graphics/Animator.h"
#include "math/Math.h"
#include "resources/TextureResource.h"
#include "resources/ModelResource.h"
#include "graphics/Shadows.h"

#include "stb_image.h"

#include <vector>

using namespace Euler;

class SandboxApp : public App
{
private:	
	Graphics::ModelPipeline _modelPipeline;
	Graphics::DirectionalLight _dirLight;
	Camera _camera;
	
	ModelResource _cubeModelRes;
	ModelResource _planeModelRes;

	Graphics::Texture _woodTexture;
	
	Mesh _cubeMesh;
	Graphics::MeshMaterial _cubeMeshMat;

	Model _cubeModel;
	Model _nearCubeModel;
	Model _floorModel;

	Graphics::Shadows _shadows;

	float _rot = 0;

public:
	void OnCreate() override
	{
		_modelPipeline.Create(Vulkan, 1920, 1080);

		// setup light
		_dirLight.Direction = Vec3(1, 1, 1);
		_dirLight.Color = Vec3(1, 1, 1);
		_dirLight.Intensity = 1.0f;
		_modelPipeline.DirLight = &_dirLight;
		_modelPipeline.AmbLight.Color = Vec3(1, 1, 1);
		_modelPipeline.AmbLight.Intensity = 0.1f;

		// create camera
		_camera.Init(1920, 1080, 60.0f, 0.01f, 100.0f);
		_camera.Transform.SetPosition(Vec3(0, 0, 2));
		_camera.Transform.SetRotation(Quaternion::Euler(Math::Rad(180.0f), Vec3(0, 1, 0)));

		// load models
		_cubeModelRes.Load("res/cube.bem");
		_planeModelRes.Load("res/plane.bem");

		// load texture
		TextureResource textureResource;
		textureResource.Load("res/wood.jpg", TEXTURE_CHANNELS_RGBA);
		_woodTexture.Shininess = 2.0f;
		_woodTexture.Create(Vulkan, &textureResource, _modelPipeline.MaterialLayout);
		textureResource.Unload();

		// create mesh material
		_cubeMesh.Vertices = _cubeModelRes.Vertices;
		_cubeMesh.Indices = _cubeModelRes.Indices;
		_cubeMesh.Create(Vulkan);

		_cubeMeshMat.Mesh = &_cubeMesh;
		_cubeMeshMat.Texture = &_woodTexture;

		_cubeModel.Transform.SetPosition(Vec3(0, -0.5f, 0));
		_cubeModel.Transform.SetScale(0.1f);
		_cubeModel.Transform.SetRotation(Quaternion::Euler(Math::Rad(45.0f), Vec3(0, 1, 1)));
		_cubeModel.Drawables.push_back(&_cubeMeshMat);
		_modelPipeline.Models.push_back(&_cubeModel);

		_nearCubeModel.Transform.SetPosition(Vec3(-0.5f, -0.5f, 0.5f));
		_nearCubeModel.Transform.SetScale(0.1f);
		_nearCubeModel.Transform.SetRotation(Quaternion::Euler(Math::Rad(45.0f), Vec3(0, 1, 1)));
		_nearCubeModel.Drawables.push_back(&_cubeMeshMat);
		_modelPipeline.Models.push_back(&_nearCubeModel);
		
		_floorModel.Transform.SetPosition(Vec3(0, -0.85f, 0));
		_floorModel.Transform.SetScale(1.0f, 0.1f, 1.0f);
		_floorModel.Drawables.push_back(&_cubeMeshMat);
		_modelPipeline.Models.push_back(&_floorModel);

		// setup shadows
		_shadows.Create(Vulkan, &_modelPipeline, 1920, 1080);
	}

	void OnUpdate() override
	{
		_rot += 0.001f;
		_cubeModel.Transform.SetRotation(Quaternion::Euler(_rot, Vec3(-1, 1, -1).Normalized()));
		_nearCubeModel.Transform.SetRotation(Quaternion::Euler(_rot/2.0f, Vec3(1, -1, 1).Normalized()));
	}

	void OnDraw() override
	{
		_modelPipeline.Update(&_camera, _camera.GetViewProj());
		_shadows.RecordCommands(_camera);
		_modelPipeline.RecordCommands(_camera.GetViewProj());
	}

	void OnDestroy() override
	{
		_shadows.Destroy();

		_cubeModelRes.Unload();
		_planeModelRes.Unload();

		_cubeMesh.Destroy(Vulkan);
		_woodTexture.Destroy();

		_modelPipeline.Destroy();
	}
};

int main(int argc, char** argv)
{	
	SandboxApp app;
	app.Run();

	return 0;
}