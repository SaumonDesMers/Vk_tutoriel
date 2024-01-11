#pragma once

#include "../defines.hpp"

#include <vulkan/vulkan.h>

namespace LIB_NAMESPACE
{
	class DescriptorSet
	{

	public:

		DescriptorSet(VkDevice device, VkDescriptorSetAllocateInfo& allocInfo);
		~DescriptorSet();

		VkDescriptorSet getVk() const { return m_descriptorSet; }
		VkDescriptorSet* getVkPtr() { return &m_descriptorSet; }

		void update(
			uint32_t descriptorWriteCount,
			VkWriteDescriptorSet* descriptorWrites,
			uint32_t descriptorCopyCount = 0,
			VkCopyDescriptorSet* descriptorCopies = nullptr
		);

	private:

		VkDescriptorSet m_descriptorSet;

		VkDevice m_device;
		VkDescriptorPool m_descriptorPool;
	};
}