#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <ft_glm/ft_glm.hpp>

#include <array>
#include <vector>

struct Vertex {
	ft::vec3 pos;
	ft::vec3 color;
	ft::vec2 texCoord;
	ft::vec3 normal;

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
	static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

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
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		/* The offset parameter specifies where the data for this attribute is located within the vertex data, this is calculated with offsetof macro */
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

		attributeDescriptions[3].binding = 0;
		attributeDescriptions[3].location = 3;
		attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(Vertex, normal);

		return attributeDescriptions;
	}

	bool operator==(const Vertex& other) const {
		return pos == other.pos && color == other.color && texCoord == other.texCoord && normal == other.normal;
	}

	void log() const {
		std::cout << "Vertex: " << std::endl;
		std::cout << "\tpos: " << pos[0] << ", " << pos[1] << ", " << pos[2] << std::endl;
		// std::cout << "\tcolor: " << color[0] << ", " << color[1] << ", " << color[2] << std::endl;
		// std::cout << "\ttexCoord: " << texCoord[0] << ", " << texCoord[1] << std::endl;
		// std::cout << "\tnormal: " << normal[0] << ", " << normal[1] << ", " << normal[2] << std::endl;
	}
};

namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(Vertex const& vertex) const {
			return (
				(hash<ft::vec3>()(vertex.pos) ^
				(hash<ft::vec3>()(vertex.color) << 1)) >> 1) ^
				(hash<ft::vec2>()(vertex.texCoord) << 1) ^
				(hash<ft::vec3>()(vertex.normal) << 1
			);
		}
	};
}

/* Always align uniform buffer objects to avoid issues with padding */
struct ModelViewPerspective {
	alignas(16) ft::mat4 model;
	alignas(16) ft::mat4 view;
	alignas(16) ft::mat4 proj;
};

/* TODO: remove this */
struct Glm_ModelViewPerspective {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

#endif