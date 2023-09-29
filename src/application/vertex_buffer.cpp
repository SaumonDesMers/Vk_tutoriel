#include "application.hpp"
#include "vertex.hpp"

void Application::createVertexBuffer() {
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeof(vertices[0]) * vertices.size();
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(this->device, &bufferInfo, nullptr, &this->vertexBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create vertex buffer!");
	}

	/* Get the memory requirements for the vertex buffer */
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(this->device, this->vertexBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = this->findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(this->device, &allocInfo, nullptr, &this->vertexBufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate vertex buffer memory!");
	}

	/* Bind the vertex buffer to the allocated memory */
	vkBindBufferMemory(this->device, this->vertexBuffer, this->vertexBufferMemory, 0);

	/* Map the vertex buffer memory to a CPU accessible memory location and copy the vertex data into it */
	void* data;
	vkMapMemory(device, vertexBufferMemory, 0, bufferInfo.size, 0, &data);
		memcpy(data, vertices.data(), (size_t) bufferInfo.size);
	vkUnmapMemory(device, vertexBufferMemory);
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