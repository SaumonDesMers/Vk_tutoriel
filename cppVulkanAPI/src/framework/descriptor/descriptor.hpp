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

		typedef uint32_t ID;

		struct CreateInfo
		{
			std::vector<VkDescriptorSetLayoutBinding> bindings;
			uint32_t descriptorCount;
		};

		static inline ID maxID = 0;

		Descriptor(VkDevice device, const CreateInfo& createInfo);
		~Descriptor();

		VkDescriptorPool pool() { return m_pool; }
		VkDescriptorSetLayout layout() { return m_layout; }
		VkDescriptorSet set(uint32_t index) { return m_sets.at(index); }
		VkDescriptorSet* pSet(uint32_t index) { return &m_sets.at(index); }
	
	private:

		VkDescriptorPool m_pool;
		VkDescriptorSetLayout m_layout;
		std::vector<VkDescriptorSet> m_sets;

		VkDevice m_device;

		void createPool(const CreateInfo& createInfo);
		void createLayout(const CreateInfo& createInfo);
		void createSets(const CreateInfo& createInfo);

	};
}