#pragma once

#include "defines.hpp"
#include "instance.hpp"
#include "queue.hpp"

#include <vulkan/vulkan.h>

#include <vector>

namespace LIB_NAMESPACE
{
	namespace core
	{
		class PhysicalDevice
		{

		public:

			struct Properties: public VkPhysicalDeviceProperties
			{
				Properties() : VkPhysicalDeviceProperties() {}

				Properties(const VkPhysicalDevice& physicalDevice)
				{
					vkGetPhysicalDeviceProperties(physicalDevice, this);
				}
			};

			struct Features: public VkPhysicalDeviceFeatures
			{
				Features() : VkPhysicalDeviceFeatures() {}

				Features(const VkPhysicalDevice& physicalDevice)
					: VkPhysicalDeviceFeatures()
				{
					vkGetPhysicalDeviceFeatures(physicalDevice, this);
				}
			};

			PhysicalDevice(VkPhysicalDevice physicalDevice);

			~PhysicalDevice();

			VkPhysicalDevice getVk() const { return m_physicalDevice; }

			static std::vector<VkQueueFamilyProperties> getQueueFamilyProperties(VkPhysicalDevice physicalDevice);

			static bool getSurfaceSupport(
				VkPhysicalDevice physicalDevice,
				uint32_t queueFamilyIndex,
				VkSurfaceKHR surface
			);

			static VkSurfaceCapabilitiesKHR getSurfaceCapabilities(
				VkPhysicalDevice physicalDevice,
				VkSurfaceKHR surface
			);

			static std::vector<VkSurfaceFormatKHR> getSurfaceFormats(
				VkPhysicalDevice physicalDevice,
				VkSurfaceKHR surface
			);

			static std::vector<VkPresentModeKHR> getSurfacePresentModes(
				VkPhysicalDevice physicalDevice,
				VkSurfaceKHR surface
			);

			static bool checkExtensionSupport(
				const VkPhysicalDevice& physicalDevice,
				const std::vector<const char*>& extensions
			);

			void getProperties(VkPhysicalDeviceProperties* properties) const;

		private:

			VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

		};
	}
}