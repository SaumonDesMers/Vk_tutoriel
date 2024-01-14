#include "debug.hpp"

#include <iostream>
#include <stdexcept>

namespace LIB_NAMESPACE
{
	namespace core
	{
		DebugMessenger::DebugMessenger(
			Instance* pInstance,
			CreateInfo* pCreateInfo
		):
			m_pInstance(pInstance)
		{
			m_customUserData.userCallback = pCreateInfo->userCallback;
			m_customUserData.pUserData = pCreateInfo->pUserData;
			pCreateInfo->pUserData = &m_customUserData;

			pCreateInfo->pfnUserCallback = debugCallback;

			if (createDebugUtilsMessengerEXT(m_pInstance->getVk(), pCreateInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to set up debug messenger.");
			}
		}

		DebugMessenger::~DebugMessenger()
		{
			destroyDebugUtilsMessengerEXT(m_pInstance->getVk(), m_debugMessenger, nullptr);
		}

		VkResult DebugMessenger::createDebugUtilsMessengerEXT(
			const VkInstance& instance,
			const CreateInfo* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugUtilsMessengerEXT* pDebugMessenger
		)
		{
			auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
			if (func != nullptr)
			{
				return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
			}
			else
			{
				return VK_ERROR_EXTENSION_NOT_PRESENT;
			}
		}

		void DebugMessenger::destroyDebugUtilsMessengerEXT(
			const VkInstance& instance,
			const VkDebugUtilsMessengerEXT& debugMessenger,
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