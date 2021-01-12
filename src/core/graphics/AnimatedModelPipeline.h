#pragma once

#include "../API.h"
#include "vulkan/Vulkan.h"
#include "Common.h"
#include "Vertex.h"
#include "AnimatedModel.h"
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
		class EULER_API AnimatedModelPipeline
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

			VkDescriptorPool _descriptorPool;
			DescriptorSetGroup _viewProjDescriptorSetGroup;
			DescriptorSetGroup _modelDescriptorSetGroup;
			DescriptorSetGroup _lightDescriptorSetGroup;

			BufferGroup _viewProjBuffers;
			BufferGroup _modelBuffers;
			BufferGroup _directionalLightBuffers;
			BufferGroup _ambientLightBuffers;

			uint64_t _modelMatrixAlignment;

		public:
			std::vector<AnimatedModel*> Models;
			DirectionalLight* DirLight;
			AmbientLight AmbLight;

			VkDescriptorSetLayout ViewProjLayout;
			VkDescriptorSetLayout ModelLayout;
			VkDescriptorSetLayout MaterialLayout;
			VkDescriptorSetLayout DirectionalLightLayout;

			void Create(Vulkan* vulkan, float viewportWidth, float viewportHeight);
			void Destroy();

			void RecordCommands(ViewProj viewProjMatrix);

		private:
			std::vector<VertexAttributeInfo> GetVertexAttributes();
			void CreateDescriptorSetLayouts();
			void CreateDescriptorSets();

			void CreateViewProjDescriptorSets();
			void CreateModelDescriptorSets();
			void CreateDirectionalLightDescriptorSets();
		};
	}
}