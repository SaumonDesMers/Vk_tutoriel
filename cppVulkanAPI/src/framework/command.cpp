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

	void Command::copyBufferToBuffer(
		VkBuffer srcBuffer,
		VkBuffer dstBuffer,
		uint32_t regionCount,
		const VkBufferCopy *pRegions
	)
	{
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		vkCmdCopyBuffer(
			commandBuffer,
			srcBuffer,
			dstBuffer,
			regionCount,
			pRegions
		);

		endSingleTimeCommands(commandBuffer);
	}

	void Command::copyBufferToImage(
		VkBuffer srcBuffer,
		VkImage dstImage,
		VkImageLayout dstImageLayout,
		uint32_t regionCount,
		const VkBufferImageCopy *pRegions
	)
	{
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		vkCmdCopyBufferToImage(
			commandBuffer,
			srcBuffer,
			dstImage,
			dstImageLayout,
			regionCount,
			pRegions
		);

		endSingleTimeCommands(commandBuffer);
	}

	void Command::transitionImageLayout(
		VkImage image,
		VkFormat format,
		VkImageLayout oldLayout,
		VkImageLayout newLayout,
		uint32_t mipLevels
	)
	{
		(void)format;

		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;

		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (
			oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
			newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
		)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (
			oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
			newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
		{
			throw std::invalid_argument("unsupported image layout transition.");
		}

		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		endSingleTimeCommands(commandBuffer);
	}


}