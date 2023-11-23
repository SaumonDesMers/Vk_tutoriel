#pragma once

#include "device.hpp"

#include "ft_glm/ft_glm.hpp"
#include <glm/glm.hpp>

namespace ft
{
	
	class Model
	{
		
	public:

		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 color;

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};
		
		Model(Device &device, const std::vector<Vertex> &vertices);
		~Model();
		
		Model(const Model &) = delete;
		void operator=(const Model &) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);
		
	private:
		
		Device &m_device;

		VkBuffer m_vertexBuffer;
		VkDeviceMemory m_vertexBufferMemory;
		uint32_t m_vertexCount;

		void createVertexBuffer(const std::vector<Vertex> &vertices);

	};

}
