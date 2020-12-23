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
			/// <summary>
			/// This constant will be used when calculating the initial size of the dynamic uniform buffer which contains
			/// the model matrices. The size of the buffer will be INITIAL_MODELS_FOR_BUFFER_SIZE * sizeof(model_matrix).
			/// </summary>
			const uint32_t INITIAL_MODELS_FOR_BUFFER_SIZE = 8;

			Vulkan* _vulkan;

			VkPipeline _pipeline;
			VkPipelineLayout _pipelineLayout;

			VkDescriptorSetLayout _viewProjLayout;
			VkDescriptorSetLayout _modelLayout;

			VkDescriptorPool _descriptorPool;
			std::vector<VkDescriptorSet> _viewProjDescriptorSets;
			std::vector<VkDescriptorSet> _modelDescriptorSets;

			std::vector<Buffer> _viewProjBuffers;
			std::vector<Buffer> _modelBuffers;

			uint64_t _modelMatrixAlignment;

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