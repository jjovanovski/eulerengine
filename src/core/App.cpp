#include "App.h"

#include <iostream>
#include <vector>

#include "graphics/vulkan/Vulkan.h"
#include "graphics/Vertex.h"
#include "graphics/MeshMaterial.h"
#include "graphics/Mesh.h"
#include "graphics/Model.h"
#include "graphics/ModelPipeline.h"
#include "graphics/Texture.h"
#include "graphics/Common.h"
#include "graphics/Camera.h"
#include "math/Vec3.h"
#include "math/Vec2.h"
#include "math/Matrices.h"
#include "math/Math.h"
#include "input/GLFWInputHandler.h"
#include "input/Input.h"
#include "util/CameraController.h"

#include "stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

using namespace Euler;

void framebufferResized(GLFWwindow* window, int width, int height)
{
	App* app = (App*) glfwGetWindowUserPointer(window);
	app->Vulkan->SetWindowResized(width, height);

	if (width == 0 || height == 0)
	{
		app->WindowMinimized = true;
	}
	else
	{
		app->WindowMinimized = false;
	}
}

App::App()
{
	const int WIDTH = 1920;
	const int HEIGHT = 1080;

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	Window = glfwCreateWindow(WIDTH, HEIGHT, "Euler Engine", nullptr, nullptr);
	
	glfwSetWindowUserPointer(Window, this);
	glfwSetFramebufferSizeCallback(Window, framebufferResized);
	glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// init input
	GLFWInputHandler* glfwInputHandler = new GLFWInputHandler(Window);
	Input::Init(glfwInputHandler);

	// create vulkan instance
	std::vector<const char*> requiredInstanceExtensions;
	std::vector<const char*> requiredInstanceLayers;

	uint32_t glfwRequiredInstanceExtensions;
	const char** instanceExtensions = glfwGetRequiredInstanceExtensions(&glfwRequiredInstanceExtensions);
	for (int i = 0; i < glfwRequiredInstanceExtensions; i++)
	{
		requiredInstanceExtensions.push_back(instanceExtensions[i]);
	}

	Graphics::Vulkan vulkan;
	Vulkan = &vulkan;	// TODO: wtf?
	vulkan.CreateInstance("Sandbox", VK_MAKE_VERSION(0, 1, 0), requiredInstanceLayers, requiredInstanceExtensions);
	
	// create vulkan device
	VkSurfaceKHR surface;
	glfwCreateWindowSurface(vulkan.GetInstance(), Window, nullptr, &surface);
	VkPhysicalDeviceFeatures physicalDeviceFeatures{};
	std::vector<const char*> requiredDeviceLayers;
	std::vector<const char*> requiredDeviceExtensions;
	vulkan.CreateDevice(surface, &physicalDeviceFeatures, requiredDeviceLayers, requiredDeviceExtensions);
	
	// init renderer
	vulkan.InitRenderer(WIDTH, HEIGHT);

	Graphics::ModelPipeline modelPipeline;
	modelPipeline.Create(&vulkan, WIDTH, HEIGHT);

	// load texture
	int width, height, channels;
	stbi_uc* pixels = stbi_load("model/red_soda_texture.png", &width, &height, &channels, STBI_rgb_alpha);
	
	Graphics::Texture texture;
	texture.Create(&vulkan, pixels, width, height, width * height * 4, modelPipeline.ColorTextureLayout);

	stbi_image_free(pixels);

	pixels = stbi_load("texture.jpg", &width, &height, &channels, STBI_rgb_alpha);

	Graphics::Texture planeTexture;
	planeTexture.Create(&vulkan, pixels, width, height, width * height * 4, modelPipeline.ColorTextureLayout);

	stbi_image_free(pixels);


	// load model
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "model/soda_can.obj")) {
		throw std::runtime_error(warn + err);
	}

	std::vector<Vertex> modelVertices;
	std::vector<uint32_t> modelIndices;
	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			Vertex vert{};
			vert.Position.x = attrib.vertices[3 * index.vertex_index + 0];
			vert.Position.y = attrib.vertices[3 * index.vertex_index + 1];
			vert.Position.z = -attrib.vertices[3 * index.vertex_index + 2];

			vert.Normal.x = attrib.normals[3 * index.normal_index + 0];
			vert.Normal.y = attrib.normals[3 * index.normal_index + 1];
			vert.Normal.z = attrib.normals[3 * index.normal_index + 2];

			vert.UV.x = attrib.texcoords[2 * index.texcoord_index + 0];
			vert.UV.y = 1.0f - attrib.texcoords[2 * index.texcoord_index + 1];

			modelVertices.push_back(vert);
			modelIndices.insert(modelIndices.begin() , modelIndices.size());
		}
	}

	Mesh modelMesh;
	modelMesh.Vertices = modelVertices;
	modelMesh.Indices = modelIndices;
	modelMesh.Texture = &texture;
	modelMesh.Create(&vulkan);
	Graphics::MeshMaterial modelMeshMaterial(&modelMesh, &texture);

	std::vector<Vertex> vertices = {
		Vertex(Vec3(-1.0f, +1.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), Vec2(0.0f, 1.0f)),
		Vertex(Vec3(-1.0f, -1.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f), Vec2(0.0f, 0.0f)),
		Vertex(Vec3(+1.0f, -1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec2(1.0f, 0.0f)),
		Vertex(Vec3(+1.0f, +1.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), Vec2(1.0f, 1.0f))
	};

	std::vector<uint32_t> indices = {
		2, 1, 0,
		3, 2, 0,

		6, 5, 4,
		7, 6, 4
	};

	Mesh planeMesh;
	planeMesh.Vertices = vertices;
	planeMesh.Indices = indices;
	planeMesh.Texture = &planeTexture;
	planeMesh.Create(&vulkan);
	Graphics::MeshMaterial planeMeshMaterial(&planeMesh, &planeTexture);

	std::vector<VkBuffer> modelBuffers;
	modelBuffers.resize(vulkan.GetSwapchainImageCount());
	std::vector<VkDeviceMemory> modelMemories;
	modelMemories.resize(vulkan.GetSwapchainImageCount());

	std::vector<Model> models;

	Model m1;
	m1.Position = Vec3(0, 0, 0);
	m1.Scale = Vec3(0.5f, 0.5f, 0.5f);
	m1.Drawables.push_back(&planeMeshMaterial);
	models.push_back(m1);

	Model m2;
	m2.Position = Vec3(-1.85f, 0, 0);
	m2.Scale = Vec3(0.5f, 0.5f, 0.5f);
	m2.Drawables.push_back(&planeMeshMaterial);
	models.push_back(m2);

	Model modelModel;
	/*modelModel.Position.y = 0.5f;
	modelModel.Rotation.x = 270.0f * (3.14159265359f / 180.0f);
	modelModel.Rotation.y = 45.0f * 5 * (3.14159265359f / 180.0f);*/
	modelModel.Scale = Vec3(0.8f, 0.8f, 0.8f);
	modelModel.Drawables.push_back(&modelMeshMaterial);

	modelPipeline.Models.push_back(&modelModel);
	modelPipeline.Models.push_back(&m1);
	modelPipeline.Models.push_back(&m2);

	Camera camera;
	camera.Init(WIDTH, HEIGHT, 60.0f, 0.01f, 10000.0f);
	camera.Transform.SetPosition(Vec3(0, 0, 3));
	camera.Transform.SetRotation(Quaternion::Euler(Math::Rad(180.0f), Vec3(0, 1, 0)));
	
	CameraController cameraController;
	cameraController.Init(&camera);

	float t = 0.0f;
	
	// setup directional light
	Graphics::DirectionalLight directionalLight(Vec3(1, 1, -1).Normalized(), Vec3(1, 1, 1), 1.0f);
	modelPipeline.DirLight = &directionalLight;

	// main loop
	while (!glfwWindowShouldClose(Window)) {
		while (WindowMinimized)
		{
			glfwWaitEvents();
		}

		m1.Rotation.z += 0.0001f;
		m1.Scale.x = (sinf(3*m1.Rotation.z)*0.5f + 0.5f) * 0.25f + 0.25f;
		m1.Scale.y = m1.Scale.x;
		
		m2.Rotation.y += 0.001f;
	
		t += 0.0001f;
		//camera.Transform.SetRotation(Quaternion::Euler(Math::Rad(180.0f + Math::Sin(t)*25.0f), Vec3(0, 1, 0)));
		//camera.Transform.SetPosition(Math::Cos(t + PI/2.0f)*3, 0, Math::Sin(t + PI / 2.0f) * 3);
		//camera.Transform.LookAt(m2.Position, Vec3(0, -1, 0));
		//cameraController.Update();
		directionalLight.Direction = Vec3(Math::Cos(t), 1, Math::Sin(t));

		vulkan.BeginDrawFrame();
		modelPipeline.RecordCommands(camera.GetViewProj());
		vulkan.EndDrawFrame();

		glfwPollEvents();
	}

	// cleanup
	texture.Destroy();
	planeMesh.Destroy(&vulkan);
	modelMesh.Destroy(&vulkan);
	modelPipeline.Destroy();

	vkDestroySurfaceKHR(vulkan.GetInstance(), surface, nullptr);
	vulkan.Cleanup();

	Input::Dispose();

	glfwDestroyWindow(Window);
	glfwTerminate();
}