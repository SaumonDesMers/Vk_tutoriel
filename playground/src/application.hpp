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

	std::unique_ptr<ft::WindowManager> m_windowManager;
	std::unique_ptr<ft::Window> m_window;
	std::unique_ptr<ft::Instance> m_instance;
	std::unique_ptr<ft::DebugMessenger> m_debugMessenger;
	std::unique_ptr<ft::Window::Surface> m_surface;
	std::unique_ptr<ft::PhysicalDevice> m_physicalDevice;
	std::unique_ptr<ft::Device> m_device;
	std::unique_ptr<ft::Queue> m_graphicsQueue;
	std::unique_ptr<ft::Queue> m_presentQueue;

	void init();

	void createWindowManager();
	void createWindow();
	void createInstance();
	void setupDebugMessenger();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSurface();

	std::vector<const char*> getRequiredExtensions();
	void populateDebugMessengerCreateInfo(ft::DebugMessenger::CreateInfo& createInfo);
	bool isDeviceSuitable(const VkPhysicalDevice& physicalDevice);
	QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& physicalDevice);
};