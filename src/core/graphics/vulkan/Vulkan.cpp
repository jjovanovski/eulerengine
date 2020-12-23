#include "Vulkan.h"

using namespace Euler::Graphics;

#include <assert.h>
#include <iostream>
#include <algorithm>

#include "../../io/Utils.h"
#include "../../math/Matrices.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define ASSERT(exp)									\
{													\
	assert(exp);									\
}													\

#define LOG(title, msg)								\
{													\
	std::cout << title << " " << msg << std::endl;	\
}													\

#define HANDLE_VKRESULT(exp, title)					\
{													\
	VkResult result = (exp);						\
	if(result != VK_SUCCESS)						\
	{												\
		LOG(title, "failed");						\
		ASSERT(0);									\
	}												\
	else											\
	{												\
		LOG(title, "succeeded");					\
	}												\
}													\

Vulkan::Vulkan() {}

void Vulkan::InitRenderer(uint32_t width, uint32_t height)
{
	_extent = { width, height };

	CreateSwapchain();
	CreateRenderPass();
	CreatePipelineFixed();
	CreateDepthImage();
	CreateFramebuffers();
	CreateCommandPools();
	CreateUniformBuffer();
	CreateTexture();
	CreateDescriptorPool();
	AllocateCommandBuffers();
	CreateFrameSyncObjects();
}

void Vulkan::Cleanup()
{
	vkDeviceWaitIdle(_device);

	DestroyFrameSyncObjects();
	FreeCommandBuffers();
	DestroyDescriptorPool();
	DestroyTexture();
	DestroyUniformBuffer();
	DestroyCommandPools();
	DestroyFramebuffers();
	DestroyDepthImage();
	DestroyPipelineFixed();
	DestroyRenderPass();
	DestroySwapchain();
	DestroyDevice();
	DestroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);
	DestroyInstance();
}

void Vulkan::RecreateSwapchain(uint32_t width, uint32_t height)
{
	vkDeviceWaitIdle(_device);

	_extent = { width, height };

	FreeCommandBuffers();
	DestroyDescriptorPool();
	DestroyUniformBuffer();
	DestroyCommandPools();
	DestroyFramebuffers();
	DestroyDepthImage();
	DestroyPipelineFixed();
	DestroyRenderPass();
	DestroySwapchain();

	CreateSwapchain();
	CreateRenderPass();
	CreatePipelineFixed();
	CreateDepthImage();
	CreateFramebuffers();
	CreateCommandPools();
	CreateUniformBuffer();
	CreateDescriptorPool();
	AllocateCommandBuffers();
}

void Vulkan::SetWindowResized(uint32_t width, uint32_t height)
{
	_resizeWidth = width;
	_resizeHeight = height;
}

void Vulkan::CreateInstance(const char* appName, uint32_t appVersion, std::vector<const char*> requiredLayerNames, std::vector<const char*> requiredExtensionNames)
{
	requiredExtensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	requiredExtensionNames.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

	auto unsupportedLayers = FilterUnsupportedLayers(requiredLayerNames);
	if (requiredLayerNames.size() > 0 && unsupportedLayers.size() > 0)
	{
		for (auto layerName : unsupportedLayers)
		{
			LOG("Not supported layer: ", layerName);
		}

		ASSERT(0);
	}

	auto unsupportedExtensions = FilterUnsupportedExtensions(requiredExtensionNames);
	if (requiredExtensionNames.size() > 0 && unsupportedExtensions.size() > 0)
	{
		for (auto extensionName : unsupportedExtensions)
		{
			LOG("Not supported extension: ", extensionName);
		}

		ASSERT(0);
	}

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.engineVersion = VK_MAKE_VERSION(EULER_MAJOR, EULER_MINOR, EULER_PATCH);
	appInfo.pEngineName = EULER_ENGINE_NAME;
	appInfo.pApplicationName = appName;
	appInfo.applicationVersion = appVersion;
	appInfo.apiVersion = VK_VERSION_1_1;							// TODO: Allow the engine/user to set the Vulkan version

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IOS_SURFACE_CREATE_INFO_MVK;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = (uint32_t)requiredLayerNames.size();
	createInfo.ppEnabledLayerNames = requiredLayerNames.data();
	createInfo.enabledExtensionCount = (uint32_t)requiredExtensionNames.size();
	createInfo.ppEnabledExtensionNames = requiredExtensionNames.data();

	HANDLE_VKRESULT(vkCreateInstance(&createInfo, nullptr, &_instance), "CreateInstance");

	CreateDebugMessenger();
}

void Vulkan::DestroyInstance()
{
	vkDestroyInstance(_instance, nullptr);
	LOG("CreateInstance", "Destroyed");
}

VkInstance Vulkan::GetInstance()
{
	return _instance;
}

std::vector<VkLayerProperties> Vulkan::GetSupportedLayers()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> layers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

	return layers;
}

std::vector<VkExtensionProperties> Vulkan::GetSupportedExtensions()
{
	uint32_t extensionCount;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> supportedExtensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, supportedExtensions.data());

	return supportedExtensions;
}

std::vector<const char*> Vulkan::FilterUnsupportedLayers(const std::vector<const char*>& requiredLayerNames)
{
	auto supportedLayers = GetSupportedLayers();
	std::vector<const char*> unsupportedLayers;
	for (auto layerName : requiredLayerNames)
	{
		bool found = false;
		for (auto& layer : supportedLayers)
		{
			if (strcmp(layerName, layer.layerName) == 0)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			unsupportedLayers.push_back(layerName);
		}
	}

	return unsupportedLayers;
}

std::vector<const char*> Vulkan::FilterUnsupportedExtensions(const std::vector<const char*>& requiredExtensionNames)
{
	auto supportedExtensions = GetSupportedExtensions();
	std::vector<const char*> unsupportedExtensions;
	for (auto layerName : requiredExtensionNames)
	{
		bool found = false;
		for (auto& extension : supportedExtensions)
		{
			if (strcmp(layerName, extension.extensionName) == 0)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			unsupportedExtensions.push_back(layerName);
		}
	}

	return unsupportedExtensions;
}

void Vulkan::CreateDebugMessenger()
{
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
	createInfo.pUserData = nullptr; // Optional

	if (CreateDebugUtilsMessengerEXT(_instance, &createInfo, nullptr, &_debugMessenger) != VK_SUCCESS)
	{
		return;
	}
}

VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan::DebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData
)
{
	LOG("Vulkan Validation layers:", pCallbackData->pMessage);
	return VK_FALSE;
}

VkResult Vulkan::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void Vulkan::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

