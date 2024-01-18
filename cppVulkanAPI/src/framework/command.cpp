#include "command.hpp"

#include <stdexcept>

namespace LIB_NAMESPACE
{
	Command::Command(VkDevice device, const CreateInfo& createInfo)
		: m_device(device), m_queue(createInfo.queue)
	{
		createCommandPool(createInfo);
	}

	Command::~Command()
	{
		vkDestroyCommandPool(m_device, m_commandPool, nullptr);
	}

	void Command::createCommandPool(const CreateInfo& createInfo)
	{
		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = createInfo.queueFamilyIndex;
		poolInfo.flags = createInfo.flags;

		VkResult result = vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool);
		if (result != VK_SUCCESS)
		{
			TROW("Failed to create command pool", result);
		}
	}

	VkCommandBuffer Command::allocateCommandBuffer(VkCommandBufferLevel level)
	{
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_commandPool;
		allocInfo.level = level;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		VkResult result = vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer);
		if (result != VK_SUCCESS)
		{
			TROW("Failed to allocate command buffer", result);
		}

		return commandBuffer;
	}

	void Command::freeCommandBuffer(VkCommandBuffer commandBuffer)
	{
		vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
	}

	VkCommandBuffer Command::beginSingleTimeCommands()
	{
		VkCommandBuffer commandBuffer = allocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
		if (result != VK_SUCCESS)
		{
			TROW("Failed to begin recording command buffer", result);
		}

		return commandBuffer;
	}

	void Command::endSingleTimeCommands(VkCommandBuffer commandBuffer)
	{
		VkResult result = vkEndCommandBuffer(commandBuffer);
		if (result != VK_SUCCESS)
		{
			TROW("Failed to record command buffer", result);
		}

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		result = vkQueueSubmit(m_queue, 1, &submitInfo, VK_NULL_HANDLE);
		if (result != VK_SUCCESS)
		{
			TROW("Failed to submit queue", result);
		}

		result = vkQueueWaitIdle(m_queue);
		if (result != VK_SUCCESS)
		{
			TROW("Failed to wait for queue to be idle", result);
		}

		freeCommandBuffer(commandBuffer);
	}
}