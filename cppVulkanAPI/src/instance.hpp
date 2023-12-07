#pragma once

#include "defines.hpp"

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace LIB_NAMESPACE
{
	struct ApplicationInfo: public VkApplicationInfo
	{
		ApplicationInfo(): VkApplicationInfo()
		{
			sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		}
	};

	class Instance
	{

	public:


		struct CreateInfo: public VkInstanceCreateInfo
		{
			CreateInfo(): VkInstanceCreateInfo()
			{
				sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			}
		};

		Instance(const ft::Instance::CreateInfo & createInfo);

		~Instance();

		VkInstance getVk() const { return m_instance; }

		static bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers);

	private:

		VkInstance m_instance;

	};

}