void Vulkan::CreateDevice(VkSurfaceKHR surface, VkPhysicalDeviceFeatures* enabledFeatures, std::vector<const char*> requiredDeviceLayerNames, std::vector<const char*> requiredDeviceExtensionNames)
{
	_surface = surface;

	// add required layers and extensions
	requiredDeviceLayerNames.push_back("VK_LAYER_KHRONOS_validation");
	requiredDeviceExtensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

	// add required features
	enabledFeatures->samplerAnisotropy = VK_TRUE;

	// get physical devices
	uint32_t physicalDevicesCount;
	vkEnumeratePhysicalDevices(_instance, &physicalDevicesCount, nullptr);
	std::vector<VkPhysicalDevice> physicalDeviceHandles(physicalDevicesCount);
	vkEnumeratePhysicalDevices(_instance, &physicalDevicesCount, physicalDeviceHandles.data());
	ASSERT(physicalDeviceHandles.size() > 0, "EnumeratePhysicalDevices");

	// get physical device properties
	_physicalDevices.resize(physicalDevicesCount);
	for (int i = 0; i < physicalDevicesCount; i++)
	{
		VkPhysicalDevice handle = physicalDeviceHandles[i];
		PhysicalDevice* physicalDevice = &_physicalDevices[i];
		physicalDevice->Handle = handle;

		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(handle, &properties);	
		physicalDevice->Properties = properties;

		uint32_t queueFamilyPropertyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(handle, &queueFamilyPropertyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(handle, &queueFamilyPropertyCount, queueFamilyProperties.data());
		physicalDevice->QueueFamilies.resize(queueFamilyPropertyCount);

		for (uint32_t i = 0; i < queueFamilyProperties.size(); i++)
		{
			VkQueueFamilyProperties familyProperties = queueFamilyProperties[i];

			QueueFamily queueFamily{};
			queueFamily.Index = i;
			queueFamily.Count = familyProperties.queueCount;
			queueFamily.Graphics = (VK_QUEUE_GRAPHICS_BIT & familyProperties.queueFlags) > 0;
			queueFamily.Transfer = (VK_QUEUE_TRANSFER_BIT & familyProperties.queueFlags) > 0;
			queueFamily.Compute = (VK_QUEUE_COMPUTE_BIT & familyProperties.queueFlags) > 0;
			queueFamily.Sparse = (VK_QUEUE_SPARSE_BINDING_BIT & familyProperties.queueFlags) > 0;

			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice->Handle, _surface, &physicalDevice->SurfaceCapabilities);

			VkBool32 presentSupported{};
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice->Handle, i, _surface, &presentSupported);
			queueFamily.Present = presentSupported == VK_TRUE;

			physicalDevice->QueueFamilies.push_back(queueFamily);
		}

		uint32_t surfaceFormatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice->Handle, _surface, &surfaceFormatCount, nullptr);
		physicalDevice->SurfaceFormats.resize(surfaceFormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice->Handle, _surface, &surfaceFormatCount, physicalDevice->SurfaceFormats.data());
	
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice->Handle, _surface, &presentModeCount, nullptr);
		physicalDevice->PresentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice->Handle, _surface, &presentModeCount, physicalDevice->PresentModes.data());

		vkGetPhysicalDeviceMemoryProperties(physicalDevice->Handle, &physicalDevice->MemoryProperties);
	}

	// pick physical device
	_physicalDevice = &_physicalDevices[0];
	ASSERT(_physicalDevice->QueueFamilies.size() > 0, "Queue Families");

	// TODO: check if the required features, layers and extensions are supported

	// select surface format
	ASSERT(_physicalDevice->SurfaceFormats.size() > 0, "");
	bool formatSelected = false;
	for (auto& surfaceFormat : _physicalDevice->SurfaceFormats)
	{
		if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			_surfaceFormat = surfaceFormat;
			formatSelected = true;
			LOG("Prefered format selected", "");
			break;
		}
	}
	if (!formatSelected)
	{
		_surfaceFormat = _physicalDevice->SurfaceFormats[0];
	}

	// select present mode
	ASSERT(_physicalDevice->PresentModes.size() > 0, "");
	bool presentModeSelected = false;
	for (auto& presentMode : _physicalDevice->PresentModes)
	{
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			_presentMode = presentMode;
			presentModeSelected = true;
			LOG("Prefered present mode selected", "");
			break;
		}
	}
	if (!presentModeSelected)
	{
		_presentMode = VK_PRESENT_MODE_FIFO_KHR;
	}

	// select queues
	QueueFamily* graphicsQueueFamily = nullptr;
	for (auto& queueFamily : _physicalDevice->QueueFamilies)
	{
		if (queueFamily.Graphics)
		{
			graphicsQueueFamily = &queueFamily;
			break;
		}
	}
	ASSERT(graphicsQueueFamily != nullptr, "Select Graphics Queue Family");

	QueueFamily* presentQueueFamily = nullptr;
	for (auto& queueFamily : _physicalDevice->QueueFamilies)
	{
		if (queueFamily.Present)
		{
			presentQueueFamily = &queueFamily;
			break;
		}
	}
	ASSERT(presentQueueFamily != nullptr, "Select Present Queue Family");
	
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

	if (graphicsQueueFamily->Index == presentQueueFamily->Index)
	{
		const float priorities[] = { 1.0f, 1.0f };
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = graphicsQueueFamily->Index;
		queueCreateInfo.queueCount = 2;
		queueCreateInfo.pQueuePriorities = priorities;
		
		queueCreateInfos.push_back(queueCreateInfo);
	}
	else
	{
		const float graphicsPriority = 1.0f;
		const float presentPriority = 1.0f;

		VkDeviceQueueCreateInfo graphicsQueueCreateInfo{};
		graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		graphicsQueueCreateInfo.queueFamilyIndex = graphicsQueueFamily->Index;
		graphicsQueueCreateInfo.queueCount = 1;
		graphicsQueueCreateInfo.pQueuePriorities = &graphicsPriority;

		VkDeviceQueueCreateInfo presentQueueCreateInfo{};
		presentQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		presentQueueCreateInfo.queueFamilyIndex = presentQueueFamily->Index;
		presentQueueCreateInfo.queueCount = 1;
		presentQueueCreateInfo.pQueuePriorities = &presentPriority;

		queueCreateInfos.push_back(graphicsQueueCreateInfo);
		queueCreateInfos.push_back(presentQueueCreateInfo);
	}

	// set-up device
	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.enabledLayerCount = requiredDeviceLayerNames.size();
	createInfo.ppEnabledLayerNames = requiredDeviceLayerNames.data();
	createInfo.enabledExtensionCount = requiredDeviceExtensionNames.size();
	createInfo.ppEnabledExtensionNames = requiredDeviceExtensionNames.data();
	createInfo.pEnabledFeatures = enabledFeatures;
	createInfo.queueCreateInfoCount = queueCreateInfos.size();
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	
	// create device
	HANDLE_VKRESULT(vkCreateDevice(_physicalDevice->Handle, &createInfo, nullptr, &_device), "Create Device");

	_graphicsQueueFamilyIndex = graphicsQueueFamily->Index;
	_presentQueueFamilyIndex = presentQueueFamily->Index;
	if (graphicsQueueFamily->Index == presentQueueFamily->Index)
	{
		vkGetDeviceQueue(_device, graphicsQueueFamily->Index, 0, &_graphicsQueue);
		vkGetDeviceQueue(_device, presentQueueFamily->Index, 1, &_presentQueue);
	}
	else
	{
		vkGetDeviceQueue(_device, graphicsQueueFamily->Index, 0, &_graphicsQueue);
		vkGetDeviceQueue(_device, presentQueueFamily->Index, 0, &_presentQueue);
	}
	ASSERT(_graphicsQueue != nullptr, "Get Graphics Queue");
	ASSERT(_presentQueue != nullptr, "Get Present Queue");
}

