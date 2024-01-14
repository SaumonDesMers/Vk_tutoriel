#include "sampler.hpp"

#include <stdexcept>

namespace LIB_NAMESPACE
{
	namespace core
	{
		Sampler::Sampler(
			VkDevice device,
			const VkSamplerCreateInfo& createInfo
		):
			m_device(device)
		{
			if (vkCreateSampler(device, &createInfo, nullptr, &m_sampler) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create sampler.");
			}
		}

		Sampler::~Sampler()
		{
			vkDestroySampler(m_device, m_sampler, nullptr);
		}
	}
}