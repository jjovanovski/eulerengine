#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0, set = 0) uniform ViewProj {
	mat4 view;
	mat4 proj;
} viewProj;

layout(binding = 0, set = 1) uniform Model {
	mat4 model;
} model;

layout(binding = 0, set = 2) uniform BoneTransforms {
	mat4 m[32];
} boneTransforms;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec3 bitangent;
layout(location = 4) in vec2 uv;
layout(location = 5) in ivec3 boneIds;
layout(location = 6) in vec3 boneWeights;

void main() {
	vec4 bonePosition1 = boneTransforms.m[max(0, boneIds[0])] * vec4(position, 1.0);
	vec4 bonePosition2 = boneTransforms.m[max(0, boneIds[1])] * vec4(position, 1.0);
	vec4 bonePosition3 = boneTransforms.m[max(0, boneIds[2])] * vec4(position, 1.0);
	vec4 bonePosition = bonePosition1 * boneWeights[0] + bonePosition2 * boneWeights[1] + bonePosition3 * boneWeights[2];
	
	vec4 pos = viewProj.proj * viewProj.view * model.model * vec4(bonePosition.xyz, 1);
    gl_Position = pos;
}