void Vulkan::DestroyDevice()
{
	vkDestroyDevice(_device, nullptr);
	LOG("Create Device", "Destroyed");
}

void Vulkan::CreateSwapchain()
{
	// calculate image count
	uint32_t imageCount = _physicalDevice->SurfaceCapabilities.minImageCount + 1;
	if (imageCount > _physicalDevice->SurfaceCapabilities.maxImageCount)
		imageCount = _physicalDevice->SurfaceCapabilities.maxImageCount;
	ASSERT(imageCount > 0, "");

	VkSwapchainCreateInfoKHR swapchainCreateInfo{};
	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.minImageCount = imageCount;
	swapchainCreateInfo.imageFormat = _surfaceFormat.format;
	swapchainCreateInfo.imageColorSpace = _surfaceFormat.colorSpace;
	swapchainCreateInfo.imageExtent = _extent;
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainCreateInfo.preTransform = _physicalDevice->SurfaceCapabilities.currentTransform;
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCreateInfo.presentMode = _presentMode;
	swapchainCreateInfo.clipped = VK_TRUE;
	swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
	swapchainCreateInfo.surface = _surface;

	uint32_t queueFamilyIndices[] = { _graphicsQueueFamilyIndex, _presentQueueFamilyIndex };

	if (_graphicsQueueFamilyIndex != _presentQueueFamilyIndex)
	{
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapchainCreateInfo.queueFamilyIndexCount = 2;
		swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	HANDLE_VKRESULT(vkCreateSwapchainKHR(_device, &swapchainCreateInfo, nullptr, &_swapchain), "Create Swapchain");

	// get images
	uint32_t swapchainImageCount;
	vkGetSwapchainImagesKHR(_device, _swapchain, &swapchainImageCount, nullptr);
	_swapchainImages.resize(swapchainImageCount);
	vkGetSwapchainImagesKHR(_device, _swapchain, &swapchainImageCount, _swapchainImages.data());
	ASSERT(_swapchainImages.size() > 0, "");

	// create image views
	_swapchainImageViews.resize(_swapchainImages.size());

	for (int i = 0; i < _swapchainImages.size(); i++)
	{
		VkImageViewCreateInfo imageViewCreateInfo{};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image = _swapchainImages[i];
		imageViewCreateInfo.format = _surfaceFormat.format;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;

		HANDLE_VKRESULT(vkCreateImageView(_device, &imageViewCreateInfo, nullptr, &_swapchainImageViews[i]), "Create Image View");
	}
}

void Vulkan::DestroySwapchain()
{
	for (auto& imageView : _swapchainImageViews)
	{
		vkDestroyImageView(_device, imageView, nullptr);
	}

	vkDestroySwapchainKHR(_device, _swapchain, nullptr);
}

void Vulkan::CreateRenderPass()
{
	// create attachment references
	VkAttachmentReference colorAttachmentReference{};
	colorAttachmentReference.attachment = 0;
	colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentReference{};
	depthAttachmentReference.attachment = 1;
	depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;

	// create subpasses
	VkSubpassDescription subpassDescription{};
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.colorAttachmentCount = 1;
	subpassDescription.pColorAttachments = &colorAttachmentReference;
	subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;

	// create? attachments
	VkAttachmentDescription colorAttachmentDescription{};
	colorAttachmentDescription.format = _surfaceFormat.format;
	colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;

	VkAttachmentDescription depthAttachmentDescription{};
	depthAttachmentDescription.format = VK_FORMAT_D32_SFLOAT;
	depthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
	depthAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	VkAttachmentDescription attachments[] = { colorAttachmentDescription, depthAttachmentDescription };

	// subpass dependency
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	createInfo.attachmentCount = 2;
	createInfo.pAttachments = attachments;
	createInfo.subpassCount = 1;
	createInfo.pSubpasses = &subpassDescription;
	createInfo.dependencyCount = 1;
	createInfo.pDependencies = &dependency;

	HANDLE_VKRESULT(vkCreateRenderPass(_device, &createInfo, nullptr, &_renderPass), "Create Render Pass");
}

void Vulkan::DestroyRenderPass()
{	vkDestroyRenderPass(_device, _renderPass, nullptr);
	LOG("Create Render Pass", "Destroyed");
}

void Vulkan::CreateShaderModule(std::vector<char> shaderCode, VkShaderModule* shaderModule)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = shaderCode.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

	HANDLE_VKRESULT(vkCreateShaderModule(_device, &createInfo, nullptr, shaderModule), "Create Shader Module");
}

void Vulkan::CreateShaderModule(const char* shaderCode, size_t codeSize, VkShaderModule* shaderModule)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = codeSize;
	createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode);		// TODO: Why not make the parameter uint32_t* ?

	HANDLE_VKRESULT(vkCreateShaderModule(_device, &createInfo, nullptr, shaderModule), "Create Shader Module");
}

void Vulkan::DestroyShaderModule(VkShaderModule shaderModule)
{
	vkDestroyShaderModule(_device, shaderModule, nullptr);
}

void Vulkan::CreateDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding> bindings, VkDescriptorSetLayout* descriptorSetLayout)
{
	VkDescriptorSetLayoutCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	createInfo.bindingCount = bindings.size();
	createInfo.pBindings = bindings.data();

	HANDLE_VKRESULT(vkCreateDescriptorSetLayout(_device, &createInfo, nullptr, descriptorSetLayout), "");
}

void Vulkan::DestroyDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout)
{
	vkDestroyDescriptorSetLayout(_device, descriptorSetLayout, nullptr);
}

