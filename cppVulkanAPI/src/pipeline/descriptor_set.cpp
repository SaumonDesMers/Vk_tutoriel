#include "descriptor_set.hpp"

#include <stdexcept>

namespace LIB_NAMESPACE
{
	DescriptorSet::DescriptorSet(VkDevice device, VkDescriptorSetAllocateInfo& allocInfo)
		: m_device(device), m_descriptorPool(allocInfo.descriptorPool)
	{
		if (vkAllocateDescriptorSets(m_device, &allocInfo, &m_descriptorSet) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate descriptor set.");
		}
	}

	DescriptorSet::~DescriptorSet()
	{
		// vkFreeDescriptorSets(m_device, m_descriptorPool, 1, &m_descriptorSet);
	}

	void DescriptorSet::update(
		uint32_t descriptorWriteCount,
		VkWriteDescriptorSet* descriptorWrites,
		uint32_t descriptorCopyCount,
		VkCopyDescriptorSet* descriptorCopies
	)
	{
		vkUpdateDescriptorSets(m_device, descriptorWriteCount, descriptorWrites, descriptorCopyCount, descriptorCopies);
	}
}