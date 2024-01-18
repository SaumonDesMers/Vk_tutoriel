#pragma once

#include "vertex.hpp"

#include <cppVulkanAPI.hpp>

#include <glm/glm.hpp>

#include <stdexcept>
#include <memory>
#include <optional>


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

	std::unique_ptr<ft::Device> m_device;

	std::unique_ptr<ft::Swapchain> m_swapchain;

	std::unique_ptr<ft::Descriptor> m_descriptor;

	std::unique_ptr<ft::Command> m_command;
	std::vector<VkCommandBuffer> m_vkCommandBuffers;

	std::unique_ptr<ft::core::RenderPass> m_renderPass;
	std::unique_ptr<ft::Pipeline> m_graphicPipeline;
	std::vector<std::unique_ptr<ft::core::Framebuffer>> m_swapchainFramebuffers;

	std::unique_ptr<ft::core::Image> m_colorImage;
	std::unique_ptr<ft::core::DeviceMemory> m_colorImageMemory;
	std::unique_ptr<ft::core::ImageView> m_colorImageView;

	std::unique_ptr<ft::core::Image> m_depthImage;
	std::unique_ptr<ft::core::DeviceMemory> m_depthImageMemory;
	std::unique_ptr<ft::core::ImageView> m_depthImageView;

	std::vector<std::unique_ptr<ft::core::Semaphore>> m_imageAvailableSemaphores;
	std::vector<std::unique_ptr<ft::core::Semaphore>> m_renderFinishedSemaphores;
	std::vector<std::unique_ptr<ft::core::Fence>> m_inFlightFences;

	std::vector<Vertex> m_vertices;
	std::vector<uint32_t> m_indices;
	std::unique_ptr<ft::core::Buffer> m_vertexBuffer;
	std::unique_ptr<ft::core::DeviceMemory> m_vertexBufferMemory;
	std::unique_ptr<ft::core::Buffer> m_indexBuffer;
	std::unique_ptr<ft::core::DeviceMemory> m_indexBufferMemory;

	uint32_t m_mipLevels;
	std::unique_ptr<ft::core::Image> m_textureImage;
	std::unique_ptr<ft::core::DeviceMemory> m_textureImageMemory;
	std::unique_ptr<ft::core::ImageView> m_textureImageView;
	std::unique_ptr<ft::core::Sampler> m_textureSampler;

	std::vector<std::unique_ptr<ft::core::Buffer>> m_uniformBuffers;
	std::vector<std::unique_ptr<ft::core::DeviceMemory>> m_uniformBuffersMemory;

	bool m_framebufferResized = false;
	
	uint32_t m_currentFrame = 0;


	void init();

	void createDevice();
	void createSwapchain();
	void recreateSwapChain();
	void createRenderPass();
	void createDescriptor();
	void createGraphicsPipeline();
	void createFramebuffers();
	void createCommandPool();
	void createColorResources();
	void createDepthResources();
	void createTextureImage();
	void createTextureImageView();
	void createTextureSampler();
	void loadModel();
	void createVertexBuffer();
	void createIndexBuffer();
	void createUniformBuffers();
	void updateDescriptorSets();
	void createCommandBuffer();
	void createSyncObjects();

	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat findDepthFormat();
	bool hasStencilComponent(VkFormat format);

	void copyBufferToBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
	void generateMipmaps(VkImage image, VkFormat format, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	void drawFrame();
	void updateUniformBuffer(uint32_t currentImage);
};