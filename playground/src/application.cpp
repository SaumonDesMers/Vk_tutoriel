#include "application.hpp"
#include "logger.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vulkan/vulkan.h>

#include <set>
#include <chrono>

Application::Application()
{
	
}

Application::~Application()
{
	m_device->device->waitIdle();

	for (auto& vkCommandBuffer : m_vkCommandBuffers)
	{
		m_command->freeCommandBuffer(vkCommandBuffer);
	}
}

void Application::run()
{
	init();

	while (m_device->window->shouldClose() == false)
	{
		m_device->windowManager->pollEvents();
		startDraw();
		startRendering();
		draw();
		endRendering();
		endDraw();
	}
}

void Application::init()
{
	createDevice();
	createSwapchain();
	createDescriptor();
	createColorResources();
	createDepthResources();
	createGraphicsPipeline();
	createCommandPool();
	createTextureImage();
	createTextureSampler();
	loadModel();
	createUniformBuffers();
	updateDescriptorSets();
	createCommandBuffer();
	createSyncObjects();

	FT_INFO("Application initialized");
}


void Application::createDevice()
{
	m_device = std::make_unique<ft::Device>();
}

void Application::createSwapchain()
{
	ft::Swapchain::CreateInfo swapchainInfo = {};
	swapchainInfo.surface = m_device->surface->getVk();
	swapchainInfo.supportDetails = m_device->querySwapChainSupport(m_device->physicalDevice->getVk());
	
	int width, height;
	m_device->window->getFramebufferSize(&width, &height);
	swapchainInfo.frameBufferExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

	swapchainInfo.queueFamilyIndices = m_device->findQueueFamilies(m_device->physicalDevice->getVk());

	swapchainInfo.oldSwapchain = VK_NULL_HANDLE;

	m_swapchain = std::make_unique<ft::Swapchain>(m_device->device->getVk(), swapchainInfo);
}

void Application::recreateSwapChain()
{
	int width = 0, height = 0;
    m_device->window->getFramebufferSize(&width, &height);
    while (width == 0 || height == 0)
	{
        m_device->window->getFramebufferSize(&width, &height);
        m_device->windowManager->waitEvents();
    }

	m_device->device->waitIdle();

	m_colorImage.reset();
	m_depthImage.reset();
	m_swapchain.reset();


	createSwapchain();
	createColorResources();
	createDepthResources();

}

void Application::createDescriptor()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;


	ft::Descriptor::CreateInfo descriptorInfo{};
	descriptorInfo.bindings = { uboLayoutBinding, samplerLayoutBinding };
	descriptorInfo.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	m_descriptor = std::make_unique<ft::Descriptor>(m_device->device->getVk(), descriptorInfo);
}

void Application::createGraphicsPipeline()
{
	ft::Pipeline::CreateInfo pipelineInfo = {};

	pipelineInfo.vertexShaderPath = "playground/shaders/simple_shader.vert.spv";
	pipelineInfo.fragmentShaderPath = "playground/shaders/simple_shader.frag.spv";

	pipelineInfo.msaaSamples = m_device->msaaSamples;

	pipelineInfo.descriptorSetLayouts = { m_descriptor->layout };

	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(ModelMatrix_push_constant);
	pipelineInfo.pushConstantRanges = { pushConstantRange };

	VkFormat colorAttachementFormat = m_colorImage->format();
	VkPipelineRenderingCreateInfo renderingInfo = {};
	renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachmentFormats = &colorAttachementFormat;
	renderingInfo.depthAttachmentFormat = findDepthFormat();

	pipelineInfo.pNext = &renderingInfo;

	m_graphicPipeline = std::make_unique<ft::Pipeline>(m_device->device->getVk(), pipelineInfo);
}

void Application::createCommandPool()
{
	ft::Queue::FamilyIndices queueFamilyIndices = m_device->findQueueFamilies(m_device->physicalDevice->getVk());

	ft::Command::CreateInfo commandInfo{};
	commandInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	commandInfo.queue = m_device->graphicsQueue->getVk();
	commandInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	m_command = std::make_unique<ft::Command>(m_device->device->getVk(), commandInfo);
}

void Application::createColorResources()
{
	m_colorImage = std::make_unique<ft::Image>(ft::Image::createColorImage(
		m_device->device->getVk(),
		m_device->physicalDevice->getVk(),
		m_swapchain->swapchain->getExtent(),
		m_swapchain->swapchain->getImageFormat(),
		m_device->msaaSamples
	));

}

