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
		m_isMapped(false),
		m_mappedMemory(nullptr),
		m_writedDataSize(0)
	{
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

		init(createInfo, physicalDevice, properties);
	}

	Buffer::Buffer(
		VkDevice device,
		VkPhysicalDevice physicalDevice,
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties
	)
		:m_device(device),
		m_isMapped(false),
		m_mappedMemory(nullptr),
		m_writedDataSize(0)
	{
		VkBufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = size;
		createInfo.usage = usage;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		init(createInfo, physicalDevice, properties);
	}

	Buffer::~Buffer()
	{
		vkDestroyBuffer(m_device, m_buffer, nullptr);
		vkFreeMemory(m_device, m_bufferMemory, nullptr);
	}

	void Buffer::init(
		const VkBufferCreateInfo& createInfo,
		VkPhysicalDevice physicalDevice,
		const VkMemoryPropertyFlags& properties
	)
	{
		if (vkCreateBuffer(m_device, &createInfo, nullptr, &m_buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create vertex buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(m_device, m_buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(
			physicalDevice,
			memRequirements.memoryTypeBits,
			properties
		);

		if (vkAllocateMemory(m_device, &allocInfo, nullptr, &m_bufferMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate vertex buffer memory!");
		}

		vkBindBufferMemory(m_device, m_buffer, m_bufferMemory, 0);
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

		throw std::runtime_error("failed to find suitable memory type for buffer.");
	}

	VkResult Buffer::map(VkDeviceSize offset, VkDeviceSize size)
	{
		VkResult result = vkMapMemory(m_device, m_bufferMemory, offset, size, 0, &m_mappedMemory);
		if (result == VK_SUCCESS)
		{
			m_isMapped = true;
		}
		return result;
	}

	void Buffer::unmap()
	{
		vkUnmapMemory(m_device, m_bufferMemory);
		m_isMapped = false;
	}

	void Buffer::write(void *data, uint32_t size)
	{
		if (m_isMapped == true)
		{
			memcpy(m_mappedMemory, data, size);
		}
		else
		{
			map();
			if (m_isMapped == true)
			{
				memcpy(m_mappedMemory, data, size);
			}
			else
			{
				throw std::runtime_error("cannot map to unmaped buffer.");
			}
			unmap();
		}
	}
	
}