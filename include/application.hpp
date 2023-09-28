#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <set>
#include <optional>
#include <cstdint>
#include <limits>
#include <algorithm>
#include <fstream>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

/* List of validation layers to use */
const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

/* List of device extensions to use */
const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME /* Swapchain requirement, should be available if presentation queue is available */
};

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class Application {
public:
	void run() {
		this->initWindow();
		this->initVulkan();
		this->mainLoop();
		this->cleanup();
	}

private:
	GLFWwindow* window;

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkSurfaceKHR surface;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;

	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	VkFence inFlightFence;

	void initVulkan() {
		this->createInstance();
		this->setupDebugMessenger();
		this->createSurface();
		this->pickPhysicalDevice();
		this->createLogicalDevice();
		this->createSwapChain();
		this->createImageViews();
		this->createRenderPass();
		this->createGraphicsPipeline();
		this->createFramebuffers();
		this->createCommandPool();
		this->createCommandBuffer();
		this->createSyncObjects();
	}

	/* window.cpp */
	void initWindow();
	void createSurface();

	/* main_loop.cpp */
	void mainLoop();

	/* clean_up.cpp */
	void cleanup();

	/* instance.cpp */
	void createInstance();
	std::vector<const char*> getGlfwRequiredExtensions();
	bool checkValidationLayerSupport();

	/* debug.cpp */
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void setupDebugMessenger();
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

	/* physical_device.cpp */
	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

	/* logical_device.cpp */
	void createLogicalDevice();

	/* swap_chain.cpp */
	void createSwapChain();
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	/* image_views.cpp */
	void createImageViews();

	/* render_pass.cpp */
	void createRenderPass();

	/* graphics_pipeline.cpp */
	void createGraphicsPipeline();
	static std::vector<char> readFile(const std::string& filename);
	VkShaderModule createShaderModule(const std::vector<char>& code);

	/* frame_buffer.cpp */
	void createFramebuffers();

	/* command.cpp */
	void createCommandPool();
	void createCommandBuffer();
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

	/* sync_objects.cpp */
	void createSyncObjects();

	/* draw.cpp */
	void drawFrame();

};

#endif // APPLICATION_HPP