void Vulkan::CreatePipeline(const PipelineInfo* pipelineInfo, VkPipelineLayout* pipelineLayout, VkPipeline* pipeline)
{
	/* === PIPELINE SHADERS AND STAGES === */
	
	ASSERT(pipelineInfo->VertexShaderCode != nullptr);
	ASSERT(pipelineInfo->VertexShaderCodeSize > 0);
	ASSERT(pipelineInfo->FragmentShaderCode != nullptr);
	ASSERT(pipelineInfo->FragmentShaderCodeSize> 0);

	VkShaderModule vertexShaderModule;
	VkShaderModule fragmentShaderModule;

	CreateShaderModule(pipelineInfo->VertexShaderCode, pipelineInfo->VertexShaderCodeSize, &vertexShaderModule);
	CreateShaderModule(pipelineInfo->FragmentShaderCode, pipelineInfo->FragmentShaderCodeSize, &fragmentShaderModule);

	VkPipelineShaderStageCreateInfo vertexStageCreateInfo{};
	vertexStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertexStageCreateInfo.module = vertexShaderModule;
	vertexStageCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragmentStageCreateInfo{};
	fragmentStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragmentStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragmentStageCreateInfo.module = fragmentShaderModule;
	fragmentStageCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo pipelineStages[] = { vertexStageCreateInfo, fragmentStageCreateInfo };

	/* === VERTEX INPUT === */

	ASSERT(pipelineInfo->VertexStride > 0);
	ASSERT(pipelineInfo->VertexAttributes.size() > 0);

	VkVertexInputBindingDescription vertexInputBindingDesc{};
	vertexInputBindingDesc.binding = 0;
	vertexInputBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	vertexInputBindingDesc.stride = pipelineInfo->VertexStride;

	auto vertexAttributeCount = pipelineInfo->VertexAttributes.size();
	std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions(vertexAttributeCount);

	for (auto i = 0; i < vertexAttributeCount; i++)
	{
		vertexAttributeDescriptions[i].binding = 0;
		vertexAttributeDescriptions[i].format = pipelineInfo->VertexAttributes[i].Format;
		vertexAttributeDescriptions[i].location = pipelineInfo->VertexAttributes[i].Location;
		vertexAttributeDescriptions[i].offset = pipelineInfo->VertexAttributes[i].Offset;
	}

	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
	vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
	vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBindingDesc;
	vertexInputStateCreateInfo.vertexAttributeDescriptionCount = vertexAttributeDescriptions.size();
	vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();

	/* === INPUT ASSEMBLY === */

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
	inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyStateCreateInfo.topology = pipelineInfo->Topology;
	inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

	/* === DEPTH TESTING === */

	VkPipelineDepthStencilStateCreateInfo depthStateCreateInfo{};
	depthStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStateCreateInfo.depthTestEnable = pipelineInfo->DepthTestEnabled ? VK_TRUE : VK_FALSE;
	depthStateCreateInfo.depthWriteEnable = pipelineInfo->DepthTestEnabled ? VK_TRUE : VK_FALSE;
	depthStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
	depthStateCreateInfo.stencilTestEnable = VK_FALSE;

	/* === VIEWPORT AND SCISSORS === */
	
	VkViewport viewport{};
	viewport.width = pipelineInfo->ViewportWidth;
	viewport.height = pipelineInfo->ViewportHeight;
	viewport.x = 0;
	viewport.y = 0;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.extent = _extent;
	scissor.offset = { 0, 0 };

	VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
	viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateCreateInfo.viewportCount = 1;
	viewportStateCreateInfo.pViewports = &viewport;
	viewportStateCreateInfo.scissorCount = 1;
	viewportStateCreateInfo.pScissors = &scissor;

	/* === RASTERIZATION === */

	VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{};
	rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizationStateCreateInfo.lineWidth = 1.0f;

	/* === MULTISAMPLING === */

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	/* === COLOR BLENDING === */

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	/* === PIPELINE LAYOUT === */

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = pipelineInfo->DescriptorSetLayouts.size();
	pipelineLayoutCreateInfo.pSetLayouts = pipelineInfo->DescriptorSetLayouts.data();
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0;

	HANDLE_VKRESULT(vkCreatePipelineLayout(_device, &pipelineLayoutCreateInfo, nullptr, pipelineLayout), "");

	/* === CREATE PIPELINE === */

	VkGraphicsPipelineCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	createInfo.stageCount = 2;
	createInfo.pStages = pipelineStages;
	createInfo.pVertexInputState = &vertexInputStateCreateInfo;
	createInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
	createInfo.pTessellationState = nullptr;
	createInfo.pViewportState = &viewportStateCreateInfo;
	createInfo.pRasterizationState = &rasterizationStateCreateInfo;
	createInfo.pMultisampleState = &multisampling;
	createInfo.pDepthStencilState = &depthStateCreateInfo;
	createInfo.pColorBlendState = &colorBlending;
	createInfo.pDynamicState = nullptr;
	createInfo.layout = *pipelineLayout;
	createInfo.renderPass = pipelineInfo->RenderPass;
	createInfo.subpass = 0;

	HANDLE_VKRESULT(vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &createInfo, nullptr, pipeline), "");

	/* === CLEAN UP === */

	DestroyShaderModule(vertexShaderModule);
	DestroyShaderModule(fragmentShaderModule);
}

void Vulkan::DestroyPipeline(VkPipelineLayout pipelineLayout, VkPipeline pipeline)
{
	vkDestroyPipeline(_device, pipeline, nullptr);
	vkDestroyPipelineLayout(_device, pipelineLayout, nullptr);
}

