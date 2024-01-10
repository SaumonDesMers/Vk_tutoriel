#include "command_buffer.hpp"

#include <stdexcept>

namespace LIB_NAMESPACE
{
	CommandBuffer::CommandBuffer(VkDevice device, const VkCommandBufferAllocateInfo& allocateInfo)
		: m_device(device), m_commandPool(allocateInfo.commandPool)
	{
		if (vkAllocateCommandBuffers(device, &allocateInfo, &m_commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers.");
		}
	}

	CommandBuffer::~CommandBuffer()
	{
		vkFreeCommandBuffers(m_device, m_commandPool, 1, &m_commandBuffer);
	}

	void CommandBuffer::begin(const VkCommandBufferBeginInfo& beginInfo)
	{
		if (vkBeginCommandBuffer(m_commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer.");
		}
	}

	void CommandBuffer::end()
	{
		if (vkEndCommandBuffer(m_commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer.");
		}
	}

	VkResult CommandBuffer::reset(VkCommandBufferResetFlags flags)
	{
		return vkResetCommandBuffer(m_commandBuffer, flags);
	}
}