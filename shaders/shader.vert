#version 450

/* 
 * Vertex shader for the triangle example.
 * 
 * This shader is responsible for transforming the vertex positions from
 * model space to clip space. It also passes the vertex colors to the
 * fragment shader.
 */

/* Vertex attributes */
layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
}