void Vulkan::CreatePipelineFixed()
{
	// create shaders
	std::vector<char> vertexShaderCode = ReadFile("shaders/vertex.spv");
	ASSERT(vertexShaderCode.size() > 0, "Vertex Shader Loaded");

	std::vector<char> fragmentShaderCode = ReadFile("shaders/fragment.spv");
	ASSERT(fragmentShaderCode.size() > 0, "Fragment Shader Loaded");

	VkShaderModule vertexShaderModule;
	VkShaderModule fragmentShaderModule;
	CreateShaderModule(vertexShaderCode, &vertexShaderModule);
	CreateShaderModule(fragmentShaderCode, &fragmentShaderModule);

	// setup shader stages
	VkPipelineShaderStageCreateInfo vertexStageCreateInfo{};
	vertexStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertexStageCreateInfo.module = vertexShaderModule;
	vertexStageCreateInfo.pName = "main";
	
	VkPipelineShaderStageCreateInfo fragmentStageCreateInfo{};
	fragmentStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragmentStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragmentStageCreateInfo.module = fragmentShaderModule;
	fragmentStageCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo stages[] = { vertexStageCreateInfo, fragmentStageCreateInfo };

	// vertex binding description
	VkVertexInputBindingDescription bindingDesc{};
	bindingDesc.binding = 0;
	bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	bindingDesc.stride = sizeof(Vertex);

	// vertex attribute description
	VkVertexInputAttributeDescription positionAttrDesc{};
	positionAttrDesc.binding = 0;
	positionAttrDesc.format = VK_FORMAT_R32G32B32_SFLOAT;
	positionAttrDesc.location = 0;
	positionAttrDesc.offset = 0;

	VkVertexInputAttributeDescription colorAttrDesc{};
	colorAttrDesc.binding = 0;
	colorAttrDesc.format = VK_FORMAT_R32G32B32_SFLOAT;
	colorAttrDesc.location = 1;
	colorAttrDesc.offset = offsetof(Vertex, Color);

	VkVertexInputAttributeDescription uvAttrDesc{};
	uvAttrDesc.binding = 0;
	uvAttrDesc.format = VK_FORMAT_R32G32_SFLOAT;
	uvAttrDesc.location = 2;
	uvAttrDesc.offset = offsetof(Vertex, Uv);

	VkVertexInputAttributeDescription attrDescs[] = { positionAttrDesc, colorAttrDesc, uvAttrDesc };

	// vertex input state
	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
	vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
	vertexInputStateCreateInfo.pVertexBindingDescriptions = &bindingDesc;
	vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 3;
	vertexInputStateCreateInfo.pVertexAttributeDescriptions = attrDescs;

	// input assembly state
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{};
	inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

	// depth state
	VkPipelineDepthStencilStateCreateInfo depthStateCreateInfo{};
	depthStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStateCreateInfo.depthTestEnable = VK_TRUE;
	depthStateCreateInfo.depthWriteEnable = VK_TRUE;
	depthStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
	depthStateCreateInfo.stencilTestEnable = VK_FALSE;

	// viewport state
	VkViewport viewport{};
	viewport.width = (float) _extent.width;
	viewport.height = (float) _extent.height;
	viewport.x = 0;
	viewport.y = 0;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.extent = _extent;
	scissor.offset = { 0, 0 };

	VkPipelineViewportStateCreateInfo viewportStateCreateInfo{};
	viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateCreateInfo.viewportCount = 1;
	viewportStateCreateInfo.pViewports = &viewport;
	viewportStateCreateInfo.scissorCount = 1;
	viewportStateCreateInfo.pScissors = &scissor;

	// rasterization state
	VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{};
	rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizationStateCreateInfo.lineWidth = 1.0f;

	// multisampling state
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	// color blending
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	// ViewProj descriptor set layout
	VkDescriptorSetLayoutBinding viewProjBinding{};
	viewProjBinding.binding = 0;
	viewProjBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	viewProjBinding.descriptorCount = 1;
	viewProjBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutCreateInfo viewProjInfo{};
	viewProjInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	viewProjInfo.bindingCount = 1;
	viewProjInfo.pBindings = &viewProjBinding;

	HANDLE_VKRESULT(vkCreateDescriptorSetLayout(_device, &viewProjInfo, nullptr, &_viewProjLayout), "Create ViewProj Layout");

	// sampler descriptor set layout
	VkDescriptorSetLayoutBinding samplerBinding{};
	samplerBinding.binding = 0;
	samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerBinding.descriptorCount = 1;
	samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	samplerInfo.bindingCount = 1;
	samplerInfo.pBindings = &samplerBinding;

	HANDLE_VKRESULT(vkCreateDescriptorSetLayout(_device, &samplerInfo, nullptr, &_samplerLayout), "Create Model Layout");

	// Model descriptor set layout
	VkDescriptorSetLayoutBinding modelBinding{};
	modelBinding.binding = 0;
	modelBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	modelBinding.descriptorCount = 1;
	modelBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutCreateInfo modelInfo{};
	modelInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	modelInfo.bindingCount = 1;
	modelInfo.pBindings = &modelBinding;

	HANDLE_VKRESULT(vkCreateDescriptorSetLayout(_device, &modelInfo, nullptr, &_modelLayout), "Create Model Layout");

	// layout
	VkDescriptorSetLayout layouts[] = { _viewProjLayout, _samplerLayout, _modelLayout };
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 3;
	pipelineLayoutCreateInfo.pSetLayouts = layouts;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	
	HANDLE_VKRESULT(vkCreatePipelineLayout(_device, &pipelineLayoutCreateInfo, nullptr, &_graphicsPipelineLayout), "Create Pipeline Layout");

	// create pipeline
	VkGraphicsPipelineCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	createInfo.stageCount = 2;
	createInfo.pStages = stages;
	createInfo.pVertexInputState = &vertexInputStateCreateInfo;
	createInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
	createInfo.pTessellationState = nullptr;
	createInfo.pViewportState = &viewportStateCreateInfo;
	createInfo.pRasterizationState = &rasterizationStateCreateInfo;
	createInfo.pMultisampleState = &multisampling;
	createInfo.pDepthStencilState = &depthStateCreateInfo;
	createInfo.pColorBlendState = &colorBlending;
	createInfo.pDynamicState = nullptr;
	createInfo.layout = _graphicsPipelineLayout;
	createInfo.renderPass = _renderPass;
	createInfo.subpass = 0;

	HANDLE_VKRESULT(vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &_graphicsPipeline), "Create Graphics Pipeline");

	// destroy shaders
	DestroyShaderModule(vertexShaderModule);
	DestroyShaderModule(fragmentShaderModule);
}

void Vulkan::DestroyPipelineFixed() 
{
	vkDestroyPipeline(_device, _graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(_device, _graphicsPipelineLayout, nullptr);

	vkDestroyDescriptorSetLayout(_device, _viewProjLayout, nullptr);
	//vkDestroyDescriptorSetLayout(_device, _modelLayout, nullptr);
	vkDestroyDescriptorSetLayout(_device, _samplerLayout, nullptr);
}

void Vulkan::CreateFramebuffers()
{
	_swapchainFramebuffers.resize(_swapchainImageViews.size());

	for (int i = 0; i < _swapchainImageViews.size(); i++)
	{
		VkImageView attachments[] = { _swapchainImageViews[i], _depthImageView };

		VkFramebufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.width = _extent.width;
		createInfo.height = _extent.height;
		createInfo.layers = 1;
		createInfo.renderPass = _renderPass;
		createInfo.attachmentCount = 2;
		createInfo.pAttachments = attachments;
		
		HANDLE_VKRESULT(vkCreateFramebuffer(_device, &createInfo, nullptr, &_swapchainFramebuffers[i]), "Framebuffer Created");
	}
}

void Vulkan::DestroyFramebuffers()
{
	for (auto& framebuffer : _swapchainFramebuffers)
	{
		vkDestroyFramebuffer(_device, framebuffer, nullptr);
	}
}

void Vulkan::CreateCommandPools()
{
	_commandPools.resize(_swapchainImageViews.size());

	for (int i = 0; i < _swapchainImageViews.size(); i++)
	{
		VkCommandPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.queueFamilyIndex = _graphicsQueueFamilyIndex;

		HANDLE_VKRESULT(vkCreateCommandPool(_device, &createInfo, nullptr, &_commandPools[i]), "Create Command Pool");
	}
}

void Vulkan::DestroyCommandPools()
{
	for (int i = 0; i < _commandPools.size(); i++)
	{
		vkDestroyCommandPool(_device, _commandPools[i], nullptr);
	}
}

void Vulkan::AllocateCommandBuffers()
{
	_commandBuffers.resize(_swapchainImageViews.size());

	// allocate command buffers
	for (int i = 0; i < _commandBuffers.size(); i++)
	{
		VkCommandBufferAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandPool = _commandPools[i];
		allocateInfo.commandBufferCount = 1;

		HANDLE_VKRESULT(vkAllocateCommandBuffers(_device, &allocateInfo, &_commandBuffers[i]), "Allocate Command Buffer");
	}
}

void Vulkan::FreeCommandBuffers()
{
	for (int i = 0; i < _commandBuffers.size(); i++)
	{
		vkFreeCommandBuffers(_device, _commandPools[i], 1, &_commandBuffers[i]);
	}
}

void Vulkan::CreateFrameSyncObjects()
{
	// create semaphores
	_imageAvailableSemaphores.resize(_framesInFlight);
	_renderFinishedSemaphores.resize(_framesInFlight);
	for (int i = 0; i < _framesInFlight; i++)
	{
		VkSemaphoreCreateInfo semaphroeCreateInfo{};
		semaphroeCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		HANDLE_VKRESULT(vkCreateSemaphore(_device, &semaphroeCreateInfo, nullptr, &_imageAvailableSemaphores[i]), "Create Semaphore");
		HANDLE_VKRESULT(vkCreateSemaphore(_device, &semaphroeCreateInfo, nullptr, &_renderFinishedSemaphores[i]), "Create Semaphore");
	}

	// create fences
	_fences.resize(_framesInFlight);
	for (int i = 0; i < _framesInFlight; i++)
	{
		VkFenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		vkCreateFence(_device, &fenceCreateInfo, nullptr, &_fences[i]);
	}

	_imageFences.resize(_swapchainImageViews.size());
	for (int i = 0; i < _imageFences.size(); i++)
	{
		_imageFences[i] = VK_NULL_HANDLE;
	}
}

void Vulkan::DestroyFrameSyncObjects()
{
	for (int i = 0; i < _framesInFlight; i++)
	{
		vkDestroySemaphore(_device, _imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(_device, _renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(_device, _fences[i], nullptr);
	}
}

uint32_t Vulkan::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	for (uint32_t i = 0; i < _physicalDevice->MemoryProperties.memoryTypeCount; i++)
	{
		if (((typeFilter & (1 << i)) != 0)
			&& ((_physicalDevice->MemoryProperties.memoryTypes[i].propertyFlags & properties) == properties))
		{
			return i;
		}
	}
	
	return UINT32_MAX;
}

void Vulkan::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& deviceMemory)
{
	VkBufferCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.usage = usage;
	createInfo.size = size;

	HANDLE_VKRESULT(vkCreateBuffer(_device, &createInfo, nullptr, &buffer), "Create Buffer");

	VkMemoryRequirements memreq;
	vkGetBufferMemoryRequirements(_device, buffer, &memreq);

	VkMemoryAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize = memreq.size;
	allocateInfo.memoryTypeIndex = FindMemoryType(memreq.memoryTypeBits, properties);
	ASSERT(allocateInfo.memoryTypeIndex < UINT32_MAX);

	HANDLE_VKRESULT(vkAllocateMemory(_device, &allocateInfo, nullptr, &deviceMemory), "Allocate Buffer Memory");
	HANDLE_VKRESULT(vkBindBufferMemory(_device, buffer, deviceMemory, 0), "Bind Buffer Memory");
}

void Vulkan::DestroyBuffer(VkBuffer buffer, VkDeviceMemory deviceMemory)
{
	vkDestroyBuffer(_device, buffer, nullptr);
	vkFreeMemory(_device, deviceMemory, nullptr);
}

void Vulkan::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBuffer commandBuffer = BeginSingleUseCommandBuffer();
	
	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
	
	EndSingleUseCommandBuffer(commandBuffer);
}

