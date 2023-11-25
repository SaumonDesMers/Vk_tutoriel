#pragma once

#include "device.hpp"
#include "buffer.hpp"

#include "ft_glm/ft_glm.hpp"
#include <glm/glm.hpp>

#include <memory>

namespace ft
{
	
	class Model
	{
		
	public:

		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 color;
			glm::vec3 normal;
			glm::vec2 uv;

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex &other) const
			{
				return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
			}
		};

		struct Builder
		{
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string &filepath);
		};
		
		Model(Device &device, const Model::Builder &builder);
		~Model();
		
		Model(const Model &) = delete;
		void operator=(const Model &) = delete;

		static std::unique_ptr<Model> createModelFromFile(Device &device, const std::string &filepath);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);
		
	private:
		
		Device &m_device;

		std::unique_ptr<Buffer> m_vertexBuffer;
		uint32_t m_vertexCount;

		bool m_hasIndexBuffer = false;
		std::unique_ptr<Buffer> m_indexBuffer;
		uint32_t m_indexCount;

		void createVertexBuffer(const std::vector<Vertex> &vertices);
		void createIndexBuffer(const std::vector<uint32_t> &indices);

	};

}
