#include "buffer.hpp"

namespace LIB_NAMESPACE
{
	Buffer::Buffer(
		VkDevice device,
		VkPhysicalDevice physicalDevice,
		VkBufferCreateInfo bufferInfo,
		VkMemoryPropertyFlags properties
	)
	{
		m_buffer = std::make_unique<core::Buffer>(device, bufferInfo);

		VkMemoryRequirements memRequirements = m_buffer->getMemoryRequirements();

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = core::DeviceMemory::findMemoryType(
			physicalDevice,
			memRequirements.memoryTypeBits,
			properties
		);

		m_memory = std::make_unique<core::DeviceMemory>(device, allocInfo);

		vkBindBufferMemory(device, m_buffer->getVk(), m_memory->getVk(), 0);
	}

	Buffer::~Buffer()
	{
	}

	VkResult Buffer::map(
		VkDeviceSize offset = 0,
		VkDeviceSize size = VK_WHOLE_SIZE,
		VkMemoryMapFlags flags = 0
	)
	{
		return m_memory->map(offset, size, flags);
	}

	void Buffer::unmap()
	{
		m_memory->unmap();
	}

	void Buffer::write(void *data, uint32_t size)
	{
		m_memory->write(data, size);
	}
}