void Vulkan::CreateVertexBuffer(size_t vertexSize, uint32_t vertexCount, void* data, Buffer* buffer)
{
	ASSERT(vertexSize > 0);
	ASSERT(vertexCount > 0);
	ASSERT(data != nullptr);

	size_t bufferSize = vertexSize * vertexCount;

	// create staging buffer
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CreateBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingBufferMemory
	);

	// fill staging buffer
	void* bufferData;
	vkMapMemory(_device, stagingBufferMemory, 0, bufferSize, 0, &bufferData);
	memcpy(bufferData, data, bufferSize);
	vkUnmapMemory(_device, stagingBufferMemory);

	// create device-local buffer
	CreateBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		buffer->Buffer,
		buffer->Memory
	);

	// copy staging to device-local
	CopyBuffer(stagingBuffer, buffer->Buffer, bufferSize);

	// destroy staging buffer
	DestroyBuffer(stagingBuffer, stagingBufferMemory);
}

void Vulkan::CreateIndexBuffer(size_t indexSize, uint32_t indexCount, void* data, Buffer* buffer)
{
	ASSERT(indexSize > 0);
	ASSERT(indexCount > 0);
	ASSERT(data != nullptr);

	size_t bufferSize = indexSize * indexCount;

	// create staging buffer
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CreateBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingBufferMemory
	);

	// fill staging buffer
	void* bufferData;
	vkMapMemory(_device, stagingBufferMemory, 0, bufferSize, 0, &bufferData);
	memcpy(bufferData, data, bufferSize);
	vkUnmapMemory(_device, stagingBufferMemory);

	// create device-local buffer
	CreateBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		buffer->Buffer,
		buffer->Memory
	);

	// copy staging to device-local
	CopyBuffer(stagingBuffer, buffer->Buffer, bufferSize);

	// destroy staging buffer
	DestroyBuffer(stagingBuffer, stagingBufferMemory);
}

void Vulkan::CreateUniformBuffer()
{
	ViewProj mvp;

	_uniformBuffers.resize(_swapchainImageViews.size());
	_uniformBufferMemories.resize(_swapchainImageViews.size());

	for (int i = 0; i < _uniformBuffers.size(); i++)
	{
		CreateBuffer(sizeof(mvp), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _uniformBuffers[i], _uniformBufferMemories[i]);
	}
}

void Vulkan::DestroyUniformBuffer()
{
	for (int i = 0; i < _uniformBuffers.size(); i++)
	{
		DestroyBuffer(_uniformBuffers[i], _uniformBufferMemories[i]);
	}
}

void Vulkan::CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& memory)
{
	VkImageCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createInfo.imageType = VK_IMAGE_TYPE_2D;
	createInfo.extent.width = width;
	createInfo.extent.height = height;
	createInfo.extent.depth = 1;
	createInfo.arrayLayers = 1;
	createInfo.mipLevels = 1;
	createInfo.format = format;
	createInfo.tiling = tiling;
	createInfo.usage = usage;
	createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	HANDLE_VKRESULT(vkCreateImage(_device, &createInfo, nullptr, &image), "Create Image");

	VkMemoryRequirements memreq;
	vkGetImageMemoryRequirements(_device, image, &memreq);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memreq.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memreq.memoryTypeBits, properties);

	HANDLE_VKRESULT(vkAllocateMemory(_device, &allocInfo, nullptr, &memory), "Allocate Image Memory");

	vkBindImageMemory(_device, image, memory, 0);
}

void Vulkan::DestroyImage(VkImage image, VkDeviceMemory memory)
{
	vkDestroyImage(_device, image, nullptr);
	vkFreeMemory(_device, memory, nullptr);
}

void Vulkan::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkCommandBuffer commandBuffer = BeginSingleUseCommandBuffer();
	
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	
	VkPipelineStageFlags srcStage = 0;
	VkPipelineStageFlags dstStage = 0;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if(oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}

	ASSERT(srcStage != 0);
	ASSERT(dstStage != 0);

	vkCmdPipelineBarrier(
		commandBuffer,
		srcStage, dstStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	EndSingleUseCommandBuffer(commandBuffer);
}

void Vulkan::CopyBufferToImage(VkBuffer srcBuffer, VkImage dstImage, uint32_t width, uint32_t height)
{
	VkCommandBuffer commandBuffer = BeginSingleUseCommandBuffer();

	VkBufferImageCopy info{};
	info.bufferOffset = 0;
	info.bufferRowLength = 0;
	info.bufferImageHeight = 0;

	info.imageSubresource.mipLevel = 0;
	info.imageSubresource.baseArrayLayer = 0;
	info.imageSubresource.layerCount = 1;
	info.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

	info.imageOffset = { 0, 0, 0 };
	info.imageExtent = { width, height, 1 };

	vkCmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &info);

	EndSingleUseCommandBuffer(commandBuffer);
}

