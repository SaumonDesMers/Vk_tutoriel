#version 450

/* 
 * This shader is responsible for transforming the vertex positions from
 * model space to clip space. It also passes the vertex colors to the
 * fragment shader.
 */

layout(binding = 0) uniform ModelViewPerspective {
    mat4 model;
    mat4 view;
    mat4 proj;
} mvp;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = mvp.proj * mvp.view * mvp.model * vec4(inPosition, 1.0);
    fragColor = inColor;
	// fragColor = inNormal;
    fragTexCoord = inTexCoord;
}
