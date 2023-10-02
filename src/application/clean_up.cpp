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

void Application::cleanupSwapChain() {
	for (size_t i = 0; i < swapChainFramebuffers.size(); i++) {
		vkDestroyFramebuffer(this->device, this->swapChainFramebuffers[i], nullptr);
	}

	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		vkDestroyImageView(this->device, this->swapChainImageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(this->device, this->swapChain, nullptr);
}

void Application::cleanup() {
	this->cleanupSwapChain();

	vkDestroyImage(device, textureImage, nullptr);
    vkFreeMemory(device, textureImageMemory, nullptr);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(device, uniformBuffers[i], nullptr);
        vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
    }

	vkDestroyDescriptorPool(device, descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

	vkDestroyBuffer(this->device, this->indexBuffer, nullptr);
    vkFreeMemory(this->device, this->indexBufferMemory, nullptr);

	vkDestroyBuffer(this->device, this->vertexBuffer, nullptr);
	vkFreeMemory(this->device, this->vertexBufferMemory, nullptr);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(this->device, this->renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(this->device, this->imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(this->device, this->inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(this->device, this->commandPool, nullptr);

	vkDestroyPipeline(this->device, this->graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(this->device, this->pipelineLayout, nullptr);
	vkDestroyRenderPass(this->device, this->renderPass, nullptr);

	vkDestroyDevice(this->device, nullptr);

	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(this->instance, this->debugMessenger, nullptr);
	}

	vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
	vkDestroyInstance(this->instance, nullptr);

	glfwDestroyWindow(this->window);

	glfwTerminate();
}