void Application::createDepthResources()
{
	VkFormat depthFormat = findDepthFormat();

	m_depthImage = std::make_unique<ft::Image>(ft::Image::createDepthImage(
		m_device->device->getVk(),
		m_device->physicalDevice->getVk(),
		m_swapchain->swapchain->getExtent(),
		depthFormat,
		m_device->msaaSamples
	));
}

void Application::createTextureImage()
{
	m_texture = std::make_unique<ft::Texture>(
		m_device->device->getVk(),
		m_device->physicalDevice->getVk(),
		*m_command.get(),
		"playground/textures/viking_room.ppm"
	);

	generateMipmaps(
		m_texture->image().image(),
		VK_FORMAT_R8G8B8A8_SRGB,
		m_texture->width(),
		m_texture->height(),
		m_texture->image().mipLevels()
	);

}

void Application::createTextureSampler()
{
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;

	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	VkPhysicalDeviceProperties properties{};
	m_device->physicalDevice->getProperties(&properties);

	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = static_cast<float>(m_mipLevels);

	m_textureSampler = std::make_unique<ft::core::Sampler>(m_device->device->getVk(), samplerInfo);
}

void Application::loadModel()
{
	std::string modelPath = "playground/models/viking_room.obj";

	ft::Mesh::CreateInfo meshInfo = {};

	ft::Mesh::readObjFile(modelPath, meshInfo.vertices, meshInfo.indices);

	m_mesh = std::make_unique<ft::Mesh>(
		m_device->device->getVk(),
		m_device->physicalDevice->getVk(),
		*m_command.get(),
		meshInfo
	);
}

void Application::createUniformBuffers()
{
    VkDeviceSize bufferSize = sizeof(ViewProj_UBO);

    m_uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = bufferSize;
		bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		m_uniformBuffers[i] = std::make_unique<ft::Buffer>(
			m_device->device->getVk(),
			m_device->physicalDevice->getVk(),
			bufferInfo,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);

		m_uniformBuffers[i]->map();
    }
}

void Application::updateDescriptorSets()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = m_uniformBuffers[i]->buffer();
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(ViewProj_UBO);

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = m_texture->image().view();
		imageInfo.sampler = m_textureSampler->getVk();

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = m_descriptor->sets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = m_descriptor->sets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(
			m_device->device->getVk(),
			static_cast<uint32_t>(descriptorWrites.size()),
			descriptorWrites.data(),
			0, nullptr
		);
	}

}

void Application::createCommandBuffer()
{
	m_vkCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		m_vkCommandBuffers[i] = m_command->allocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	}
}

void Application::createSyncObjects()
{
	m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_swapchainUpdatedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	ft::core::Semaphore::CreateInfo semaphoreInfo{};

	ft::core::Fence::CreateInfo fenceInfo{};
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		m_imageAvailableSemaphores[i] = std::make_unique<ft::core::Semaphore>(m_device->device->getVk(), semaphoreInfo);
		m_renderFinishedSemaphores[i] = std::make_unique<ft::core::Semaphore>(m_device->device->getVk(), semaphoreInfo);
		m_swapchainUpdatedSemaphores[i] = std::make_unique<ft::core::Semaphore>(m_device->device->getVk(), semaphoreInfo);
		m_inFlightFences[i] = std::make_unique<ft::core::Fence>(m_device->device->getVk(), fenceInfo);
	}

}


VkFormat Application::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (const auto& format : candidates)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(m_device->physicalDevice->getVk(), format, &props);

		if (
			tiling == VK_IMAGE_TILING_LINEAR &&
			(props.linearTilingFeatures & features) == features
		)
		{
			return format;
		}
		else if (
			tiling == VK_IMAGE_TILING_OPTIMAL &&
			(props.optimalTilingFeatures & features) == features
		)
		{
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format.");
}

VkFormat Application::findDepthFormat() {
    static VkFormat depthFormat = findSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
	return depthFormat;
}

bool Application::hasStencilComponent(VkFormat format) {
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}


