#pragma once

#include "defines.hpp"

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace LIB_NAMESPACE
{
	class Descriptor
	{

	public:

		struct CreateInfo
		{
			std::vector<VkDescriptorSetLayoutBinding> bindings;
			uint32_t descriptorCount;
		};

		VkDescriptorPool pool;
		VkDescriptorSetLayout layout;
		std::vector<VkDescriptorSet> sets;

		Descriptor(VkDevice device, const CreateInfo& createInfo);
		~Descriptor();
	
	private:

		VkDevice m_device;

		void createPool(const CreateInfo& createInfo);
		void createLayout(const CreateInfo& createInfo);
		void createSets(const CreateInfo& createInfo);

	};
}