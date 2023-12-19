#pragma once

#include "defines.hpp"

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace LIB_NAMESPACE
{
	class Swapchain
	{
	
	public:

		struct CreateInfo: public VkSwapchainCreateInfoKHR
		{
			CreateInfo(): VkSwapchainCreateInfoKHR()
			{
				this->sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			}
		};

		Swapchain(VkDevice device, const CreateInfo& createInfo);
		~Swapchain();

		VkSwapchainKHR getVk() const { return m_swapchain; }

		VkImage getImage(uint32_t index) const { return m_images[index]; }
		VkFormat getImageFormat() const { return m_imageFormat; }
		VkExtent2D getExtent() const { return m_extent; }

	private:

		VkSwapchainKHR m_swapchain;

		VkDevice m_device;

		std::vector<VkImage> m_images;
		VkFormat m_imageFormat;
		VkExtent2D m_extent;

	};

}