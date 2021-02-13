#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0, set = 0) uniform ViewProj {
	mat4 view;
	mat4 proj;
} viewProj;

layout(binding = 0, set = 1) uniform Model {
	mat4 model;
} model;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragUv;
layout(location = 2) out vec3 fragPos;

void main() {
    fragNormal = mat3(transpose(inverse(model.model))) * normal;
	fragUv = uv;
	vec4 pos = viewProj.proj * viewProj.view * model.model * vec4(position, 1);
	fragPos = pos.xyz;
    gl_Position = pos;
}