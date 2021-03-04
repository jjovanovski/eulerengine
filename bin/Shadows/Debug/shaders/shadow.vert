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
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec3 bitangent;
layout(location = 4) in vec2 uv;

void main() {
	vec4 pos = viewProj.proj * viewProj.view * model.model * vec4(position, 1);
    gl_Position = pos;
}