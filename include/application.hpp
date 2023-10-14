#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
#include <array>
#include <memory>

#include "vertex.hpp"
#include "object.hpp"
#include "camera.hpp"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::string MODEL_PATH = "models/42.obj";
const std::string TEXTURE_PATH = "textures/texture.ppm";

const int MAX_FRAMES_IN_FLIGHT = 2;

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

	/* Callback function for keyboard events */
	typedef void (Application::*key_callback)(int, int, int, int);
	key_callback key_callbacks[GLFW_KEY_LAST];

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

	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;

	Camera camera = Camera(ft::vec3(0.0f, 0.0f, 7.0f), ft::vec3(0.0f, 0.0f, 0.0f), ft::vec3(0.0f, 1.0f, 0.0f));
	std::unique_ptr<Object> object;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;

	bool framebufferResized = false;

	uint32_t currentFrame = 0;

	void initVulkan() {
		this->loadModel();
		
		this->createInstance();
		this->setupDebugMessenger();
		this->createSurface();
		this->pickPhysicalDevice();
		this->createLogicalDevice();
		this->createSwapChain();
		this->createImageViews();
		this->createRenderPass();
		this->createDescriptorSetLayout();
		this->createGraphicsPipeline();
		this->createCommandPool();
		this->createDepthResources();
		this->createFramebuffers();
		this->createTextureImage();
		this->createTextureImageView();
		this->createTextureSampler();
		this->createVertexBuffer();
		this->createIndexBuffer();
		this->createUniformBuffers();
		this->createDescriptorPool();
		this->createDescriptorSets();
		this->createCommandBuffers();
		this->createSyncObjects();
	}

	/* window.cpp */
	void initWindow();
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
	void createSurface();

	/* key_callback.cpp */
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void mapKeyCallback();
	void escape(int key, int scancode, int action, int mods);
	void arrowUp(int key, int scancode, int action, int mods);
	void arrowDown(int key, int scancode, int action, int mods);
	void arrowLeft(int key, int scancode, int action, int mods);
	void arrowRight(int key, int scancode, int action, int mods);
	void key_a(int key, int scancode, int action, int mods);
	void key_d(int key, int scancode, int action, int mods);
	void key_w(int key, int scancode, int action, int mods);
	void key_s(int key, int scancode, int action, int mods);

	/* mouse_callback.cpp */
	static void scrollCallback(GLFWwindow* window, double xpos, double ypos);

	/* main_loop.cpp */
	void mainLoop();

	/* clean_up.cpp */
	void cleanup();
	void cleanupSwapChain();

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
	void recreateSwapChain();
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	/* image_views.cpp */
	void createImageViews();
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

	/* render_pass.cpp */
	void createRenderPass();

	/* descriptor.cpp */
	void createDescriptorSetLayout();
	void createDescriptorPool();
	void createDescriptorSets();

	/* graphics_pipeline.cpp */
	void createGraphicsPipeline();
	static std::vector<char> readFile(const std::string& filename);
	VkShaderModule createShaderModule(const std::vector<char>& code);

	/* texture.cpp */
	void createTextureImage();
	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void createTextureImageView();
	void createTextureSampler();

	/* depth.cpp */
	void createDepthResources();
	VkFormat findDepthFormat();
	bool hasStencilComponent(VkFormat format);
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	/* buffer.cpp */
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	/* frame_buffer.cpp */
	void createFramebuffers();

	/* vertex_buffer.cpp */
	void createVertexBuffer();

	/* index.cpp */
	void createIndexBuffer();

	/* uniform_buffer.cpp */
	void createUniformBuffers();

	/* command.cpp */
	void createCommandPool();
	void createCommandBuffers();
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);

	/* sync_objects.cpp */
	void createSyncObjects();

	/* draw.cpp */
	void drawFrame();
	void updateUniformBuffer(uint32_t currentImage);

	/* model_loading.cpp */
	void loadModel();

};

#endif // APPLICATION_HPP