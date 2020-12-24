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
	//texture.Create(&vulkan, pixels, width, height, width * height * 4, VK_NULL_HANDLE);

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

	Mat4 mat;
	auto minOffset = vulkan._physicalDevice->Properties.limits.minUniformBufferOffsetAlignment;
	auto alignment = (sizeof(mat) + minOffset - 1) & ~(minOffset - 1);

	// write dynamic model matrices
	for (int i = 0; i < modelBuffers.size(); i++)
	{
		Mat4 model;
		vulkan.CreateBuffer(models.size() * alignment, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, modelBuffers[i], modelMemories[i]);

		int j = 0;
		for (auto& model : models)
		{
			Mat4 modelMatrix = model.GetModelMatrix();
			modelMatrix.Transpose();
			void* data;
			vkMapMemory(vulkan._device, modelMemories[i], j * alignment, alignment, 0, &data);
			memcpy(data, &modelMatrix, sizeof(modelMatrix));
			memset(static_cast<char*>(data) + sizeof(modelMatrix) + 1, 0, alignment - sizeof(modelMatrix));
			vkUnmapMemory(vulkan._device, modelMemories[i]);

			j++;
		}
	}

	std::vector<VkDescriptorSetLayout> modelLayouts(vulkan.GetSwapchainImageCount(), vulkan._modelLayout);
	vulkan._modelSets.resize(vulkan.GetSwapchainImageCount());
	VkDescriptorSetAllocateInfo modelAllocInfo{};
	modelAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	modelAllocInfo.descriptorPool = vulkan._descriptorPool;
	modelAllocInfo.descriptorSetCount = static_cast<uint32_t>(vulkan.GetSwapchainImageCount());
	modelAllocInfo.pSetLayouts = modelLayouts.data();

	vkAllocateDescriptorSets(vulkan._device, &modelAllocInfo, vulkan._modelSets.data());

	for (int i = 0; i < vulkan._modelSets.size(); i++)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = modelBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = VK_WHOLE_SIZE;

		VkWriteDescriptorSet writeUbo{};
		writeUbo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeUbo.dstSet = vulkan._modelSets[i];
		writeUbo.dstBinding = 0;
		writeUbo.dstArrayElement = 0;
		writeUbo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		writeUbo.descriptorCount = 1;
		writeUbo.pBufferInfo = &bufferInfo;

		VkWriteDescriptorSet writes[] = { writeUbo };

		vkUpdateDescriptorSets(vulkan._device, 1, writes, 0, nullptr);
	}

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

		//uint32_t i = 0;
		//Mat4 m;
		//for (auto& model : models)
		//{
		//	uint32_t offset = i * alignment;
		//	vkCmdBindDescriptorSets(
		//		*vulkan.GetMainCommandBuffer(),
		//		VK_PIPELINE_BIND_POINT_GRAPHICS,
		//		vulkan._graphicsPipelineLayout,
		//		2,
		//		1,
		//		&vulkan._modelSets[vulkan._currentImage],
		//		1,
		//		&offset
		//	);
		//	model.Meshes[0]->RecordDrawCommands(&vulkan, *vulkan.GetMainCommandBuffer());

		//	i++;
		//}

		vulkan.EndDrawFrame();

		glfwPollEvents();
	}

	for (int i = 0; i < modelBuffers.size(); i++)
	{
		vulkan.DestroyBuffer(modelBuffers[i], modelMemories[i]);
	}

	plane.Destroy(&vulkan);
	
	modelPipeline.Destroy();

	// cleanup
	vkDestroySurfaceKHR(vulkan.GetInstance(), surface, nullptr);
	vulkan.Cleanup();

	glfwDestroyWindow(Window);
	glfwTerminate();
}