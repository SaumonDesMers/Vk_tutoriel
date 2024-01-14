#pragma once

#include "../defines.hpp"

#include <vulkan/vulkan.h>

namespace LIB_NAMESPACE
{
	namespace core
	{
		class Sampler
		{

		public:

			struct CreateInfo: public VkSamplerCreateInfo
			{
				CreateInfo(): VkSamplerCreateInfo()
				{
					this->sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
				}
			};

			Sampler(VkDevice device, const VkSamplerCreateInfo& createInfo);
			~Sampler();

			VkSampler getVk() const { return m_sampler; }

		private:

			VkSampler m_sampler;

			VkDevice m_device;

		};
	}
}