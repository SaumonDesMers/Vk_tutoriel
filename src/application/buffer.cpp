#include "application.hpp"

/* TODO: It should be noted that in a real world application, you're not supposed to actually call vkAllocateMemory for every individual buffer.
 * The maximum number of simultaneous memory allocations is limited by the maxMemoryAllocationCount physical device limit, which may be as low as 4096 even on high end hardware like an NVIDIA GTX 1080.
 * The right way to allocate memory for a large number of objects at the same time is to create a custom allocator
 * that splits up a single allocation among many different objects by using the offset parameters that we've seen in many functions.
 * This is known as "memory aliasing" and is a common strategy in Vulkan.
 */

/*
 * Helper function to create a buffer.
 *
 * usage can be:
 * 	VK_BUFFER_USAGE_TRANSFER_SRC_BIT: Buffer can be used as source in a memory transfer operation.
 * 	VK_BUFFER_USAGE_TRANSFER_DST_BIT: Buffer can be used as destination in a memory transfer operation.
 * 	VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT: Buffer can be used as uniform buffer.
 * 	VK_BUFFER_USAGE_INDEX_BUFFER_BIT: Buffer can be used as index buffer.
 * 	VK_BUFFER_USAGE_VERTEX_BUFFER_BIT: Buffer can be used as vertex buffer.
 * 
 * properties can be:
 * 	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT: Memory is visible to the host (CPU).
 * 	VK_MEMORY_PROPERTY_HOST_COHERENT_BIT: Allows mapping a memory region for persistant access.
 * 	VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT: Memory is only accessible by the GPU.
 */
void Application::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(this->device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

	/* Get the memory requirements for the vertex buffer */
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(this->device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = this->findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(this->device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

	/* Bind the vertex buffer to the allocated memory */
    vkBindBufferMemory(this->device, buffer, bufferMemory, 0);
}

uint32_t Application::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(this->physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

void Application::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
	/* TODO: create a permanent command buffer for this (using VK_COMMAND_POOL_CREATE_TRANSIENT_BIT) */
	
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer);
}