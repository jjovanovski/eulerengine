#include "App.h"

#include <iostream>
#include <vector>

#include "graphics/vulkan/Vulkan.h"

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

	// main loop
	while (!glfwWindowShouldClose(Window)) {
		while (WindowMinimized)
		{
			glfwWaitEvents();
		}

		vulkan.DrawFrame();
		glfwPollEvents();
	}
	
	// cleanup
	vkDestroySurfaceKHR(vulkan.GetInstance(), surface, nullptr);
	vulkan.Cleanup();

	glfwDestroyWindow(Window);
	glfwTerminate();
}