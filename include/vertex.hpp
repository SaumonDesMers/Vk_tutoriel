#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <glm/glm.hpp>
#include <array>

struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;

	/* The binding description describes at which rate to load data from memory throughout the vertices. */
	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		/* The binding parameter specifies the index of the binding in the array of bindings */
		bindingDescription.binding = 0;
		/* The stride parameter specifies the number of bytes from one entry to the next */
		bindingDescription.stride = sizeof(Vertex);
		/* The inputRate parameter can have one of the following values:
		 * VK_VERTEX_INPUT_RATE_VERTEX: Move to the next data entry after each vertex
		 * VK_VERTEX_INPUT_RATE_INSTANCE: Move to the next data entry after each instance
		 */
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	/* The attribute descriptions describe how to extract a vertex attribute from a chunk of vertex data originating from a binding description. */
	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

		/* The binding parameter specifies which binding the per-vertex data comes from */
		attributeDescriptions[0].binding = 0;
		/* The location parameter references the location directive of the input in the vertex shader */
		attributeDescriptions[0].location = 0;
		/* The format parameter describes the type of data for the attribute
		 * float: VK_FORMAT_R32_SFLOAT
		 * vec2: VK_FORMAT_R32G32_SFLOAT
		 * vec3: VK_FORMAT_R32G32B32_SFLOAT
		 * vec4: VK_FORMAT_R32G32B32A32_SFLOAT
		 * ivec2: VK_FORMAT_R32G32_SINT : 2-components of 32 bit signed integer
		 * uvec4: VK_FORMAT_R32G32B32A32_UINT : 4-components of 32 bit unsigned integer
		 * double: VK_FORMAT_R64_SFLOAT : a double-precision (64-bit) float
		 */
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		/* The offset parameter specifies where the data for this attribute is located within the vertex data, this is calculated with offsetof macro */
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}
};

/* Always align uniform buffer objects to avoid issues with padding */
struct UniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};


const std::vector<Vertex> vertices = {
	{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
	{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
	{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

const std::vector<uint32_t> indices = {
	0, 1, 2, 2, 3, 0
};

#endif