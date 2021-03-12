#pragma once

#include "../../API.h"
#include "../../math/Vec2.h"
#include "../../math/Vec3.h"
#include "../../math/Mat4.h"
#include "../Common.h"
#include "../RendererInfo.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace Euler
{
    namespace Graphics
    {
        struct EULER_API QueueFamily
        {
            uint32_t Index;
            uint32_t Count;
            bool Graphics;
            bool Transfer;
            bool Compute;
            bool Sparse;
            bool Present;
        };

        struct EULER_API PhysicalDevice
        {
            VkPhysicalDevice Handle;
            VkPhysicalDeviceProperties Properties;
            std::vector<QueueFamily> QueueFamilies;
            VkSurfaceCapabilitiesKHR SurfaceCapabilities;
            std::vector<VkSurfaceFormatKHR> SurfaceFormats;
            std::vector<VkPresentModeKHR> PresentModes;
            VkPhysicalDeviceMemoryProperties MemoryProperties;
        };

        class EULER_API Buffer
        {
        public:
            VkBuffer Buffer;
            VkDeviceMemory Memory;
        };

        class EULER_API Vulkan
        {
        public:
            VkInstance _instance;

            VkDebugUtilsMessengerEXT _debugMessenger;

            PhysicalDevice* _physicalDevice;
            std::vector<PhysicalDevice> _physicalDevices;
            VkDevice _device;
            QueueFamily* _graphicsQueueFamily;

            uint32_t _graphicsQueueFamilyIndex;
            VkQueue _graphicsQueue;

            uint32_t _presentQueueFamilyIndex;
            VkQueue _presentQueue;

            VkSurfaceKHR _surface;
            VkSurfaceFormatKHR _surfaceFormat;
            VkPresentModeKHR _presentMode;
            VkSwapchainKHR _swapchain;
            VkExtent2D _extent;
            std::vector<VkImage> _swapchainImages;
            std::vector<VkImageView> _swapchainImageViews;
            std::vector<VkFramebuffer> _swapchainFramebuffers;

            VkRenderPass _renderPass;

            VkPipelineLayout _graphicsPipelineLayout;
            VkPipeline _graphicsPipeline;

            std::vector<VkCommandPool> _commandPools;
            std::vector<VkCommandBuffer> _commandBuffers;

            std::vector<VkSemaphore> _imageAvailableSemaphores;
            std::vector<VkSemaphore> _renderFinishedSemaphores;
            std::vector<VkFence> _fences;
            std::vector<VkFence> _imageFences;
            int _framesInFlight = 3;
            int _currentFrame = 0;
            uint32_t _currentImage = 0;

            uint32_t _resizeWidth = UINT32_MAX;
            uint32_t _resizeHeight = UINT32_MAX;

            VkDescriptorSetLayout _viewProjLayout;
            VkDescriptorSetLayout _modelLayout;
            VkDescriptorSetLayout _samplerLayout;
            std::vector<VkBuffer> _uniformBuffers;
            std::vector<VkDeviceMemory> _uniformBufferMemories;
            std::vector<VkBuffer> _modelBuffers;
            std::vector<VkDeviceMemory> _modelBufferMemories;
            VkDescriptorPool _descriptorPool;
            std::vector<VkDescriptorSet> _descriptorSets;
            std::vector<VkDescriptorSet> _modelSets;
            std::vector<VkDescriptorSet> _samplerDescriptorSets;

            VkImage _textureImage;
            VkDeviceMemory _textureMemory;
            VkImageView _textureImageView;
            VkSampler _sampler;

            VkImage _depthImage;
            VkDeviceMemory _depthMemory;
            VkImageView _depthImageView;
            
            // tmp:
            float zRot = 0.0f;

        public:
            Vulkan();

            void InitRenderer(uint32_t width, uint32_t height);
            void Cleanup();
            void RecreateSwapchain(uint32_t width, uint32_t height);

            void SetWindowResized(uint32_t width, uint32_t height);

            void CreateInstance(const char* appName, uint32_t appVersion, std::vector<const char*> requiredLayerNames, std::vector<const char*> requiredExtensionNames);
            void DestroyInstance();
            VkInstance GetInstance();
            std::vector<VkLayerProperties> GetSupportedLayers();
            std::vector<VkExtensionProperties> GetSupportedExtensions();
            std::vector<const char*> FilterUnsupportedLayers(const std::vector<const char*>& requiredLayerNames);
            std::vector<const char*> FilterUnsupportedExtensions(const std::vector<const char*>& requiredExtensionNames);

            void CreateDebugMessenger();
            VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
            void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
            static VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

            void CreateDevice(VkSurfaceKHR surface, VkPhysicalDeviceFeatures* enabledFeatures, std::vector<const char*> requiredDeviceLayerNames, std::vector<const char*> requiredDeviceExtensionNames);
            void DestroyDevice();

            void CreateSwapchain();
            void DestroySwapchain();

            void CreateRenderPass();
            void DestroyRenderPass();

            void CreateShadowRenderPass(VkRenderPass* renderPass);

            void CreateFramebuffers();
            void DestroyFramebuffers();

            void CreateCommandPools();
            void DestroyCommandPools();

            void AllocateCommandBuffers();
            void FreeCommandBuffers();

            void CreateFrameSyncObjects();
            void DestroyFrameSyncObjects();

            void CreateDepthImage();
            void DestroyDepthImage();

            VkCommandBuffer BeginSingleUseCommandBuffer();
            void EndSingleUseCommandBuffer(VkCommandBuffer commandBuffer);

            void BeginDrawFrame();
            VkCommandBuffer* GetMainCommandBuffer();
            void EndDrawFrame();


            // =====   ABSTRACTED   =====

            int GetSwapchainImageCount();
            PhysicalDevice* GetPhysicalDevice();

            uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
            void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& deviceMemory);
            void DestroyBuffer(VkBuffer buffer, VkDeviceMemory deviceMemory);
            void CopyBuffer(VkBuffer srcBuffer, VkBuffer destBuffer, VkDeviceSize size);

            void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& memory);
            void DestroyImage(VkImage image, VkDeviceMemory memory);
            void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
            void CopyBufferToImage(VkBuffer srcBuffer, VkImage dstImage, uint32_t width, uint32_t height);

            void CreateImageView(VkImage image, VkImageView* imageView);
            void DestroyImageView(VkImageView imageView);

            void CreateSampler(VkSampler* sampler);
            void DestroySampler(VkSampler sampler);

            void CreateShaderModule(const char* shaderCode, size_t codeSize, VkShaderModule* shaderModule);
            void DestroyShaderModule(VkShaderModule shaderModule);

            void CreateDescriptorPool(std::vector<VkDescriptorPoolSize> poolSizes, uint32_t maxSets, VkDescriptorPool* pool);
            void DestroyDescriptorPool(VkDescriptorPool pool);

            void CreateDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding> bindings, VkDescriptorSetLayout* descriptorSetLayout);
            void DestroyDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout);

            void CreatePipeline(const PipelineInfo* pipelineInfo, VkPipelineLayout* pipelineLayout, VkPipeline* pipeline);
            void CreatePipeline(const Euler::Graphics::RendererInfo* rendererInfo, VkPipelineLayout* pipelineLayout, VkPipeline* pipeline);
            void DestroyPipeline(VkPipelineLayout pipelineLayout, VkPipeline pipeline);

            void CreateVertexBuffer(size_t vertexSize, uint32_t vertexCount, void* data, Buffer* buffer);
            void CreateIndexBuffer(size_t indexSize, uint32_t indexCount, void*data, Buffer* buffer);

            void MapMemory(VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, void** data);
            void UnmapMemory(VkDeviceMemory memory);
            void CopyToMemory(VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, void* sourceData);

            void DrawMesh(VkCommandBuffer commandBuffer, Buffer* vertexBuffer, Buffer* indexBuffer, int indexCount);

            // ===== ABSTRACTED END =====
        };
    };
};
