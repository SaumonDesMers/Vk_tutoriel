#pragma once

#include "defines.hpp"
#include "framework/memory/buffer.hpp"
#include "vertex.hpp"

#include <vector>

namespace LIB_NAMESPACE
{
    class Mesh
    {
    
    public:

		struct MeshCreateInfo
		{
			std::vector<Vertex>& vertices;
			std::vector<uint32_t>& indices;
		};

		Mesh(
			VkDevice device,
			VkPhysicalDevice physicalDevice,
			MeshCreateInfo meshInfo
		);
		~Mesh();

    private:

		std::unique_ptr<core::Buffer> m_vertexBuffer;
		std::unique_ptr<core::Buffer> m_indexBuffer;

    };
}