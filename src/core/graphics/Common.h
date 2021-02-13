#pragma once

#include "../API.h"
#include "../math/Mat4.h"

#include <vulkan/vulkan.h>
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

	VkPrimitiveTopology Topology;

	bool DepthTestEnabled = true;

	float ViewportWidth;
	float ViewportHeight;

	std::vector<VkDescriptorSetLayout> DescriptorSetLayouts;

	VkRenderPass RenderPass;

	VkCullModeFlags CullMode = VK_CULL_MODE_BACK_BIT;
	VkFrontFace FrontFace = VK_FRONT_FACE_CLOCKWISE;
};

class EULER_API ViewProj
{
public:
	Mat4 View;
	Mat4 Projection;
};