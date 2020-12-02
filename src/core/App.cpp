#include "App.h"

#include <iostream>
#include <vector>

#include "graphics/vulkan/Vulkan.h"

using namespace Euler;


App::App()
{
	const int WIDTH = 800;
	const int HEIGHT = 800;

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	_window = glfwCreateWindow(WIDTH, HEIGHT, "Euler Engine", nullptr, nullptr);


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
	vulkan.CreateInstance("Sandbox", VK_MAKE_VERSION(0, 1, 0), requiredInstanceLayers, requiredInstanceExtensions);
	
	// create vulkan device
	VkSurfaceKHR surface;
	glfwCreateWindowSurface(vulkan.GetInstance(), _window, nullptr, &surface);
	VkPhysicalDeviceFeatures physicalDeviceFeatures{};
	std::vector<const char*> requiredDeviceLayers;
	std::vector<const char*> requiredDeviceExtensions;
	vulkan.CreateDevice(surface, &physicalDeviceFeatures, requiredDeviceLayers, requiredDeviceExtensions);
	
	// init renderer
	vulkan.InitRenderer(WIDTH, HEIGHT);

	// main loop
	while (!glfwWindowShouldClose(_window)) {
		vulkan.DrawFrame();
		glfwPollEvents();
	}
	
	// cleanup
	vkDestroySurfaceKHR(vulkan.GetInstance(), surface, nullptr);
	vulkan.Cleanup();

	glfwDestroyWindow(_window);
	glfwTerminate();
}