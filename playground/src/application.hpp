#pragma once

#include <cppVulkanAPI.hpp>

#include <stdexcept>
#include <memory>
#include <optional>

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

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
	std::unique_ptr<ft::PipelineLayout> m_pipelineLayout;
	std::unique_ptr<ft::Pipeline> m_graphicPipeline;


	void init();

	void createWindowManager();
	void createWindow();
	void createInstance();
	void setupDebugMessenger();
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSwapChain();
	void createImageViews();
	void createRenderPass();
	void createGraphicsPipeline();

	std::vector<const char*> getRequiredExtensions();
	void populateDebugMessengerCreateInfo(ft::DebugMessenger::CreateInfo& createInfo);
	bool isDeviceSuitable(const VkPhysicalDevice& physicalDevice);
	QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& physicalDevice);

	SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice& device);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
};