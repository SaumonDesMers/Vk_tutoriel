#pragma once

#include "../defines.hpp"

#include <vulkan/vulkan.h>

namespace LIB_NAMESPACE
{
	class Image
	{

	public:

		Image(
			VkDevice device,
			VkImageCreateInfo& createInfo
		);
		~Image();

		VkImage getVk() const { return m_image; }

		VkMemoryRequirements getMemoryRequirements() const;

		VkResult bindMemory(
			VkDeviceMemory memory,
			VkDeviceSize memoryOffset = 0
		) const;

	private:

		VkImage m_image;

		VkDevice m_device;
	};
}