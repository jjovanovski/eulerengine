#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragUv;
layout(location = 2) in vec3 fragPos;
layout(location = 4) in mat3 tbn;

layout(location = 0) out vec4 outColor;

layout(binding = 0, set = 2) uniform sampler2D colorMap;
layout(binding = 0, set = 5) uniform sampler2D normalMap;

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

layout(binding = 0, set = 6) uniform MaterialProperties {
	float shininess;
	float useNormalMap;
	float useSpecularMap;
} materialProperties;


void main() {
	vec3 ambLight = ambientLight.color * ambientLight.intensity;
	
	vec3 lightDir = normalize(directionalLight.direction);
	vec3 surfaceNormal = normalize(fragNormal);
	if(materialProperties.useNormalMap > 0.0) {
		surfaceNormal = texture(normalMap, fragUv).rgb;
		//surfaceNormal = surfaceNormal * 2.0 - 1.0;
		surfaceNormal = tbn * normalize(surfaceNormal);
		surfaceNormal = normalize(surfaceNormal);
	}
	vec3 dirLight = directionalLight.color * max(0, dot(-lightDir, surfaceNormal)) * directionalLight.intensity;
	
	// specular 
    vec3 viewDir = normalize(ambientLight.cameraPosition - fragPos);
    vec3 halfwayDir = normalize(-lightDir + viewDir);
	float spec = pow(max(dot(surfaceNormal, halfwayDir), 0.0), materialProperties.shininess);
    vec3 specular = directionalLight.color * spec; 
	
	vec3 texcolor = texture(colorMap, fragUv).xyz;
    outColor = vec4(texcolor * (ambLight + dirLight + specular), 1);
    //outColor = vec4(surfaceNormal, 1);
}