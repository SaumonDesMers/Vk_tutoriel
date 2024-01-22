#include "uniform_buffer.hpp"

#include <stdexcept>

namespace LIB_NAMESPACE
{
	UniformBuffer::UniformBuffer(
		VkDevice device,
		VkPhysicalDevice physicalDevice,
		CreateInfo& createInfo
	)
	{
		createBuffer(device, physicalDevice, createInfo);
		createDescriptor(device, createInfo);
	}

	UniformBuffer::~UniformBuffer()
	{
	}

	void UniformBuffer::createBuffer(
		VkDevice device,
		VkPhysicalDevice physicalDevice,
		CreateInfo& createInfo
	)
	{
		m_buffers.resize(MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = createInfo.size;
			bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			m_buffers[i] = std::make_unique<ft::Buffer>(
				device,
				physicalDevice,
				bufferInfo,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			);

			m_buffers[i]->map();
		}
	}

	void UniformBuffer::createDescriptor(
		VkDevice device,
		CreateInfo& createInfo
	)
	{
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = 0;
		layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		layoutBinding.descriptorCount = 1;
		layoutBinding.stageFlags = createInfo.stageFlags;

		ft::Descriptor::CreateInfo descriptorInfo{};
		descriptorInfo.bindings = { layoutBinding };
		descriptorInfo.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		m_descriptor = std::make_unique<Descriptor>(device, descriptorInfo);


		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = m_buffers[i]->buffer();
			bufferInfo.offset = 0;
			bufferInfo.range = createInfo.size;

			VkWriteDescriptorSet uboDescriptorWrites{};

			uboDescriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			uboDescriptorWrites.dstSet = m_descriptor->set(i);
			uboDescriptorWrites.dstBinding = 0;
			uboDescriptorWrites.dstArrayElement = 0;
			uboDescriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uboDescriptorWrites.descriptorCount = 1;
			uboDescriptorWrites.pBufferInfo = &bufferInfo;

			vkUpdateDescriptorSets(
				device,
				1,
				&uboDescriptorWrites,
				0, nullptr
			);
		}
	}

}