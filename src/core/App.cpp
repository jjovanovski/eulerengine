#include "App.h"

#include <iostream>
#include <vector>

#include "graphics/vulkan/Vulkan.h"
#include "graphics/Vertex.h"
#include "graphics/Mesh.h"
#include "math/Vec3.h"
#include "math/Vec2.h"

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
	plane.Create(&vulkan);

	// main loop
	while (!glfwWindowShouldClose(Window)) {
		while (WindowMinimized)
		{
			glfwWaitEvents();
		}

		vulkan.BeginDrawFrame();
		plane.RecordDrawCommands(&vulkan, *vulkan.GetMainCommandBuffer());
		vulkan.EndDrawFrame();

		glfwPollEvents();
	}

	plane.Destroy(&vulkan);
	
	// cleanup
	vkDestroySurfaceKHR(vulkan.GetInstance(), surface, nullptr);
	vulkan.Cleanup();

	glfwDestroyWindow(Window);
	glfwTerminate();
}