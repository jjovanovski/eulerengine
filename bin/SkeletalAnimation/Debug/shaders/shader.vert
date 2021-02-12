#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0, set = 0) uniform ViewProj {
	mat4 view;
	mat4 proj;
} viewProj;

layout(binding = 0, set = 1) uniform Model {
	mat4 model;
} model;

layout(binding = 0, set = 4) uniform BoneTransforms {
	mat4 m[32];
} boneTransforms;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in ivec3 boneIds;
layout(location = 4) in vec3 boneWeights;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragUv;
layout(location = 2) out vec3 fragPos;

void main() {
	vec4 bonePosition1 = boneTransforms.m[max(0, boneIds[0])] * vec4(position, 1.0);
	vec4 bonePosition2 = boneTransforms.m[max(0, boneIds[1])] * vec4(position, 1.0);
	vec4 bonePosition3 = boneTransforms.m[max(0, boneIds[2])] * vec4(position, 1.0);
	vec4 bonePosition = bonePosition1 * boneWeights[0] + bonePosition2 * boneWeights[1] + bonePosition3 * boneWeights[2];
	
    gl_Position = viewProj.proj * viewProj.view * model.model * bonePosition;
    fragNormal = mat3(transpose(inverse(model.model))) * normal;
	fragUv = uv;
	fragPos = vec3(model.model * vec4(position.x, -position.y, position.z, 1.0));
}