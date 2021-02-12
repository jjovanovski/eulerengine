#pragma once

#include "../API.h"
#include "vulkan/Vulkan.h"
#include "Common.h"
#include "Vertex.h"
#include "ModelPipeline.h"

#include <vector>

namespace Euler
{
	namespace Graphics
	{
		class EULER_API Shadows
		{
		private:
			Vulkan* _vulkan;

			VkRenderPass _shadowRenderPass;

			VkPipeline _pipeline;
			VkPipelineLayout _pipelineLayout;

			ModelPipeline* _modelPipeline;

			VkDescriptorSetLayout ViewProjLayout;
			VkDescriptorSetLayout ModelLayout;

			float _viewportWidth;
			float _viewportHeight;

			std::vector<VkImage> _depthImages;
			std::vector<VkDeviceMemory> _depthImageMemories;
			std::vector<VkImageView> _depthImageViews;

			std::vector<VkFramebuffer> _framebuffers;

			VkSampler _sampler;

		public:
			void Create(Vulkan* vulkan, ModelPipeline* modelPipeline, float viewportWidth, float viewportHeight);
			void Destroy();

			void CreateFramebuffers();
			void CreateDescriptorSetLayouts();

			void UpdateDescriptorSets();

			void RecordCommands();
		};
	}
}