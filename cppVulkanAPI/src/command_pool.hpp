#pragma once

#include "defines.hpp"

#include <vulkan/vulkan.h>

namespace LIB_NAMESPACE
{
	namespace core
	{
		class CommandPool
		{
			public:

			struct CreateInfo: public VkCommandPoolCreateInfo
			{
				CreateInfo(): VkCommandPoolCreateInfo()
				{
					this->sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				}
			};

			CommandPool(VkDevice device, const CreateInfo& createInfo);
			~CommandPool();

			VkCommandPool getVk() const { return m_commandPool; }

		private:

			VkCommandPool m_commandPool;

			VkDevice m_device;

		};
	}
}