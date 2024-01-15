#include "debug.hpp"

#include <iostream>
#include <stdexcept>

namespace LIB_NAMESPACE
{
	namespace core
	{
		DebugMessenger::DebugMessenger(
			VkInstance instance,
			CreateInfo& createInfo
		):
			m_instance(instance)
		{
			m_customUserData.userCallback = createInfo.userCallback;
			m_customUserData.pUserData = createInfo.pUserData;
			createInfo.pUserData = &m_customUserData;

			createInfo.pfnUserCallback = debugCallback;

			if (createDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to set up debug messenger.");
			}
		}

		DebugMessenger::~DebugMessenger()
		{
			destroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
		}

		VkResult DebugMessenger::createDebugUtilsMessengerEXT(
			VkInstance instance,
			const CreateInfo* createInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger
		)
		{
			auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
			if (func != nullptr)
			{
				return func(instance, createInfo, pAllocator, pDebugMessenger);
			}
			else
			{
				return VK_ERROR_EXTENSION_NOT_PRESENT;
			}
		}

		void DebugMessenger::destroyDebugUtilsMessengerEXT(
			VkInstance instance,
			VkDebugUtilsMessengerEXT debugMessenger,
			const VkAllocationCallbacks* pAllocator
		)
		{
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
			if (func != nullptr)
			{
				func(instance, debugMessenger, pAllocator);
			}
		}

		VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessenger::debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData
		)
		{
			auto customUserData = reinterpret_cast<CustomUserData*>(pUserData);

			if (customUserData && customUserData->userCallback != nullptr)
			{
				customUserData->userCallback(messageSeverity, messageType, pCallbackData, customUserData->pUserData);
			}
			else
			{
				defaultDebugCallback(messageSeverity, messageType, pCallbackData, customUserData ? customUserData->pUserData : nullptr);
			}

			return VK_FALSE;
		}

		void DebugMessenger::defaultDebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT,
			VkDebugUtilsMessageTypeFlagsEXT,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void*
		)
		{
			std::cerr << pCallbackData->pMessage << std::endl;
		}
	}
}