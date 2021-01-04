#pragma once

#include "../API.h"
#include "vulkan/Vulkan.h"
#include "DescriptorSetGroup.h"
#include "BufferGroup.h"

#include <stdint.h>

namespace Euler
{
	namespace Graphics
	{
		class EULER_API Texture
		{
		private:
			Vulkan* _vulkan;

			VkImage _image;
			VkDeviceMemory _memory;
			VkImageView _imageView;
			VkSampler _sampler;

			BufferGroup _shininessBufferGroup;

			static bool DescriptorPoolCreated;
			static VkDescriptorPool DescriptorPool;

		public:
			//std::vector<VkDescriptorSet> DescriptorSets;
			DescriptorSetGroup DescriptorSetGroup;

			float Shininess;

			void Create(Vulkan* vulkan, void* pixels, uint32_t width, uint32_t height, size_t size, VkDescriptorSetLayout descriptorSetLayout);
			void Destroy();

		private:
			void CreateDescriptorPool();
			void DestroyDescriptorPool();
		};
	}
}