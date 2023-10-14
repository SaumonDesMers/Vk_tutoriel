#include "application.hpp"
#include "vertex.hpp"

void Application::createVertexBuffer() {
	VkDeviceSize bufferSize = sizeof(this->object->getVertices()[0]) * this->object->getVertices().size();

	/* Create a temporary buffer to hold the vertex data to be copied to the device local buffer which is optimized for device access but is not accessible by the CPU.
	 * VK_BUFFER_USAGE_TRANSFER_SRC_BIT: Buffer can be used as source in a memory transfer operation.
	 * VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT: Memory is visible to the host (CPU).
	 * VK_MEMORY_PROPERTY_HOST_COHERENT_BIT: Allows mapping a memory region for persistant access.
	 */
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	/* Map the vertex buffer memory to the staging buffer memory and copy the vertex data to it */
    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, this->object->getVertices().data(), (size_t) bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

	/* Create the vertex buffer:
	 * VK_BUFFER_USAGE_TRANSFER_DST_BIT: Buffer can be used as destination in a memory transfer operation.
	 * VK_BUFFER_USAGE_VERTEX_BUFFER_BIT: Buffer can be used as vertex buffer.
	 * VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT: Memory is only accessible by the GPU.
	 */
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->vertexBuffer, this->vertexBufferMemory);

	copyBuffer(stagingBuffer, this->vertexBuffer, bufferSize);

	/* Destroy the staging buffer */
	vkDestroyBuffer(this->device, stagingBuffer, nullptr);
    vkFreeMemory(this->device, stagingBufferMemory, nullptr);
}
