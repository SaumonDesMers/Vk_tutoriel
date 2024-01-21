#pragma once

#include "game_object.hpp"

#include <cppVulkanAPI.hpp>

#include <glm/glm.hpp>

#include <stdexcept>
#include <memory>
#include <optional>

const int MAX_FRAMES_IN_FLIGHT = 3;

struct ViewProj_UBO {
	glm::mat4 view;
	glm::mat4 proj;
};

struct ModelMatrix_push_constant
{
	glm::mat4 model;
};

struct Timer
{
	std::chrono::_V2::system_clock::time_point start;

	Timer()
	{
		start = std::chrono::high_resolution_clock::now();
	}

	float getElapsedTime()
	{
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration<float, std::chrono::seconds::period>(end - start);
		return duration.count();
	}
};

class RenderAPI
{

public:

	RenderAPI();

	~RenderAPI();

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

	std::unique_ptr<ft::Pipeline> m_graphicPipeline;

	std::unique_ptr<ft::Image> m_colorImage;
	std::unique_ptr<ft::Image> m_depthImage;

	std::vector<std::unique_ptr<ft::core::Semaphore>> m_imageAvailableSemaphores;
	std::vector<std::unique_ptr<ft::core::Semaphore>> m_renderFinishedSemaphores;
	std::vector<std::unique_ptr<ft::core::Semaphore>> m_swapchainUpdatedSemaphores;
	std::vector<std::unique_ptr<ft::core::Fence>> m_inFlightFences;

	ft::Mesh::ID m_maxMeshID = 0;
	std::map<ft::Mesh::ID, std::unique_ptr<ft::Mesh>> m_meshAtlas;

	uint32_t m_mipLevels;
	std::unique_ptr<ft::Texture> m_texture;
	std::unique_ptr<ft::core::Sampler> m_textureSampler;

	std::vector<std::unique_ptr<ft::Buffer>> m_uniformBuffers;

	bool m_framebufferResized = false;
	
	uint32_t m_currentFrame = 0;

	Timer m_timer;

	std::vector<GameObject> m_gameObjects;


	void init();

	void createDevice();
	void createSwapchain();
	void recreateSwapChain();
	void createDescriptor();
	void createGraphicsPipeline();
	void createCommandPool();
	void createColorResources();
	void createDepthResources();
	void createTextureImage();
	void createTextureSampler();
	ft::Mesh::ID loadModel(const std::string& filename);
	void createUniformBuffers();
	void updateDescriptorSets();
	void createCommandBuffer();
	void createSyncObjects();

	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat findDepthFormat();
	bool hasStencilComponent(VkFormat format);

	void generateMipmaps(VkImage image, VkFormat format, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);


	// function to start recording a command buffer
	void startDraw();

	// function to start a render pass
	void startRendering();

	// function to do the actual drawing
	void draw();
	void updateUniformBuffer(uint32_t currentImage);

	// function to end a render pass
	void endRendering();

	// function to end recording a command buffer
	void endDraw();
	void copyRenderedImageToSwapchainImage(uint32_t swapchainImageIndex);
};