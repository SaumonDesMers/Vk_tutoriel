#include "mesh.hpp"

namespace LIB_NAMESPACE
{
	Mesh::Mesh(
		VkDevice device,
		VkPhysicalDevice physicalDevice,
		MeshCreateInfo meshInfo
	)
	{
		uint32_t verticesSize = meshInfo.vertices.size() * sizeof(Vertex);

		
	}

	Mesh::~Mesh()
	{
	}
}
