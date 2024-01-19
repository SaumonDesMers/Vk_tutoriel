#include "mesh.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <unordered_map>
#include <stdexcept>
#include <iostream>

namespace LIB_NAMESPACE
{
	Mesh::Mesh(
		VkDevice device,
		VkPhysicalDevice physicalDevice,
		Command& command,
		CreateInfo& meshInfo
	):
		m_vertexCount(meshInfo.vertices.size()),
		m_indexCount(meshInfo.indices.size())
	{
		createVertexBuffer(device, physicalDevice, command, meshInfo.vertices);
		createIndexBuffer(device, physicalDevice, command, meshInfo.indices);
	}

	Mesh::~Mesh()
	{
	}

	void Mesh::createVertexBuffer(
		VkDevice device,
		VkPhysicalDevice physicalDevice,
		Command& command,
		const std::vector<Vertex>& vertices
	)
	{
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

		Buffer stagingBuffer = Buffer::createStagingBuffer(
			device,
			physicalDevice,
			bufferSize
		);

		stagingBuffer.map();
		stagingBuffer.write((void*)vertices.data(), bufferSize);
		stagingBuffer.unmap();


		m_vertexBuffer = std::make_unique<Buffer>(Buffer::createVertexBuffer(
			device,
			physicalDevice,
			bufferSize
		));

		VkBufferCopy copyRegion = {};
		copyRegion.size = bufferSize;

		command.copyBufferToBuffer(stagingBuffer.buffer(), m_vertexBuffer->buffer(), 1, &copyRegion);
	}

	void Mesh::createIndexBuffer(
		VkDevice device,
		VkPhysicalDevice physicalDevice,
		Command& command,
		const std::vector<uint32_t>& indices
	)
	{
		VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

		Buffer stagingBuffer = Buffer::createStagingBuffer(
			device,
			physicalDevice,
			bufferSize
		);

		stagingBuffer.map();
		stagingBuffer.write((void*)indices.data(), bufferSize);
		stagingBuffer.unmap();


		m_indexBuffer = std::make_unique<Buffer>(Buffer::createIndexBuffer(
			device,
			physicalDevice,
			bufferSize
		));

		VkBufferCopy copyRegion = {};
		copyRegion.size = bufferSize;

		command.copyBufferToBuffer(stagingBuffer.buffer(), m_indexBuffer->buffer(), 1, &copyRegion);
	}

	void Mesh::readObjFile(
		const std::string& filename,
		std::vector<Vertex>& vertices,
		std::vector<uint32_t>& indices
	)
	{
		vertices.clear();
		indices.clear();

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warning, error;

		if (tinyobj::LoadObj(&attrib, &shapes, &materials, &warning, &error, filename.c_str()) == false)
		{
			throw std::runtime_error(warning + error);
		}

		std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				Vertex vertex = {};

				vertex.pos = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.texCoord = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};

				vertex.color = { 1.0f, 1.0f, 1.0f };

				if (uniqueVertices.count(vertex) == 0)
				{
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}

				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}
}
