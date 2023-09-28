#include "application.hpp"

/* Callback function for the debug messenger */
VKAPI_ATTR VkBool32 VKAPI_CALL Application::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

/*
 * This function is used to create a debug messenger object.
 * It is needed because the vkCreateDebugUtilsMessengerEXT function is an extension function which is not automatically loaded.
 * Therefore we have to manually load it using vkGetInstanceProcAddr.
 */
static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void Application::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	/* MessageSeverity can be:
		* VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
		* VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
		* VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
		* VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
		*/
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	/* MessageType can be:
		* VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
		* VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
		* VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
		*/
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}

void Application::setupDebugMessenger() {
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}
}