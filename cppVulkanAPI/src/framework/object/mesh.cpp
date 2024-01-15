#include "mesh.hpp"
#include "framework/memory/staging_buffer.hpp"

namespace LIB_NAMESPACE
{
	Mesh::Mesh(
		VkDevice device,
		VkPhysicalDevice physicalDevice,
		MeshCreateInfo meshInfo
	)
	{
		uint32_t verticesSize = meshInfo.vertices.size() * sizeof(Vertex);

		StagingBuffer stagingBuffer(device, physicalDevice, meshInfo.vertices.data(), verticesSize);
		
	}

	Mesh::~Mesh()
	{
	}
}
