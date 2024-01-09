#include "buffer.hpp"

#include <stdexcept>
#include <string.h>

namespace LIB_NAMESPACE
{
	Buffer::Buffer(
		VkDevice device,
		VkPhysicalDevice physicalDevice,
		VkMemoryPropertyFlags properties,
		VkBufferCreateInfo& createInfo
	)
		:m_device(device),
		m_mappedData(nullptr)
	{
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

		if (vkCreateBuffer(device, &createInfo, nullptr, &m_buffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create vertex buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, m_buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(
			physicalDevice,
			memRequirements.memoryTypeBits,
			properties
		);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &m_bufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate vertex buffer memory!");
		}

		vkBindBufferMemory(device, m_buffer, m_bufferMemory, 0);
	}

	Buffer::~Buffer()
	{
		vkDestroyBuffer(m_device, m_buffer, nullptr);
		vkFreeMemory(m_device, m_bufferMemory, nullptr);
	}

	uint32_t Buffer::findMemoryType(
		VkPhysicalDevice physicalDevice,
		uint32_t typeFilter,
		VkMemoryPropertyFlags properties
	)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if (
				(typeFilter & (1 << i))
				&& (memProperties.memoryTypes[i].propertyFlags & properties) == properties
			)
			{
				return i;
			}
		}

		throw std::runtime_error("buffer: failed to find suitable memory type.");
	}

	VkResult Buffer::map(VkDeviceSize offset, VkDeviceSize size)
	{
		return vkMapMemory(m_device, m_bufferMemory, offset, size, 0, &m_mappedData);
	}

	void Buffer::unmap()
	{
		vkUnmapMemory(m_device, m_bufferMemory);
	}

	void Buffer::write(void *data, uint32_t size)
	{
		map();
		memcpy(m_mappedData, data, size);
		unmap();
	}
}