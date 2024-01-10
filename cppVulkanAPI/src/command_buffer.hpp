#pragma once

#include "defines.hpp"

#include <vulkan/vulkan.h>

namespace LIB_NAMESPACE
{
	class CommandBuffer
	{
		public:

		struct AllocateInfo: public VkCommandBufferAllocateInfo
		{
			AllocateInfo(): VkCommandBufferAllocateInfo()
			{
				this->sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			}
		};

		struct BeginInfo: public VkCommandBufferBeginInfo
		{
			BeginInfo(): VkCommandBufferBeginInfo()
			{
				this->sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			}
		};

		CommandBuffer(VkDevice device, const VkCommandBufferAllocateInfo& allocateInfo);
		~CommandBuffer();

		VkCommandBuffer getVk() const { return m_commandBuffer; }

		void begin(const VkCommandBufferBeginInfo& beginInfo);
		void end();
		VkResult reset(VkCommandBufferResetFlags flags = 0);
	
	private:
	
		VkCommandBuffer m_commandBuffer;

		VkDevice m_device;
		VkCommandPool m_commandPool;

	};
}