#pragma once

#include "defines.hpp"
#include "instance.hpp"

#include <vulkan/vulkan.h>

#include <functional>

namespace LIB_NAMESPACE
{

	class DebugMessenger
	{

	public:

		using Callback = std::function<void(
			VkDebugUtilsMessageSeverityFlagBitsEXT,
			VkDebugUtilsMessageTypeFlagsEXT,
			const VkDebugUtilsMessengerCallbackDataEXT*,
			void*
		)>;

		struct CreateInfo: private VkDebugUtilsMessengerCreateInfoEXT
		{
			friend class DebugMessenger;

			using VkDebugUtilsMessengerCreateInfoEXT::pNext;
			using VkDebugUtilsMessengerCreateInfoEXT::flags;
			using VkDebugUtilsMessengerCreateInfoEXT::messageSeverity;
			using VkDebugUtilsMessengerCreateInfoEXT::messageType;
			using VkDebugUtilsMessengerCreateInfoEXT::pUserData;

			Callback userCallback = nullptr;

			CreateInfo(): VkDebugUtilsMessengerCreateInfoEXT()
			{
				sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
				pfnUserCallback = debugCallback;
			}
		};

		DebugMessenger(
			Instance* instance,
			CreateInfo* pCreateInfo
		);

		~DebugMessenger();

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData
		);
	
	private:

		struct CustomUserData
		{
			Callback userCallback = nullptr;
			void * pUserData = nullptr;
		} m_customUserData;

		VkDebugUtilsMessengerEXT m_debugMessenger;
		Instance *m_pInstance;

		static VkResult createDebugUtilsMessengerEXT(
			const VkInstance& instance,
			const CreateInfo* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger
		);

		static void destroyDebugUtilsMessengerEXT(
			const VkInstance& instance,
			const VkDebugUtilsMessengerEXT& debugMessenger,
			const VkAllocationCallbacks* pAllocator
		);

		static void defaultDebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData
		);
	};

}