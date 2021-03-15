#include "ModelRenderer.h"

#include "../io/Utils.h"
#include "../math/Matrices.h"

using namespace Euler::Graphics;

ModelRenderer::ModelRenderer(Vulkan* vulkan, float viewportWidth, float viewportHeight)
	: Renderer(vulkan)
{
	FillDescriptorInfos();

	/* === READ SHADER CODE === */

	std::vector<char> vertexShaderCode = ReadFile("shaders/out/vertex.spv");
	std::vector<char> fragmentShaderCode = ReadFile("shaders/out/fragment.spv");

	/* === CREATE PIPELINE === */

	RendererInfo rendererInfo{};

	rendererInfo.VertexShaderCode = vertexShaderCode.data();
	rendererInfo.VertexShaderCodeSize = vertexShaderCode.size();

	rendererInfo.FragmentShaderCode = fragmentShaderCode.data();
	rendererInfo.FragmentShaderCodeSize = fragmentShaderCode.size();

	rendererInfo.VertexStride = sizeof(Vertex);
	rendererInfo.VertexAttributes = GetVertexAttributes();

	rendererInfo.Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	rendererInfo.DepthTestEnabled = true;

	rendererInfo.RenderPass = VulkanRef->_renderPass;	// TODO: This should be a parameter

	rendererInfo.ViewportWidth = viewportWidth;
	rendererInfo.ViewportHeight = viewportHeight;

	Create(&rendererInfo);
}

std::vector<VertexAttributeInfo> ModelRenderer::GetVertexAttributes()
{
	std::vector<VertexAttributeInfo> vec(5);

	// position
	vec[0].Location = 0;
	vec[0].Offset = 0;
	vec[0].Format = VK_FORMAT_R32G32B32_SFLOAT;

	// normal
	vec[1].Location = 1;
	vec[1].Offset = offsetof(Vertex, Normal);
	vec[1].Format = VK_FORMAT_R32G32B32_SFLOAT;

	// tangent
	vec[2].Location = 2;
	vec[2].Offset = offsetof(Vertex, Tangent);
	vec[2].Format = VK_FORMAT_R32G32B32_SFLOAT;

	// bitangent
	vec[3].Location = 3;
	vec[3].Offset = offsetof(Vertex, Bitangent);
	vec[3].Format = VK_FORMAT_R32G32B32_SFLOAT;

	// uv
	vec[4].Location = 4;
	vec[4].Offset = offsetof(Vertex, UV);
	vec[4].Format = VK_FORMAT_R32G32_SFLOAT;

	return vec;
}

void ModelRenderer::FillDescriptorInfos()
{
	/* === ViewProj === */
	DescriptorInfo viewProjInfo{};
	viewProjInfo.Bindings.resize(1);

	viewProjInfo.Bindings[0].binding = 0;
	viewProjInfo.Bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	viewProjInfo.Bindings[0].descriptorCount = 1;
	viewProjInfo.Bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	DescriptorInfos.push_back(viewProjInfo);

	/* === Model === */
	DescriptorInfo modelInfo{};
	modelInfo.Bindings.resize(1);

	modelInfo.Bindings[0].binding = 0;
	modelInfo.Bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	modelInfo.Bindings[0].descriptorCount = 1;
	modelInfo.Bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	DescriptorInfos.push_back(modelInfo);

	/* === ColorTexture === */
	DescriptorInfo colorTextureInfo{};
	colorTextureInfo.Bindings.resize(1);

	colorTextureInfo.Bindings[0].binding = 0;
	colorTextureInfo.Bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	colorTextureInfo.Bindings[0].descriptorCount = 1;
	colorTextureInfo.Bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	DescriptorInfos.push_back(colorTextureInfo);

	/* === NormalMap === */
	DescriptorInfo normalMapInfo{};
	normalMapInfo.Bindings.resize(1);

	normalMapInfo.Bindings[0].binding = 0;
	normalMapInfo.Bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	normalMapInfo.Bindings[0].descriptorCount = 1;
	normalMapInfo.Bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	DescriptorInfos.push_back(normalMapInfo);

	/* === MaterialProperties === */
	DescriptorInfo materialPropertiesInfo{};
	materialPropertiesInfo.Bindings.resize(1);

	materialPropertiesInfo.Bindings[0].binding = 0;
	materialPropertiesInfo.Bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	materialPropertiesInfo.Bindings[0].descriptorCount = 1;
	materialPropertiesInfo.Bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	DescriptorInfos.push_back(materialPropertiesInfo);

	/* === DirectionalLight === */
	DescriptorInfo directionalLightInfo{};
	directionalLightInfo.Bindings.resize(3);

	directionalLightInfo.Bindings[0].binding = 0;
	directionalLightInfo.Bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	directionalLightInfo.Bindings[0].descriptorCount = 1;
	directionalLightInfo.Bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	directionalLightInfo.Bindings[1].binding = 1;
	directionalLightInfo.Bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	directionalLightInfo.Bindings[1].descriptorCount = 1;
	directionalLightInfo.Bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	directionalLightInfo.Bindings[2].binding = 2;
	directionalLightInfo.Bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	directionalLightInfo.Bindings[2].descriptorCount = 1;
	directionalLightInfo.Bindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	DescriptorInfos.push_back(directionalLightInfo);

	/* === LightView === */
	DescriptorInfo lightViewInfo{};
	lightViewInfo.Bindings.resize(1);

	lightViewInfo.Bindings[0].binding = 0;
	lightViewInfo.Bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	lightViewInfo.Bindings[0].descriptorCount = 1;
	lightViewInfo.Bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	DescriptorInfos.push_back(lightViewInfo);
}

void ModelRenderer::CreateDescriptorSets()
{

}