#include "application.hpp"
#include "vertex.hpp"

void Application::createMvpUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(ModelViewPerspective);

    this->uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    this->uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    this->uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        this->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, this->uniformBuffers[i], this->uniformBuffersMemory[i]);
		/* Persistent mapping of the uniform buffer memory */
        vkMapMemory(this->device, this->uniformBuffersMemory[i], 0, bufferSize, 0, &this->uniformBuffersMapped[i]);
    }
}

void Application::createColorTextureBlendingBuffer() {
	VkDeviceSize bufferSize = sizeof(ColorTextureBlending);

	this->textureEnabledBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	this->textureEnabledBufferMemory.resize(MAX_FRAMES_IN_FLIGHT);
	this->textureEnabledBufferMapped.resize(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		this->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, this->textureEnabledBuffers[i], this->textureEnabledBufferMemory[i]);
		/* Persistent mapping of the uniform buffer memory */
		vkMapMemory(this->device, this->textureEnabledBufferMemory[i], 0, bufferSize, 0, &this->textureEnabledBufferMapped[i]);
	}
}
