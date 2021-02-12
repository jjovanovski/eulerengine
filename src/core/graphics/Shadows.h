#pragma once

#include "../API.h"
#include "vulkan/Vulkan.h"
#include "Common.h"
#include "Vertex.h"
#include "ModelPipeline.h"

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

		public:
			void Create(Vulkan* vulkan, ModelPipeline* modelPipeline, float viewportWidth, float viewportHeight);
			void Destroy();

			void RecordCommands();
		};
	}
}