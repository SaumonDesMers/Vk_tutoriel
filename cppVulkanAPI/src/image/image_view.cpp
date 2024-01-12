#include "image_view.hpp"

#include <stdexcept>

namespace LIB_NAMESPACE
{
	ImageView::ImageView(VkDevice device, const VkImageViewCreateInfo& createInfo)
		: m_device(device)
	{
		if (vkCreateImageView(m_device, &createInfo, nullptr, &m_imageView) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image view.");
		}
	}

	ImageView::~ImageView()
	{
		vkDestroyImageView(m_device, m_imageView, nullptr);
	}

}