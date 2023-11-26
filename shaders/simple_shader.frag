#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragWorldPos;
layout (location = 2) in vec3 fragWorldNormal;

layout (location = 0) out vec4 outColor;

struct LightPoint
{
	vec4 position;
	vec4 color;
};

layout (set = 0, binding = 0) uniform GlobalUbo
{
	mat4 projection;
	mat4 view;
	vec4 ambientLightColor;
	LightPoint light[10];
	int lightCount;
} ubo;

layout (push_constant) uniform Push {
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main()
{
	vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
	vec3 surfaceNormal = normalize(fragWorldNormal);

	for (int i = 0; i < ubo.lightCount; i++)
	{
		LightPoint light = ubo.light[i];

		vec3 lightDirection = light.position.xyz - fragWorldPos;
		float attenuation = 1.0 / dot(lightDirection, lightDirection);
		float cosAngIncidence = max(dot(surfaceNormal, normalize(lightDirection)), 0.0);
		vec3 intensity = light.color.xyz * light.color.w * attenuation;

		diffuseLight += intensity * cosAngIncidence;
	}

	outColor = vec4(diffuseLight * fragColor, 1.0);
}
