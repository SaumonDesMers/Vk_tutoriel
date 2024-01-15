#include "staging_buffer.hpp"

namespace LIB_NAMESPACE
{
	StagingBuffer::StagingBuffer(
		VkDevice device,
		VkPhysicalDevice physicalDevice,
		void* data,
		uint32_t size
	):
		Buffer(
			device,
			physicalDevice,
			getBufferCreateInfo(size),
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		)
	{
		map();
		write(data, size);
		unmap();
	}

	StagingBuffer::~StagingBuffer()
	{
	}

	VkBufferCreateInfo getBufferCreateInfo(uint32_t bufferSize)
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = static_cast<VkDeviceSize>(bufferSize);
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}
}