void Application::generateMipmaps(VkImage image, VkFormat format, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
{
	// Check if image format supports linear blitting
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(m_device->physicalDevice->getVk(), format, &formatProperties);

	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
	{
		throw std::runtime_error("texture image format does not support linear blitting.");
	}

	VkCommandBuffer commandBuffer = m_command->beginSingleTimeCommands();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image = image;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = 1;

	int32_t mipWidth = texWidth;
	int32_t mipHeight = texHeight;

	for (uint32_t i = 1; i < mipLevels; i++)
	{
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(
			commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		VkImageBlit blit{};
		blit.srcOffsets[0] = {0, 0, 0};
		blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;

		blit.dstOffsets[0] = {0, 0, 0};
		blit.dstOffsets[1] = {
			mipWidth > 1 ? mipWidth / 2 : 1,
			mipHeight > 1 ? mipHeight / 2 : 1,
			1
		};
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		vkCmdBlitImage(
			commandBuffer,
			image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &blit,
			VK_FILTER_LINEAR
		);

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(
			commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		if (mipWidth > 1) mipWidth /= 2;
		if (mipHeight > 1) mipHeight /= 2;
	}

	barrier.subresourceRange.baseMipLevel = mipLevels - 1;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	m_command->endSingleTimeCommands(commandBuffer);
}


void Application::updateUniformBuffer(uint32_t currentImage)
{
	ViewProj_UBO ubo{};
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), m_swapchain->swapchain->getExtent().width / (float) m_swapchain->swapchain->getExtent().height, 0.1f, 10.0f);

	ubo.proj[1][1] *= -1;

	m_uniformBuffers[currentImage]->write(&ubo, sizeof(ubo));
}

void Application::copyRenderedImageToSwapchainImage(uint32_t swapchainImageIndex)
{

	// First, we need to transition the swap chain image to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL layout, so we can copy the offscreen image to it.
	m_command->transitionImageLayout(
		m_swapchain->swapchain->getImage(swapchainImageIndex),
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_ASPECT_COLOR_BIT,
		1,
		0,
		0,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT
	);

	// The offscreen image is allready in VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL layout, so we can copy it to the swap chain image.
	VkCommandBuffer commandBuffer = m_command->beginSingleTimeCommands();

	// copy with blit
	VkImageBlit blit{};
	blit.srcOffsets[0] = {0, 0, 0};
	blit.srcOffsets[1] = {
		static_cast<int32_t>(m_colorImage->width()),
		static_cast<int32_t>(m_colorImage->height()),
		1
	};
	blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blit.srcSubresource.mipLevel = 0;
	blit.srcSubresource.baseArrayLayer = 0;
	blit.srcSubresource.layerCount = 1;

	blit.dstOffsets[0] = {0, 0, 0};
	blit.dstOffsets[1] = {
		static_cast<int32_t>(m_swapchain->swapchain->getExtent().width),
		static_cast<int32_t>(m_swapchain->swapchain->getExtent().height),
		1
	};
	blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blit.dstSubresource.mipLevel = 0;
	blit.dstSubresource.baseArrayLayer = 0;
	blit.dstSubresource.layerCount = 1;

	vkCmdBlitImage(
		commandBuffer,
		m_colorImage->image(),
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		m_swapchain->swapchain->getImage(swapchainImageIndex),
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&blit,
		VK_FILTER_LINEAR
	);

	VkResult result = vkEndCommandBuffer(commandBuffer);
	if (result != VK_SUCCESS)
	{
		TROW("Failed to record command buffer", result);
	}

	VkSubmitInfo copyImageInfo = {};
	copyImageInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	copyImageInfo.commandBufferCount = 1;
	copyImageInfo.pCommandBuffers = &commandBuffer;

	VkSemaphore copyImageWaitSemaphores[] = {
		m_imageAvailableSemaphores[m_currentFrame]->getVk(),
		m_renderFinishedSemaphores[m_currentFrame]->getVk()
	};
	VkPipelineStageFlags copyImageWaitStages[] = {
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
	};
	copyImageInfo.waitSemaphoreCount = 2;
	copyImageInfo.pWaitSemaphores = copyImageWaitSemaphores;
	copyImageInfo.pWaitDstStageMask = copyImageWaitStages;

	VkSemaphore copyImageSignalSemaphores[] = {
		m_swapchainUpdatedSemaphores[m_currentFrame]->getVk()
	};
	copyImageInfo.signalSemaphoreCount = 1;
	copyImageInfo.pSignalSemaphores = copyImageSignalSemaphores;

	m_command->submit(1, &copyImageInfo, VK_NULL_HANDLE);


	// Now we need to transition the swap chain image to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR layout, so we can present it.
	m_command->transitionImageLayout(
		m_swapchain->swapchain->getImage(swapchainImageIndex),
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		VK_IMAGE_ASPECT_COLOR_BIT,
		1,
		0,
		0,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
	);
}



void Application::startDraw()
{
	VkCommandBuffer cmd = m_vkCommandBuffers[m_currentFrame];

	m_inFlightFences[m_currentFrame]->wait();
	m_inFlightFences[m_currentFrame]->reset();

	vkResetCommandBuffer(cmd, 0);

	ft::core::CommandBuffer::BeginInfo beginInfo = {};

	vkBeginCommandBuffer(cmd, &beginInfo);

	m_command->transitionImageLayout(
		m_colorImage->image(),
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_IMAGE_ASPECT_COLOR_BIT,
		1,
		0,
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
	);
}

void Application::startRendering()
{
	VkCommandBuffer cmd = m_vkCommandBuffers[m_currentFrame];

	VkRenderingAttachmentInfo colorAttachment{};
	colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	colorAttachment.imageView = m_colorImage->view();
	colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.clearValue = {0.0f, 0.0f, 0.0f, 1.0f};

	VkRenderingAttachmentInfo depthAttachment{};
	depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	depthAttachment.imageView = m_depthImage->view();
	depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.clearValue = {1.0f, 0};

	VkRenderingInfo renderingInfo{};
	renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	renderingInfo.renderArea = {0, 0, m_colorImage->width(), m_colorImage->height()};
	renderingInfo.layerCount = 1;
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachments = &colorAttachment;
	renderingInfo.pDepthAttachment = &depthAttachment;

	vkCmdBeginRendering(cmd, &renderingInfo);
}

void Application::draw()
{
	VkCommandBuffer cmd = m_vkCommandBuffers[m_currentFrame];

	updateUniformBuffer(m_currentFrame);

	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicPipeline->pipeline->getVk());

	vkCmdBindDescriptorSets(
		cmd,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		m_graphicPipeline->layout->getVk(),
		0, 1,
		&m_descriptor->sets[m_currentFrame],
		0, nullptr
	);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_colorImage->width());
	viewport.height = static_cast<float>(m_colorImage->height());
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(cmd, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = m_swapchain->swapchain->getExtent();
	vkCmdSetScissor(cmd, 0, 1, &scissor);

	for (size_t i = 0; i < 1; i++)
	{
		ModelMatrix_push_constant pushConstant{};
		pushConstant.model = glm::rotate(glm::mat4(1.0f), m_timer.getElapsedTime() * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		vkCmdPushConstants(
			cmd,
			m_graphicPipeline->layout->getVk(),
			VK_SHADER_STAGE_VERTEX_BIT,
			0,
			sizeof(ModelMatrix_push_constant),
			&pushConstant
		);

		VkBuffer vertexBuffers[] = {m_mesh->vertexBuffer().buffer()};
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(cmd, 0, 1, vertexBuffers, offsets);

		vkCmdBindIndexBuffer(cmd, m_mesh->indexBuffer().buffer(), 0, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(cmd, m_mesh->indexCount(), 1, 0, 0, 0);
	}
}

void Application::endRendering()
{
	VkCommandBuffer cmd = m_vkCommandBuffers[m_currentFrame];

	vkCmdEndRendering(cmd);
}

void Application::endDraw()
{
	VkCommandBuffer cmd = m_vkCommandBuffers[m_currentFrame];

	m_command->transitionImageLayout(
		m_colorImage->image(),
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		VK_IMAGE_ASPECT_COLOR_BIT,
		1,
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		0,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT
	);

	vkEndCommandBuffer(cmd);

	VkSubmitInfo renderInfo{};
	renderInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkCommandBuffer commandBuffers[] = {m_vkCommandBuffers[m_currentFrame]};
	renderInfo.commandBufferCount = 1;
	renderInfo.pCommandBuffers = commandBuffers;

	VkSemaphore signalSemaphores[] = {m_renderFinishedSemaphores[m_currentFrame]->getVk()};
	renderInfo.signalSemaphoreCount = 1;
	renderInfo.pSignalSemaphores = signalSemaphores;

	m_device->graphicsQueue->submit(1, &renderInfo, m_inFlightFences[m_currentFrame]->getVk());



	// Now instead of rendering directly to the swap chain image, we render to the offscreen image, and then copy it to the swap chain image.

	uint32_t imageIndex;
	VkResult result = m_swapchain->swapchain->acquireNextImage(
		UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame]->getVk(), VK_NULL_HANDLE, &imageIndex
	);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	copyRenderedImageToSwapchainImage(imageIndex);

	// Finally, we present the swap chain image.
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	VkSemaphore copyImageSignalSemaphores[] = {
		m_swapchainUpdatedSemaphores[m_currentFrame]->getVk()
	};
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = copyImageSignalSemaphores;

	VkSwapchainKHR swapChains[] = {m_swapchain->swapchain->getVk()};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(m_device->presentQueue->getVk(), &presentInfo);


	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized)
	{
		m_framebufferResized = false;
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image.");
	}

	m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}