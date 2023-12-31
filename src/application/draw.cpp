#include "application.hpp"
#include "vertex.hpp"

#include <chrono>

void Application::drawFrame() {
	/* Wait for the corresponding frame to be finished */
	vkWaitForFences(this->device, 1, &this->inFlightFences[this->currentFrame], VK_TRUE, UINT64_MAX);

	/* Acquire an image from the swap chain */
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(this->device, this->swapChain, UINT64_MAX, this->imageAvailableSemaphores[this->currentFrame], VK_NULL_HANDLE, &imageIndex);

	/* Check if the swap chain is out of date (e.g. window was resized) and needs to be recreated */
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		/* Recreate the swap chain and try again next frame */
		this->recreateSwapChain();
		return;
	} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) { /* Don't recreate the swap chain if it is smvpptimal because we already have an image to render to */
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	/* Update the uniforms buffers */
	this->updateMvpUniformBuffer(this->currentFrame);
	this->updateTextureEnabledBuffer(this->currentFrame);
	
	/* Reset the fence only if we are submitting work to prevent a deadlock */
	vkResetFences(this->device, 1, &this->inFlightFences[this->currentFrame]);

	vkResetCommandBuffer(this->commandBuffers[this->currentFrame], 0);
	this->recordCommandBuffer(this->commandBuffers[this->currentFrame], imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	/* Specify which semaphores to wait on before execution begins and in which stage(s) of the pipeline to wait */
	VkSemaphore waitSemaphores[] = {this->imageAvailableSemaphores[this->currentFrame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	/* Specify which command buffers to actually submit for execution */
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &this->commandBuffers[this->currentFrame];
	/* Specify which semaphores to signal once the command buffer(s) have finished execution */
	VkSemaphore signalSemaphores[] = {this->renderFinishedSemaphores[this->currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(this->graphicsQueue, 1, &submitInfo, this->inFlightFences[this->currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	/* Specify which semaphores to wait on before presentation can happen */
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	/* Specify the swap chains to present images to and the index of the image for each swap chain */
	VkSwapchainKHR swapChains[] = {this->swapChain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	/* Specify an array of VkResult values to check for every individual swap chain if presentation was successful */
	presentInfo.pResults = nullptr; // Optional

	result = vkQueuePresentKHR(this->presentQueue, &presentInfo);

	/* Check if the swap chain is out of date or smvpptimal and needs to be recreated */
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || this->framebufferResized) {
		recreateSwapChain();
		this->framebufferResized = false;
	} else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	this->currentFrame = (this->currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Application::updateMvpUniformBuffer(uint32_t currentImage) {
	float time = this->getTime();

	/* TODO: remove this */
	// Glm_ModelViewPerspective glm_mvp{};
	// glm_mvp.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	// glm_mvp.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	// glm_mvp.proj = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float) swapChainExtent.height, 0.1f, 10.0f);

	/* To rotate an object around itself, we need to move it to the center, rotate it, and then move it to the new position */
	ft::mat4 translateToOrigin = ft::translate(-this->object->getBaricenter());
	ft::mat4 rotate = ft::rotate(time * ft::radians(90.0f), ft::vec3(0.0f, 1.0f, 0.0f));
	ft::mat4 translateToPosition = ft::translate(this->object->position);

	ft::mat4 scale = ft::scale(ft::vec3(1.0f, 1.0f, 1.0f));

	ModelViewPerspective mvp{};
	mvp.model = translateToPosition * rotate * translateToOrigin * scale;
	mvp.view = ft::lookAt(
		this->camera.getPosition(), /* camera position */
		this->camera.getTarget(), /* target position */
		this->camera.getUp() /* up vector */
	);
	mvp.proj = ft::perspective<float>(
		ft::radians(45.0f), /* field of view in radians */
		(float) swapChainExtent.width / (float) swapChainExtent.height, /* aspect ratio */
		0.1f, /* near plane */
		100.0f /* far plane */
	);

	/* GLM was originally designed for OpenGL, where the Y coordinate of the clip coordinates is inverted.
		The easiest way to compensate for that is to flip the sign on the scaling factor of the Y axis in the projection matrix. */
	mvp.proj[1][1] *= -1;

	memcpy(this->uniformBuffersMapped[currentImage], &mvp, sizeof(mvp));
}

void Application::updateTextureEnabledBuffer(uint32_t currentImage) {
	static float lastFrameTime = this->getTime();
	float currentFrameTime = this->getTime();

	float transitionDuration = 1.0f;

	float deltaRatio = 1.0f * ((currentFrameTime - lastFrameTime) / transitionDuration);

	if (this->textureEnabled) {
		this->colorTextureBlending.ratio = fmin(1.0f, this->colorTextureBlending.ratio + deltaRatio);
	} else {
		this->colorTextureBlending.ratio = fmax(0.0f, this->colorTextureBlending.ratio - deltaRatio);
	}

	lastFrameTime = currentFrameTime;
	memcpy(this->textureEnabledBufferMapped[currentImage], &this->colorTextureBlending, sizeof(this->colorTextureBlending));
}