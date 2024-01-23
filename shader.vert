#version 450

layout (set = 0, binding = 1) uniform sampler2D textureSampler;

layout (set = 0, binding = 0) uniform UniformBufferObject {
	mat4 model;
	mat4 view;
	mat4 proj;
} ubo;

layout (location = 0) in vec4 position;

layout (location = 0) out vec4 outColor;

void main() {
	
}