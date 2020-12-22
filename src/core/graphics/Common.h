#pragma once

#include "../API.h"

#include <stdint.h>
#include <vector>

class EULER_API VertexAttributeInfo
{
public:
	uint32_t Location;
	uint32_t Offset;
	VkFormat Format;
};

class EULER_API PipelineInfo
{
public:
	const char* VertexShaderCode;
	size_t VertexShaderCodeSize;

	const char* FragmentShaderCode;
	size_t FragmentShaderCodeSize;

	uint32_t VertexStride;
	std::vector<VertexAttributeInfo> VertexAttributes;

	VkPrimitiveTopology Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	bool DepthTestEnabled = true;

	float ViewportWidth;
	float ViewportHeight;

	std::vector<VkDescriptorSetLayout> DescriptorSetLayouts;

	VkRenderPass RenderPass;
};