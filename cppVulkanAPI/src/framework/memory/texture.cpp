#include "texture.hpp"
#include "buffer.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <stdexcept>

namespace LIB_NAMESPACE
{
	Texture::Texture(
		VkDevice device,
		VkPhysicalDevice physicalDevice,
		Command& command,
		std::string path
	)
	{
		int texChannels;
		stbi_uc* pixels = stbi_load(path.c_str(), &m_width, &m_height, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = m_width * m_height * 4;

		if (pixels == nullptr)
		{
			throw std::runtime_error("failed to load texture: " + path);
		}

		ft::Buffer stagingBuffer = ft::Buffer::createStagingBuffer(
			device,
			physicalDevice,
			imageSize
		);

		stagingBuffer.map();
		stagingBuffer.write((void*)pixels, imageSize);
		stagingBuffer.unmap();

		stbi_image_free(pixels);

		createImage(device, physicalDevice);

		command.transitionImageLayout(
			m_image->image(),
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_ASPECT_COLOR_BIT,
			m_image->mipLevels(),
			0,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT
		);

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = {0, 0, 0};
		region.imageExtent = {
			static_cast<uint32_t>(m_width),
			static_cast<uint32_t>(m_height),
			1
		};

		command.copyBufferToImage(
			stagingBuffer.buffer(),
			m_image->image(),
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);
	}

	Texture::Texture(Texture&& other)
	{
		m_image = std::move(other.m_image);
		m_sampler = std::move(other.m_sampler);
	}

	Texture::~Texture()
	{
	}

	void Texture::createImage(VkDevice device, VkPhysicalDevice physicalDevice)
	{
		uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(m_width, m_height)))) + 1;

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = static_cast<uint32_t>(m_width);
		imageInfo.extent.height = static_cast<uint32_t>(m_height);
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = mipLevels;
		imageInfo.arrayLayers = 1;
		imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = 
			VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
			VK_IMAGE_USAGE_TRANSFER_DST_BIT |
			VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = VK_NULL_HANDLE;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = mipLevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		m_image = std::make_unique<Image>(
			device,
			physicalDevice,
			imageInfo,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			viewInfo
		);
	}

}