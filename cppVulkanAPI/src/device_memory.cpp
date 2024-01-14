#include "device_memory.hpp"

#include <stdexcept>
#include <string.h>

namespace LIB_NAMESPACE
{
	namespace core
	{
		DeviceMemory::DeviceMemory(
			VkDevice device,
			VkMemoryAllocateInfo& allocInfo
		):
			m_device(device)
		{
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

			if (vkAllocateMemory(device, &allocInfo, nullptr, &m_memory) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to allocate vertex buffer memory.");
			}
		}

		DeviceMemory::~DeviceMemory()
		{
			vkFreeMemory(m_device, m_memory, nullptr);
		}

		uint32_t DeviceMemory::findMemoryType(
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

		VkResult DeviceMemory::map(
			VkDeviceSize offset,
			VkDeviceSize size,
			VkMemoryMapFlags flags
		)
		{
			VkResult result = vkMapMemory(m_device, m_memory, offset, size, flags, &m_mappedMemory);

			if (result == VK_SUCCESS)
			{
				m_isMapped = true;
			}

			return result;
		}

		void DeviceMemory::unmap()
		{
			vkUnmapMemory(m_device, m_memory);
			m_isMapped = false;
		}

		void DeviceMemory::write(void *data, uint32_t size)
		{
			if (m_isMapped == false)
			{
				throw std::runtime_error("failed to write data to memory: memory is not mapped.");
			}

			memcpy(m_mappedMemory, data, size);

		}
	}
}