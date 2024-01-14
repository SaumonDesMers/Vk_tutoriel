#pragma once

#include "../defines.hpp"

#include <vulkan/vulkan.h>

namespace LIB_NAMESPACE
{
	namespace core
	{
		class DescriptorPool
		{

		public:

			DescriptorPool(VkDevice device, VkDescriptorPoolCreateInfo& createInfo);
			DescriptorPool(VkDevice device, uint32_t maxSets, uint32_t poolSizeCount, VkDescriptorPoolSize* poolSizes);
			~DescriptorPool();

			VkDescriptorPool getVk() const { return m_descriptorPool; }

		private:

			VkDescriptorPool m_descriptorPool;

			VkDevice m_device;
		};
	}
}