void Vulkan::CreateTexture()
{
	int width, height, channels;
	stbi_uc* pixels = stbi_load("texture.jpg", &width, &height, &channels, STBI_rgb_alpha);
	ASSERT(pixels);

	VkDeviceSize size = width * height * 4;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingMemory;
	CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingMemory);

	void* data;
	vkMapMemory(_device, stagingMemory, 0, size, 0, &data);
	memcpy(data, pixels, size);
	vkUnmapMemory(_device, stagingMemory);

	stbi_image_free(pixels);

	CreateImage(
		width,
		height,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		_textureImage,
		_textureMemory
	);

	TransitionImageLayout(_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	CopyBufferToImage(stagingBuffer, _textureImage, width, height);

	TransitionImageLayout(_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	DestroyBuffer(stagingBuffer, stagingMemory);

	// create image view
	VkImageViewCreateInfo imageViewCreateInfo{};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
	imageViewCreateInfo.image = _textureImage;
	imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	
	HANDLE_VKRESULT(vkCreateImageView(_device, &imageViewCreateInfo, nullptr, &_textureImageView), "Create Image View");

	// create image sampler
	VkSamplerCreateInfo samplerCreateInfo{};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.anisotropyEnable = VK_TRUE;
	samplerCreateInfo.maxAnisotropy = _physicalDevice->Properties.limits.maxSamplerAnisotropy;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	samplerCreateInfo.compareEnable = VK_FALSE;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = 0.0f;

	HANDLE_VKRESULT(vkCreateSampler(_device, &samplerCreateInfo, nullptr, &_sampler), "Create Sampler");
}

void Vulkan::DestroyTexture()
{
	vkDestroySampler(_device, _sampler, nullptr);
	vkDestroyImageView(_device, _textureImageView, nullptr);
	DestroyImage(_textureImage, _textureMemory);
}

void Vulkan::CreateDescriptorPool()
{
	VkDescriptorPoolSize viewProjPoolSize{};
	viewProjPoolSize.descriptorCount = static_cast<uint32_t>(_swapchainImageViews.size());
	viewProjPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

	VkDescriptorPoolSize modelPoolSize{};
	viewProjPoolSize.descriptorCount = static_cast<uint32_t>(_swapchainImageViews.size());
	viewProjPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;

	VkDescriptorPoolSize samplerPoolSize{};
	samplerPoolSize.descriptorCount = static_cast<uint32_t>(_swapchainImageViews.size());
	samplerPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

	VkDescriptorPoolSize poolSizes[] = { viewProjPoolSize, modelPoolSize, samplerPoolSize };

	VkDescriptorPoolCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	createInfo.poolSizeCount = 3;
	createInfo.pPoolSizes = poolSizes;
	createInfo.maxSets = 3 * static_cast<uint32_t>(_swapchainImageViews.size());

	HANDLE_VKRESULT(vkCreateDescriptorPool(_device, &createInfo, nullptr, &_descriptorPool), "Create Descriptor Pool");

	// create ViewProj descriptor set
	std::vector<VkDescriptorSetLayout> layouts(_swapchainImageViews.size(), _viewProjLayout);
	_descriptorSets.resize(_swapchainImageViews.size());
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = _descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(_swapchainImageViews.size());
	allocInfo.pSetLayouts = layouts.data();

	HANDLE_VKRESULT(vkAllocateDescriptorSets(_device, &allocInfo, _descriptorSets.data()), "Allocate Descriptor Sets");

	for (int i = 0; i < _descriptorSets.size(); i++)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = _uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = VK_WHOLE_SIZE;

		VkWriteDescriptorSet writeUbo{};
		writeUbo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeUbo.dstSet = _descriptorSets[i];
		writeUbo.dstBinding = 0;
		writeUbo.dstArrayElement = 0;
		writeUbo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeUbo.descriptorCount = 1;
		writeUbo.pBufferInfo = &bufferInfo;

		VkWriteDescriptorSet writes[] = { writeUbo };

		vkUpdateDescriptorSets(_device, 1, writes, 0, nullptr);
	}

	// create Model descriptor sets
	/*std::vector<VkDescriptorSetLayout> modelLayouts(_swapchainImageViews.size(), _modelLayout);
	_modelSets.resize(_swapchainImageViews.size());
	VkDescriptorSetAllocateInfo modelAllocInfo{};
	modelAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	modelAllocInfo.descriptorPool = _descriptorPool;
	modelAllocInfo.descriptorSetCount = static_cast<uint32_t>(_swapchainImageViews.size());
	modelAllocInfo.pSetLayouts = modelLayouts.data();

	HANDLE_VKRESULT(vkAllocateDescriptorSets(_device, &modelAllocInfo, _modelSets.data()), "Allocate Model Sets");

	for (int i = 0; i < _modelSets.size(); i++)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = _modelBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = VK_WHOLE_SIZE;

		VkWriteDescriptorSet writeUbo{};
		writeUbo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeUbo.dstSet = _modelSets[i];
		writeUbo.dstBinding = 0;
		writeUbo.dstArrayElement = 0;
		writeUbo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		writeUbo.descriptorCount = 1;
		writeUbo.pBufferInfo = &bufferInfo;

		VkWriteDescriptorSet writes[] = { writeUbo };

		vkUpdateDescriptorSets(_device, 1, writes, 0, nullptr);
	}*/

	// create Sampler descriptor sets
	std::vector<VkDescriptorSetLayout> samplerLayouts(_swapchainImageViews.size(), _samplerLayout);
	_samplerDescriptorSets.resize(_swapchainImageViews.size());
	VkDescriptorSetAllocateInfo samplerAllocInfo{};
	samplerAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	samplerAllocInfo.descriptorPool = _descriptorPool;
	samplerAllocInfo.descriptorSetCount = static_cast<uint32_t>(_swapchainImageViews.size());
	samplerAllocInfo.pSetLayouts = samplerLayouts.data();

	HANDLE_VKRESULT(vkAllocateDescriptorSets(_device, &samplerAllocInfo, _samplerDescriptorSets.data()), "Allocate Descriptor Sets");

	for (int i = 0; i < _samplerDescriptorSets.size(); i++)
	{
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageView = _textureImageView;
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.sampler = _sampler;

		VkWriteDescriptorSet writeSampler{};
		writeSampler.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeSampler.dstSet = _samplerDescriptorSets[i];
		writeSampler.dstBinding = 0;
		writeSampler.dstArrayElement = 0;
		writeSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeSampler.descriptorCount = 1;
		writeSampler.pImageInfo = &imageInfo;

		VkWriteDescriptorSet writes[] = { writeSampler };

		vkUpdateDescriptorSets(_device, 1, writes, 0, nullptr);
	}
}

void Vulkan::DestroyDescriptorPool()
{
	vkFreeDescriptorSets(_device, _descriptorPool, _descriptorSets.size(), _descriptorSets.data());
	vkFreeDescriptorSets(_device, _descriptorPool, _samplerDescriptorSets.size(), _samplerDescriptorSets.data());
	vkDestroyDescriptorPool(_device, _descriptorPool, nullptr);
}

