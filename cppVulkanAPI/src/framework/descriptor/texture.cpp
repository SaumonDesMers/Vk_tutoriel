#include "texture.hpp"
#include "framework/memory/buffer.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <stdexcept>

namespace LIB_NAMESPACE
{
	Texture::Texture(
		VkDevice device,
		VkPhysicalDevice physicalDevice,
		Command& command,
		CreateInfo& createInfo
	)
	{
		int texChannels;
		stbi_uc* pixels = stbi_load(createInfo.filepath.c_str(), &m_width, &m_height, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = m_width * m_height * 4;

		if (pixels == nullptr)
		{
			throw std::runtime_error("failed to load texture: " + createInfo.filepath);
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

		createImage(device, physicalDevice, createInfo);

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

		createSampler(device, physicalDevice, createInfo);
		createDescriptor(device, createInfo);
	}

	Texture::~Texture()
	{
	}

	void Texture::createImage(
		VkDevice device,
		VkPhysicalDevice physicalDevice,
		CreateInfo& createInfo
	)
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = static_cast<uint32_t>(m_width);
		imageInfo.extent.height = static_cast<uint32_t>(m_height);
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = createInfo.mipLevel;
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
		viewInfo.subresourceRange.levelCount = createInfo.mipLevel;
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

	void Texture::createSampler(
		VkDevice device,
		VkPhysicalDevice physicalDevice,
		CreateInfo& createInfo
	)
	{
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;

		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(physicalDevice, &properties);

		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

		samplerInfo.unnormalizedCoordinates = VK_FALSE;

		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = static_cast<float>(createInfo.mipLevel);

		m_sampler = std::make_unique<ft::core::Sampler>(device, samplerInfo);
	}

	void Texture::createDescriptor(
		VkDevice device,
		CreateInfo& createInfo
	)
	{
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = 0;
		layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		layoutBinding.descriptorCount = 1;
		layoutBinding.stageFlags = createInfo.stageFlags;

		ft::Descriptor::CreateInfo descriptorInfo{};
		descriptorInfo.bindings = { layoutBinding };
		descriptorInfo.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		m_descriptor = std::make_unique<Descriptor>(device, descriptorInfo);


		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = m_image->view();
			imageInfo.sampler = m_sampler->getVk();

			VkWriteDescriptorSet samplerDescriptorWrites{};
			samplerDescriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			samplerDescriptorWrites.dstSet = m_descriptor->set(i);
			samplerDescriptorWrites.dstBinding = 0;
			samplerDescriptorWrites.dstArrayElement = 0;
			samplerDescriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			samplerDescriptorWrites.descriptorCount = 1;
			samplerDescriptorWrites.pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(
				device,
				1,
				&samplerDescriptorWrites,
				0, nullptr
			);
		}
	}

}