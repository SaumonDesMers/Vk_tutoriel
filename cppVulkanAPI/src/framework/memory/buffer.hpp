#pragma once

#include "defines.hpp"
#include "core/buffer.hpp"
#include "core/device_memory.hpp"

#include <memory>

namespace LIB_NAMESPACE
{
	class Buffer
	{

	public:

		Buffer(
			VkDevice device,
			VkPhysicalDevice physicalDevice,
			VkBufferCreateInfo bufferInfo,
			VkMemoryPropertyFlags properties
		);
		~Buffer();

		VkResult map(
			VkDeviceSize offset = 0,
			VkDeviceSize size = VK_WHOLE_SIZE,
			VkMemoryMapFlags flags = 0
		);
		void unmap();

		void write(void *data, uint32_t size);

		void copyToBuffer(Buffer& buffer);

	private:

		std::unique_ptr<core::Buffer> m_buffer;
		std::unique_ptr<core::DeviceMemory> m_memory;

	};
}