void Vulkan::CreateDepthImage()
{	
	CreateImage(
		_extent.width,
		_extent.height,
		VK_FORMAT_D32_SFLOAT,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		_depthImage,
		_depthMemory
	);

	VkImageViewCreateInfo imageViewCreateInfo{};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.format = VK_FORMAT_D32_SFLOAT;
	imageViewCreateInfo.image = _depthImage;
	imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

	HANDLE_VKRESULT(vkCreateImageView(_device, &imageViewCreateInfo, nullptr, &_depthImageView), "Create Image View");
}

void Vulkan::DestroyDepthImage()
{
	vkDestroyImageView(_device, _depthImageView, nullptr);
	DestroyImage(_depthImage, _depthMemory);
}

VkCommandBuffer Vulkan::BeginSingleUseCommandBuffer()
{
	VkCommandBufferAllocateInfo commandBufferAllocInfo{};
	commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocInfo.commandPool = _commandPools[0];
	commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(_device, &commandBufferAllocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void Vulkan::EndSingleUseCommandBuffer(VkCommandBuffer commandBuffer)
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	vkQueueSubmit(_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(_graphicsQueue);

	vkFreeCommandBuffers(_device, _commandPools[0], 1, &commandBuffer);
}

int Vulkan::GetSwapchainImageCount()
{
	return _swapchainImages.size();
}

void Vulkan::CreateDescriptorPool(std::vector<VkDescriptorPoolSize> poolSizes, uint32_t maxSets, VkDescriptorPool* pool)
{
	VkDescriptorPoolCreateInfo poolCreateInfo{};
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.poolSizeCount = poolSizes.size();
	poolCreateInfo.pPoolSizes = poolSizes.data();
	poolCreateInfo.maxSets = maxSets;

	HANDLE_VKRESULT(vkCreateDescriptorPool(_device, &poolCreateInfo, nullptr, pool), "");
}

void Vulkan::DestroyDescriptorPool(VkDescriptorPool pool)
{
	vkDestroyDescriptorPool(_device, pool, nullptr);
}

void Vulkan::BeginDrawFrame()
{
	if (_resizeWidth != UINT32_MAX && _resizeHeight != UINT32_MAX)
	{
		RecreateSwapchain(_resizeWidth, _resizeHeight);
		_resizeWidth = UINT32_MAX;
		_resizeHeight = UINT32_MAX;
	}

	vkWaitForFences(_device, 1, &_fences[_currentFrame], VK_TRUE, UINT64_MAX);

	VkResult acquireImageResult = vkAcquireNextImageKHR(_device, _swapchain, UINT64_MAX, _imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, &_currentImage);

	if (acquireImageResult == VK_ERROR_OUT_OF_DATE_KHR)
	{
		//return;	// TODO: fix this
	}
	else
	{
		ASSERT(acquireImageResult == VK_SUCCESS || acquireImageResult == VK_SUBOPTIMAL_KHR);
	}

	vkResetCommandPool(_device, _commandPools[_currentImage], 0);

	// begin recording the main command buffer
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	vkBeginCommandBuffer(_commandBuffers[_currentImage], &beginInfo);
	
		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		VkClearValue clearDepth = { 1.0f, 0.0f, 0.0f, 0.0f };
		VkClearValue clearValues[] = { clearColor, clearDepth };

		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = _renderPass;
		renderPassBeginInfo.framebuffer = _swapchainFramebuffers[_currentImage];
		renderPassBeginInfo.renderArea.extent = _extent;
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.clearValueCount = 2;
		renderPassBeginInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass(_commandBuffers[_currentImage], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		//vkCmdBindPipeline(_commandBuffers[_currentImage], VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);

		//// TODO: Set the descriptor sets per model
		//VkDescriptorSet sets[] = { _descriptorSets[_currentImage], };
		//vkCmdBindDescriptorSets(
		//	_commandBuffers[_currentImage],
		//	VK_PIPELINE_BIND_POINT_GRAPHICS, 
		//	_graphicsPipelineLayout, 
		//	0, 
		//	1,
		//	&_descriptorSets[_currentImage],
		//	0, 
		//	nullptr
		//);
		//vkCmdBindDescriptorSets(
		//	_commandBuffers[_currentImage],
		//	VK_PIPELINE_BIND_POINT_GRAPHICS,
		//	_graphicsPipelineLayout,
		//	1,
		//	1,
		//	&_samplerDescriptorSets[_currentImage],
		//	0,
		//	nullptr
		//);
}

VkCommandBuffer* Vulkan::GetMainCommandBuffer()
{
	return &_commandBuffers[_currentImage];
}

void Vulkan::EndDrawFrame()
{
	// end recording the main command buffer
	vkCmdEndRenderPass(_commandBuffers[_currentImage]);
	vkEndCommandBuffer(_commandBuffers[_currentImage]);

	/* TMP */
	// update uniform buffer
	zRot += 0.0001f;
	ViewProj mvp;
	mvp.View = Math::Matrices::Translate(sinf(zRot), 0, 3);
	mvp.Projection = Euler::Math::Matrices::Perspective(_extent.width, _extent.height, 60, 0.001f, 100.0f);

	mvp.View.Transpose();
	mvp.Projection.Transpose();

	void* data;
	vkMapMemory(_device, _uniformBufferMemories[_currentImage], 0, sizeof(mvp), 0, &data);
	memcpy(data, &mvp, sizeof(mvp));
	vkUnmapMemory(_device, _uniformBufferMemories[_currentImage]);
	/* END TMP*/

	if (_imageFences[_currentImage] != VK_NULL_HANDLE)
	{
		vkWaitForFences(_device, 1, &_imageFences[_currentImage], VK_TRUE, UINT64_MAX);
	}
	_imageFences[_currentImage] = _fences[_currentFrame];

	VkPipelineStageFlags waitStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &_commandBuffers[_currentImage];
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &_imageAvailableSemaphores[_currentFrame];
	submitInfo.pWaitDstStageMask = &waitStages;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &_renderFinishedSemaphores[_currentFrame];

	vkResetFences(_device, 1, &_fences[_currentFrame]);
	vkQueueSubmit(_graphicsQueue, 1, &submitInfo, _fences[_currentFrame]);

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &_swapchain;
	presentInfo.pImageIndices = &_currentImage;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &_renderFinishedSemaphores[_currentFrame];

	VkResult presentResult = vkQueuePresentKHR(_presentQueue, &presentInfo);

	if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR)
	{
		return;
	}
	else
	{
		ASSERT(presentResult == VK_SUCCESS);
	}

	//vkQueueWaitIdle(_graphicsQueue);
	_currentFrame = (_currentFrame + 1) % _framesInFlight;
}

void Vulkan::DrawMesh(VkCommandBuffer commandBuffer, Buffer* vertexBuffer, Buffer* indexBuffer, int indexCount)
{
	VkBuffer buffers[] = { vertexBuffer->Buffer };
	VkDeviceSize offsets[] = { 0 };

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer->Buffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
}