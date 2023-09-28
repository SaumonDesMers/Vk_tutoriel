#include "application.hpp"

/*
 * This function is used to destroy a debug messenger object.
 * It is needed because the vkDestroyDebugUtilsMessengerEXT function is an extension function which is not automatically loaded.
 * Therefore we have to manually load it using vkGetInstanceProcAddr.
 */
static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void Application::cleanup() {
	vkDestroySemaphore(this->device, this->renderFinishedSemaphore, nullptr);
	vkDestroySemaphore(this->device, this->imageAvailableSemaphore, nullptr);
	vkDestroyFence(this->device, this->inFlightFence, nullptr);

	vkDestroyCommandPool(this->device, this->commandPool, nullptr);

	for (auto framebuffer : this->swapChainFramebuffers) {
		vkDestroyFramebuffer(this->device, framebuffer, nullptr);
	}

	vkDestroyPipeline(this->device, this->graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(this->device, this->pipelineLayout, nullptr);
	vkDestroyRenderPass(this->device, this->renderPass, nullptr);

	for (auto imageView : this->swapChainImageViews) {
		vkDestroyImageView(this->device, imageView, nullptr);
	}

	vkDestroySwapchainKHR(this->device, this->swapChain, nullptr);
	vkDestroyDevice(this->device, nullptr);

	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(this->instance, this->debugMessenger, nullptr);
	}

	vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
	vkDestroyInstance(this->instance, nullptr);

	glfwDestroyWindow(this->window);

	glfwTerminate();
}