#include "image.hpp"

#include <stdexcept>

namespace LIB_NAMESPACE
{
	Image::Image(
		VkDevice device,
		VkPhysicalDevice physicalDevice,
		VkImageCreateInfo& createInfo
	):
		m_device(device)
	{
		if (vkCreateImage(device, &createInfo, nullptr, &m_image) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image.");
		}
	}

	Image::~Image()
	{
		vkDestroyImage(m_device, m_image, nullptr);
	}
}