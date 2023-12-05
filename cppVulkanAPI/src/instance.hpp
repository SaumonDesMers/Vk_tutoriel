#pragma once

#include "defines.hpp"

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace LIB_NAMESPACE
{

	struct ApplicationInfo: public VkApplicationInfo
	{
	};

	struct InstanceCreateInfo: public VkInstanceCreateInfo
	{
	};

	class Instance
	{

	public:

		Instance(const ft::InstanceCreateInfo & createInfo);

		~Instance();

		static bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers);

	private:

		VkInstance m_instance;

	};

}