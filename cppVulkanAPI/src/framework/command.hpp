#pragma once

#include "defines.hpp"

#include <vulkan/vulkan.h>

#include <vector>

namespace LIB_NAMESPACE
{
	class Command
	{
	
	public:

		struct CreateInfo
		{
			VkCommandPoolCreateFlags flags = 0;
			uint32_t queueFamilyIndex = 0;
			VkQueue queue = VK_NULL_HANDLE;
		};

		Command(VkDevice device, const CreateInfo& createInfo);
		~Command();

		VkCommandBuffer allocateCommandBuffer(VkCommandBufferLevel level);
		void freeCommandBuffer(VkCommandBuffer commandBuffer);

		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);

		void submit(
			uint32_t submitCount,
			const VkSubmitInfo *pSubmits,
			VkFence fence
		);

		void copyBufferToBuffer(
			VkBuffer srcBuffer,
			VkBuffer dstBuffer,
			uint32_t regionCount,
			const VkBufferCopy *pRegions
		);

		void copyBufferToImage(
			VkBuffer srcBuffer,
			VkImage dstImage,
			VkImageLayout dstImageLayout,
			uint32_t regionCount,
			const VkBufferImageCopy *pRegions
		);

		void copyImageToImage(
			VkImage srcImage,
			VkImageLayout srcImageLayout,
			VkImage dstImage,
			VkImageLayout dstImageLayout,
			uint32_t regionCount,
			const VkImageCopy *pRegions
		);

		void transitionImageLayout(
			VkImage image,
			VkImageLayout oldLayout,
			VkImageLayout newLayout,
			VkImageAspectFlags aspectMask,
			uint32_t mipLevels,
			VkAccessFlags srcAccessMask,
			VkAccessFlags dstAccessMask,
			VkPipelineStageFlags srcStageMask,
			VkPipelineStageFlags dstStageMask
		);

	private:

		VkCommandPool m_commandPool;

		VkDevice m_device;
		VkQueue m_queue;

		void createCommandPool(const CreateInfo& createInfo);
	
	};
}