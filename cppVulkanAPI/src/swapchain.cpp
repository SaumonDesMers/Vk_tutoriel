#include "swapchain.hpp"

#include <stdexcept>

namespace LIB_NAMESPACE
{
	namespace core
	{
		Swapchain::Swapchain(VkDevice device, const CreateInfo& createInfo)
			: m_device(device)
		{
			if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapchain) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create swap chain.");
			}

			uint32_t imageCount;
			vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, nullptr);
			m_images.resize(imageCount);
			vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, m_images.data());

			m_imageFormat = createInfo.imageFormat;
			m_extent = createInfo.imageExtent;
		}

		Swapchain::~Swapchain()
		{
			vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
		}

		VkResult Swapchain::acquireNextImage(uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* imageIndex)
		{
			return vkAcquireNextImageKHR(m_device, m_swapchain, timeout, semaphore, fence, imageIndex);
		}
	}
}