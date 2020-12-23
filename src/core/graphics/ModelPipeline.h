#pragma once

#include "../API.h"
#include "vulkan/Vulkan.h"
#include "Common.h"
#include "Vertex.h"
#include "Model.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace Euler 
{
	namespace Graphics 
	{
		class EULER_API ModelPipeline
		{
		private:
			Vulkan* _vulkan;

			VkPipeline _pipeline;
			VkPipelineLayout _pipelineLayout;

			VkDescriptorSetLayout _viewProjLayout;
			VkDescriptorSetLayout _modelLayout;
			//VkDescriptorSetLayout _textureLayout;

			VkDescriptorPool _descriptorPool;
			std::vector<VkDescriptorSet> _viewProjDescriptorSets;
			std::vector<VkDescriptorSet> _modelDescriptorSets;

			std::vector<Buffer> _viewProjBuffers;
			std::vector<Buffer> _modelBuffers;

		public:
			std::vector<Model*> Models;

			void Create(Vulkan* vulkan, float viewportWidth, float viewportHeight);
			void Destroy();

			void RecordCommands();

		private:
			std::vector<VertexAttributeInfo> GetVertexAttributes();
			void CreateDescriptorSetLayouts();
			void CreateDescriptorSets();

			void CreateViewProjDescriptorSets();
			void CreateModelDescriptorSets();
		};
	}
}