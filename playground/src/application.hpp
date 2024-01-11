#pragma once

#include <cppVulkanAPI.hpp>

#include <glm/glm.hpp>

#include <stdexcept>
#include <memory>
#include <optional>

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

const int MAX_FRAMES_IN_FLIGHT = 2;

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

class Application
{

public:

	Application();

	~Application();

	void run();

private:

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	std::unique_ptr<ft::WindowManager> m_windowManager;
	std::unique_ptr<ft::Window> m_window;
	std::unique_ptr<ft::Instance> m_instance;
	std::unique_ptr<ft::DebugMessenger> m_debugMessenger;
	std::unique_ptr<ft::Window::Surface> m_surface;
	std::unique_ptr<ft::PhysicalDevice> m_physicalDevice;
	std::unique_ptr<ft::Device> m_device;
	std::unique_ptr<ft::Queue> m_graphicsQueue;
	std::unique_ptr<ft::Queue> m_presentQueue;
	std::unique_ptr<ft::Swapchain> m_swapchain;
	std::vector<std::unique_ptr<ft::ImageView>> m_swapchainImageViews;
	std::unique_ptr<ft::RenderPass> m_renderPass;
	std::unique_ptr<ft::DescriptorSetLayout> m_descriptorSetLayout;
	std::unique_ptr<ft::PipelineLayout> m_pipelineLayout;
	std::unique_ptr<ft::Pipeline> m_graphicPipeline;
	std::vector<std::unique_ptr<ft::Framebuffer>> m_swapchainFramebuffers;
	std::unique_ptr<ft::CommandPool> m_commandPool;
	std::vector<std::unique_ptr<ft::CommandBuffer>> m_commandBuffers;
	std::vector<std::unique_ptr<ft::Semaphore>> m_imageAvailableSemaphores;
	std::vector<std::unique_ptr<ft::Semaphore>> m_renderFinishedSemaphores;
	std::vector<std::unique_ptr<ft::Fence>> m_inFlightFences;
	std::unique_ptr<ft::Buffer> m_vertexBuffer;
	std::unique_ptr<ft::DeviceMemory> m_vertexBufferMemory;
	std::unique_ptr<ft::Buffer> m_indexBuffer;
	std::unique_ptr<ft::DeviceMemory> m_indexBufferMemory;
	std::vector<std::unique_ptr<ft::Buffer>> m_uniformBuffers;
	std::vector<std::unique_ptr<ft::DeviceMemory>> m_uniformBuffersMemory;
	std::unique_ptr<ft::DescriptorPool> m_descriptorPool;
	std::vector<std::unique_ptr<ft::DescriptorSet>> m_descriptorSets;
	bool m_framebufferResized = false;
	
	uint32_t m_currentFrame = 0;


	void init();

	void createWindowManager();
	void createWindow();
	void createInstance();
	void setupDebugMessenger();
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSwapChain();
	void recreateSwapChain();
	void createImageViews();
	void createRenderPass();
	void createDescriptorSetLayout();
	void createGraphicsPipeline();
	void createFramebuffers();
	void createCommandPool();
	void createTextureImage();
	void createVertexBuffer();
	void createIndexBuffer();
	void createUniformBuffers();
	void createDescriptorPool();
	void createDescriptorSets();
	void createCommandBuffer();
	void createSyncObjects();

	std::vector<const char*> getRequiredExtensions();
	void populateDebugMessengerCreateInfo(ft::DebugMessenger::CreateInfo& createInfo);
	bool isDeviceSuitable(const VkPhysicalDevice& physicalDevice);
	QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& physicalDevice);

	SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice& device);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);


	void recordCommandBuffer(const std::unique_ptr<ft::CommandBuffer>& commandBuffer, uint32_t imageIndex);
	void drawFrame();
	void updateUniformBuffer(uint32_t currentImage);
};