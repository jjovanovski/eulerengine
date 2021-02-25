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
}

void App::Run()
{
	OnStart();

	const int WIDTH = 1920;
	const int HEIGHT = 1080;

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	Window = glfwCreateWindow(WIDTH, HEIGHT, "Euler Engine", nullptr, nullptr);

	glfwSetWindowUserPointer(Window, this);
	glfwSetFramebufferSizeCallback(Window, framebufferResized);
	//glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

	OnCreate();

	// main loop
	while (!glfwWindowShouldClose(Window)) {
		while (WindowMinimized)
		{
			glfwWaitEvents();
		}

		OnUpdate();

		vulkan.BeginDrawFrame();
		OnDraw();
		vulkan.EndDrawFrame();

		glfwPollEvents();
	}

	vkDestroySurfaceKHR(vulkan.GetInstance(), surface, nullptr);
	vulkan.Cleanup();

	Input::Dispose();

	//OnDestroy();

	glfwDestroyWindow(Window);
	glfwTerminate();
}