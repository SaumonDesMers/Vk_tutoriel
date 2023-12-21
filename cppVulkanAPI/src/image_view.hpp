#pragma once

#include "defines.hpp"

#include <vulkan/vulkan.h>

namespace LIB_NAMESPACE
{
	class ImageView
	{
	
	public:

		struct CreateInfo: public VkImageViewCreateInfo
		{
			CreateInfo(): VkImageViewCreateInfo()
			{
				this->sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			}
		};

		ImageView(VkDevice device, const CreateInfo& createInfo);
		~ImageView();

		VkImageView getVk() const { return m_imageView; }

	private:

		VkImageView m_imageView;

		VkDevice m_device;

	};

}