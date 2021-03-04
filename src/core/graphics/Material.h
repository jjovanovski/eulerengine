#pragma once

#include "../API.h"
#include "Texture.h"

namespace Euler
{
	namespace Graphics
	{
		class EULER_API MaterialProperties
		{
		public:
			float Shininess;

			float UseNormalMap = 0.0f;
			float UseSpecularMap = 0.0f;
		};

		class EULER_API Material
		{
		private:
			Vulkan* _vulkan;

		public:
			Texture* ColorMap = nullptr;
			Texture* NormalMap = nullptr;
			Texture* SpecularMap = nullptr;

			MaterialProperties Properties;

			BufferGroup MaterialPropertiesBuffers;
			DescriptorSetGroup MaterialPropertiesDescriptorSetGroup;

			static bool DescriptorPoolCreated;
			static VkDescriptorPool DescriptorPool;

			void Create(Vulkan* vulkan, VkDescriptorSetLayout layout);
			void Destroy();

			void CreateDescriptorPool();
		};
	}
}