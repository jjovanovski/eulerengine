#pragma once

#include "../API.h"
#include "Renderer.h"
#include "vulkan/Vulkan.h"
#include "Vertex.h"

namespace Euler
{
	namespace Graphics
	{
		class ModelRenderer : public Renderer
		{
		public:
			ModelRenderer(Vulkan* vulkan, float viewportWidth, float viewportHeight);

			std::vector<VertexAttributeInfo> GetVertexAttributes();

			void FillDescriptorInfos();
			void CreateDescriptorSets();
		};
	}
}