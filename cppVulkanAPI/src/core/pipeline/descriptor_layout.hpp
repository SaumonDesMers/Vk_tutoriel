#pragma once

#include "../defines.hpp"

#include <vulkan/vulkan.h>

namespace LIB_NAMESPACE
{
	namespace core
	{
		class DescriptorSetLayout
		{
		
		public:

			DescriptorSetLayout(VkDevice device, VkDescriptorSetLayoutCreateInfo& createInfo);
			~DescriptorSetLayout();

			VkDescriptorSetLayout getVk() const { return m_layout; }
			VkDescriptorSetLayout& getVkRef() { return m_layout; }

		private:

			VkDescriptorSetLayout m_layout;

			VkDevice m_device;

		};
	}
}