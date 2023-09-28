#include "application.hpp"

void Application::createCommandPool() {
	QueueFamilyIndices queueFamilyIndices = this->findQueueFamilies(this->physicalDevice);

	/* Create command pool for the graphics queue */
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	if (vkCreateCommandPool(this->device, &poolInfo, nullptr, &this->commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

void Application::createCommandBuffer() {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = this->commandPool;
	/*
	 * The level parameter specifies if the allocated command buffers are primary or secondary command buffers.
	 * VK_COMMAND_BUFFER_LEVEL_PRIMARY: Can be submitted to a queue for execution, but cannot be called from other command buffers.
	 * VK_COMMAND_BUFFER_LEVEL_SECONDARY: Cannot be submitted directly, but can be called from primary command buffers.
	 */
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(this->device, &allocInfo, &this->commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

void Application::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	/*
	 * The flags parameter specifies how we're going to use the command buffer.
	 * VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT: The command buffer will be rerecorded right after executing it once.
	 * VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT: This is a secondary command buffer that will be entirely within a single render pass.
	 * VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT: The command buffer can be resubmitted while it is also already pending execution.
	 */
	beginInfo.flags = 0; // Optional
	/* Only for secondary command buffers */
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(this->commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = this->renderPass;
	renderPassInfo.framebuffer = this->swapChainFramebuffers[imageIndex];
	/* Specify the render area by setting the offset and extent fields */
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = this->swapChainExtent;
	/* Specify the clear color value to use for VK_ATTACHMENT_LOAD_OP_CLEAR, as well as the default values for the other attachments */
	VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(this->commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(this->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->graphicsPipeline);

	/* We set viewport and scissor as dynamic state, so we need to set them before drawing */
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(this->swapChainExtent.width);
	viewport.height = static_cast<float>(this->swapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(this->commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = this->swapChainExtent;
	vkCmdSetScissor(this->commandBuffer, 0, 1, &scissor);

	/*
		* vertexCount: The number of vertices to draw.
		* instanceCount: Used for instanced rendering, use 1 if you're not doing that.
		* firstVertex: Used as an offset into the vertex buffer, defines the lowest value of gl_VertexIndex.
		* firstInstance: Used as an offset for instanced rendering, defines the lowest value of gl_InstanceIndex.
		*/
	vkCmdDraw(this->commandBuffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(this->commandBuffer);

	if (vkEndCommandBuffer(this->commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}

}