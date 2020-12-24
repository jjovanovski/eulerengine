#include "App.h"

#include <iostream>
#include <vector>

#include "graphics/vulkan/Vulkan.h"
#include "graphics/Vertex.h"
#include "graphics/Mesh.h"
#include "graphics/Model.h"
#include "graphics/ModelPipeline.h"
#include "graphics/Texture.h"
#include "math/Vec3.h"
#include "math/Vec2.h"

#include "stb_image.h"

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
	const int WIDTH = 1200;
	const int HEIGHT = 800;

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	Window = glfwCreateWindow(WIDTH, HEIGHT, "Euler Engine", nullptr, nullptr);
	
	glfwSetWindowUserPointer(Window, this);
	glfwSetFramebufferSizeCallback(Window, framebufferResized);

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

	int width, height, channels;
	stbi_uc* pixels = stbi_load("texture.jpg", &width, &height, &channels, STBI_rgb_alpha);
	
	Graphics::Texture texture;
	texture.Create(&vulkan, pixels, width, height, width * height * 4, modelPipeline.ColorTextureLayout);

	stbi_image_free(pixels);

	std::vector<Vertex> vertices = {
		Vertex(Vec3(-1.0f, +1.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), Vec2(0.0f, 1.0f)),
		Vertex(Vec3(-1.0f, -1.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f), Vec2(0.0f, 0.0f)),
		Vertex(Vec3(+1.0f, -1.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec2(1.0f, 0.0f)),
		Vertex(Vec3(+1.0f, +1.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f), Vec2(1.0f, 1.0f))
	};

	std::vector<uint32_t> indices = {
		0, 1, 2,
		0, 2, 3,

		4, 5, 6,
		4, 6, 7
	};

	Mesh plane;
	plane.Vertices = vertices;
	plane.Indices = indices;
	plane.Texture = &texture;
	plane.Create(&vulkan);

	std::vector<VkBuffer> modelBuffers;
	modelBuffers.resize(vulkan.GetSwapchainImageCount());
	std::vector<VkDeviceMemory> modelMemories;
	modelMemories.resize(vulkan.GetSwapchainImageCount());

	std::vector<Model> models;

	Model m1;
	m1.Position = Vec3(1, 0, 1);
	m1.Meshes.push_back(&plane);
	models.push_back(m1);

	Model m2;
	m2.Meshes.push_back(&plane);
	models.push_back(m2);

	modelPipeline.Models.push_back(&m1);
	modelPipeline.Models.push_back(&m2);

	// main loop
	while (!glfwWindowShouldClose(Window)) {
		while (WindowMinimized)
		{
			glfwWaitEvents();
		}

		m1.Position.x += 0.0001f;
		m2.Position.y += 0.0001f;

		vulkan.BeginDrawFrame();
		modelPipeline.RecordCommands();
		vulkan.EndDrawFrame();

		glfwPollEvents();
	}

	// cleanup
	texture.Destroy();
	plane.Destroy(&vulkan);
	modelPipeline.Destroy();

	vkDestroySurfaceKHR(vulkan.GetInstance(), surface, nullptr);
	vulkan.Cleanup();

	glfwDestroyWindow(Window);
	glfwTerminate();
}