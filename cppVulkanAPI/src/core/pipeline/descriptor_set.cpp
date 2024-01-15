#include "descriptor_set.hpp"

#include <stdexcept>

namespace LIB_NAMESPACE
{
	namespace core
	{
		DescriptorSet::DescriptorSet(VkDevice device, VkDescriptorSetAllocateInfo& allocInfo)
			: m_device(device)
		{
			if (vkAllocateDescriptorSets(m_device, &allocInfo, &m_descriptorSet) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to allocate descriptor set.");
			}
		}

		DescriptorSet::~DescriptorSet()
		{
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
}