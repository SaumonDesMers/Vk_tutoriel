#pragma once

#include "defines.hpp"
#include "device_memory.hpp"

#include <vulkan/vulkan.h>

namespace LIB_NAMESPACE
{
	class Buffer
	{

	public:

		struct CreateInfo: public VkBufferCreateInfo
		{
			CreateInfo(): VkBufferCreateInfo()
			{
				sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			}
		};

		Buffer(
			VkDevice device,
			VkBufferCreateInfo& createInfo
		);
		Buffer(
			VkDevice device,
			VkDeviceSize size,
			VkBufferUsageFlags usage
		);
		~Buffer();

		VkBuffer getVk() { return m_buffer; }

		VkMemoryRequirements getMemoryRequirements();
	
	private:

		VkBuffer m_buffer;

		VkDevice m_device;

		void init(
			const VkBufferCreateInfo& createInfo,
			const VkMemoryPropertyFlags& properties
		);


	};
}
