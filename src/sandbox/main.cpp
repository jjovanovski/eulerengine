#include <iostream>

#include "App.h"
#include "graphics/Mesh.h"
#include "graphics/ModelPipeline.h"
#include "graphics/Camera.h"
#include "graphics/Vertex.h"
#include "graphics/Texture.h"
#include "graphics/DirectionalLight.h"
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
		_modelPipeline.AmbLight.Intensity = 0.05f;

		// create camera
		_camera.Init(1920, 1080, 60.0f, 0.01f, 100.0f);
		_camera.Transform.SetPosition(Vec3(0, 0, 2));
		_camera.Transform.SetRotation(Quaternion::Euler(Math::Rad(180.0f), Vec3(0, 1, 0)));

		// load texture
		int width, height, channels;
		stbi_uc* pixels = stbi_load("texture.jpg", &width, &height, &channels, STBI_rgb_alpha);

		_texture.Shininess = 2.0f;
		_texture.Create(Vulkan, pixels, width, height, width * height * 4, _modelPipeline.MaterialLayout);

		stbi_image_free(pixels);

		// create mesh
		std::vector<Vertex> triangleVertices = {
			Vertex(Vec3(-0.5f, -0.5f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec2(0.0f, 1.0f)),
			Vertex(Vec3(0.0f, 0.5f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec2(0.5f, 0.0f)),
			Vertex(Vec3(0.5f, -0.5f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec2(1.0f, 1.0f))
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
		_model.Position = Vec3(0, 0, 0);
		_model.Drawables.push_back(&_meshMaterial);
		_modelPipeline.Models.push_back(&_model);
	}

	void OnUpdate() override
	{

	}

	void OnDraw() override
	{
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