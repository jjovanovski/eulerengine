#include "Instance.h"

#include <iostream>

using namespace Euler::Vulkan;

void Instance::Init(const std::vector<const char*>& requiredLayerNames, const std::vector<const char*>& requiredExtensionNames)
{
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.engineVersion = VK_MAKE_VERSION(EULER_MAJOR, EULER_MINOR, EULER_PATCH);
	appInfo.pEngineName = EULER_ENGINE_NAME;
	appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);			// TODO: Allow the user to set the application version
	appInfo.pApplicationName = "Euler Application";					// TODO: Allow the user to set the application name
	appInfo.apiVersion = VK_VERSION_1_1;							// TODO: Allow the engine/user to set the Vulkan version

	auto unsupportedLayers = FilterUnsupportedLayers(requiredLayerNames);
	if (requiredLayerNames.size() > 0 && unsupportedLayers.size() > 0)
	{
		std::cout << "Not supported layers found:" << std::endl;
		for (auto layerName : unsupportedLayers)
		{
			std::cout << layerName << std::endl;
		}
	}

	auto unsupportedExtensions = FilterUnsupportedExtensions(requiredExtensionNames);
	if (requiredExtensionNames.size() > 0 && unsupportedExtensions.size() > 0)
	{
		std::cout << "Not supported extensions found:" << std::endl;
		for (auto extensionName : unsupportedExtensions)
		{
			std::cout << extensionName << std::endl;
		}
	}

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IOS_SURFACE_CREATE_INFO_MVK;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = (uint32_t) requiredLayerNames.size();
	createInfo.ppEnabledLayerNames = requiredLayerNames.data();
	createInfo.enabledExtensionCount = (uint32_t) requiredExtensionNames.size();
	createInfo.ppEnabledExtensionNames = requiredExtensionNames.data();

	auto result = vkCreateInstance(&createInfo, nullptr, &_handle);
	if (result == VK_SUCCESS)
	{
		std::cout << "Instance created" << std::endl;
	}
	else
	{
		std::cout << "Failed to create instance" << std::endl;
	}
}

void Instance::Cleanup()
{
	vkDestroyInstance(_handle, nullptr);
	std::cout << "Instance destroyed" << std::endl;
}

std::vector<VkLayerProperties> Instance::GetSupportedLayers()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> layers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

	return layers;
}

std::vector<VkExtensionProperties> Instance::GetSupportedExtensions()
{
	uint32_t extensionCount;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> supportedExtensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, supportedExtensions.data());

	return supportedExtensions;
}


std::vector<const char*> Instance::FilterUnsupportedLayers(const std::vector<const char*>& requiredLayerNames)
{
	auto supportedLayers = GetSupportedLayers();
	std::vector<const char*> unsupportedLayers;
	for (auto layerName : requiredLayerNames)
	{
		bool found = false;
		for (auto &layer : supportedLayers)
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

std::vector<const char*> Instance::FilterUnsupportedExtensions(const std::vector<const char*>& requiredExtensionNames)
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

std::vector<VkPhysicalDevice> Instance::GetPhysicalDevices()
{
	uint32_t physicalDevicesCount;
	vkEnumeratePhysicalDevices(_handle, &physicalDevicesCount, nullptr);
	std::vector<VkPhysicalDevice> physicalDevices(physicalDevicesCount);
	vkEnumeratePhysicalDevices(_handle, &physicalDevicesCount, physicalDevices.data());

	return physicalDevices;
}