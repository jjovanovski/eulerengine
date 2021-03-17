#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragUv;
layout(location = 2) in vec3 fragPos;
layout(location = 3) in vec3 lightFragPos;

layout(location = 0) out vec4 outColor;

layout(binding = 0, set = 2) uniform sampler2D tex;
layout(binding = 1, set = 2) uniform Material {
	float shininess;
} material;

layout(binding = 0, set = 3) uniform DirectionalLight {
	vec3 direction;
	vec3 color;
	float intensity;
} directionalLight;

layout(binding = 1, set = 3) uniform AmbientLight {
	vec3 cameraPosition;
	vec3 color;
	float intensity;
} ambientLight;

layout(binding = 2, set = 3) uniform sampler2D shadowMap;

float ShadowCalc() {
	vec3 tmp = lightFragPos;
	tmp.x = tmp.x * 0.5 + 0.5;
	tmp.y = -(tmp.y * 0.5 + 0.5);
	
	float closestDepth = texture(shadowMap, tmp.xy).r;
	
	float currentDepth = tmp.z;
	
	float bias = 0.005;
	float shadow = currentDepth - bias < closestDepth ? 1.0 : 0.2;
	
	return shadow;
}

void main() {
	vec3 ambLight = ambientLight.color * ambientLight.intensity;
	
	vec3 lightDir = -normalize(directionalLight.direction);
	vec3 dirLight = directionalLight.color * max(0, dot(lightDir, normalize(fragNormal))) * directionalLight.intensity;
	
	// specular
    vec3 viewDir = normalize(ambientLight.cameraPosition - fragPos);
    vec3 reflectDir = reflect(lightDir, fragNormal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = directionalLight.color * spec * 0.5; 
	
    outColor = vec4(texture(tex, fragUv).xyz * (dirLight + ambLight), 1);
}