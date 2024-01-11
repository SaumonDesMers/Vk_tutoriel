#pragma once

#include "defines.hpp"

#include <vulkan/vulkan.h>

namespace LIB_NAMESPACE
{
	class Image
	{

	public:

		Image(
			VkDevice device,
			VkPhysicalDevice physicalDevice,
			VkImageCreateInfo& createInfo
		);
		~Image();

		VkImage getVk() { return m_image; }
	

	private:

		VkImage m_image;

		VkDevice m_device;
	};
}