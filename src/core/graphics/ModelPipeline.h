#pragma once

#include "../API.h"
#include "vulkan/Vulkan.h"
#include "Common.h"
#include "Vertex.h"
#include "Model.h"
#include "DirectionalLight.h"
#include "AmbientLight.h"
#include "BufferGroup.h"
#include "DescriptorSetGroup.h"

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


			BufferGroup _viewProjBuffers;
			BufferGroup _modelBuffers;
			BufferGroup _directionalLightBuffers;
			BufferGroup _ambientLightBuffers;

		public:
			std::vector<Model*> Models;
			DirectionalLight* DirLight;
			AmbientLight AmbLight;

			VkDescriptorPool _descriptorPool;

			VkDescriptorSetLayout ViewProjLayout;
			VkDescriptorSetLayout ModelLayout;
			VkDescriptorSetLayout MaterialLayout;
			VkDescriptorSetLayout DirectionalLightLayout;

			DescriptorSetGroup _viewProjDescriptorSetGroup;
			DescriptorSetGroup _modelDescriptorSetGroup;
			DescriptorSetGroup _lightDescriptorSetGroup;

			uint64_t _modelMatrixAlignment;

			void Create(Vulkan* vulkan, float viewportWidth, float viewportHeight);
			void Destroy();

			void Update(ViewProj viewProjMatrix);
			void RecordCommands(ViewProj viewProjMatrix);

			std::vector<VertexAttributeInfo> GetVertexAttributes();

		private:
			void CreateDescriptorSetLayouts();
			void CreateDescriptorSets();

			void CreateViewProjDescriptorSets();
			void CreateModelDescriptorSets();
			void CreateDirectionalLightDescriptorSets();
		};
	}
}