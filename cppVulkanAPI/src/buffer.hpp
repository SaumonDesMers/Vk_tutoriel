#pragma once

#include "defines.hpp"

#include <vulkan/vulkan.h>

namespace LIB_NAMESPACE
{
	class Buffer
	{

	public:

		struct CreateInfo: public VkBufferCreateInfo
		{
			CreateInfo(): VkBufferCreateInfo()
			{
				sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			}
		};

		Buffer(
			VkDevice device,
			VkPhysicalDevice physicalDevice,
			VkMemoryPropertyFlags properties,
			VkBufferCreateInfo& createInfo
		);
		~Buffer();

		VkBuffer getVk() { return m_buffer; }
		
		void write(void *data, uint32_t size);
	
	private:

		VkBuffer m_buffer;
		VkDeviceMemory m_bufferMemory;

		VkDevice m_device;

		void *m_mappedData;

		uint32_t findMemoryType(
			VkPhysicalDevice physicalDevice,
			uint32_t typeFilter,
			VkMemoryPropertyFlags properties
		);

		VkResult map(
			VkDeviceSize offset = 0,
			VkDeviceSize size = VK_WHOLE_SIZE
		);
		void unmap();		
	};
}
