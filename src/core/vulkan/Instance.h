#pragma once

#include "../API.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace Euler
{
    namespace Vulkan
    {
        class EULER_API Instance
        {
        private:
            VkInstance _handle;

        public:
            void Init(const std::vector<const char*>& requiredLayerNames, const std::vector<const char*>& requiredExtensionNames);
            void Cleanup();

            std::vector<VkLayerProperties> GetSupportedLayers();
            std::vector<VkExtensionProperties> GetSupportedExtensions();

            std::vector<VkPhysicalDevice> GetPhysicalDevices();

        private:
            std::vector<const char*> FilterUnsupportedLayers(const std::vector<const char*>& requiredLayerNames);
            std::vector<const char*> FilterUnsupportedExtensions(const std::vector<const char*>& requiredExtensionNames);
        };
    };
};
