#include "instance.hpp"
#include "defines.hpp"

#include <stdexcept>
#include <cstring>

namespace LIB_NAMESPACE
{

	Instance::Instance(const ft::InstanceCreateInfo& createInfo)
	{
		// Create instance
		VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create instance");
		}
	}

	Instance::~Instance()
	{
		vkDestroyInstance(m_instance, nullptr);
	}

	bool Instance::checkValidationLayerSupport(const std::vector<const char*>& validationLayers)
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers)
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (layerFound == false)
			{
				return false;
			}
		}

		return true;
	}


}