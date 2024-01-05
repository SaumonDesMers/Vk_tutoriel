#pragma once

#include "defines.hpp"

#include <vulkan/vulkan.h>

namespace LIB_NAMESPACE
{
	class Semaphore
	{

	public:

		struct CreateInfo: public VkSemaphoreCreateInfo
		{
			CreateInfo(): VkSemaphoreCreateInfo()
			{
				this->sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			}
		};

		Semaphore(VkDevice device, const CreateInfo& createInfo);
		~Semaphore();

		VkSemaphore getVk() const { return m_semaphore; }

	private:

		VkSemaphore m_semaphore;

		VkDevice m_device;

	};



	class Fence
	{

	public:

		struct CreateInfo: public VkFenceCreateInfo
		{
			CreateInfo(): VkFenceCreateInfo()
			{
				this->sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			}
		};

		Fence(VkDevice device, const CreateInfo& createInfo);
		~Fence();

		VkFence getVk() const { return m_fence; }

		void wait(uint64_t timeout = UINT64_MAX);
		void reset();

	private:

		VkFence m_fence;

		VkDevice m_device;

	};
}