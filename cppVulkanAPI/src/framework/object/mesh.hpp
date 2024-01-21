#pragma once

#include "defines.hpp"
#include "framework/memory/buffer.hpp"
#include "framework/command.hpp"
#include "vertex.hpp"

#include <vector>

namespace LIB_NAMESPACE
{
    class Mesh
    {
    
    public:

		typedef uint32_t ID;

		struct CreateInfo
		{
			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;
		};

		Mesh(
			VkDevice device,
			VkPhysicalDevice physicalDevice,
			Command& command,
			CreateInfo& meshInfo
		);
		~Mesh();

		inline Buffer& vertexBuffer() { return *m_vertexBuffer; }
		inline uint32_t vertexCount() { return m_vertexCount; }
		inline Buffer& indexBuffer() { return *m_indexBuffer; }
		inline uint32_t indexCount() { return m_indexCount; }

		static void readObjFile(
			const std::string& filename,
			std::vector<Vertex>& vertices,
			std::vector<uint32_t>& indices
		);

    private:

		std::unique_ptr<Buffer> m_vertexBuffer;
		uint32_t m_vertexCount;
		std::unique_ptr<Buffer> m_indexBuffer;
		uint32_t m_indexCount;

		void createVertexBuffer(
			VkDevice device,
			VkPhysicalDevice physicalDevice,
			Command& command,
			const std::vector<Vertex>& vertices
		);

		void createIndexBuffer(
			VkDevice device,
			VkPhysicalDevice physicalDevice,
			Command& command,
			const std::vector<uint32_t>& indices
		);

    };
}