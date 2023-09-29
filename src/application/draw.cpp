#include "application.hpp"

void Application::drawFrame() {
	/* Wait for the corresponding frame to be finished */
	vkWaitForFences(this->device, 1, &this->inFlightFences[this->currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(this->device, this->swapChain, UINT64_MAX, this->imageAvailableSemaphores[this->currentFrame], VK_NULL_HANDLE, &imageIndex);

	/* Check if the swap chain is out of date (e.g. window was resized) and needs to be recreated */
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		/* Recreate the swap chain and try again next frame */
		this->recreateSwapChain();
		return;
	} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) { /* Don't recreate the swap chain if it is suboptimal because we already have an image to render to */
		throw std::runtime_error("failed to acquire swap chain image!");
	}
	
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

	/* Check if the swap chain is out of date or suboptimal and needs to be recreated */
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || this->framebufferResized) {
		recreateSwapChain();
		this->framebufferResized = false;
	} else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	this->currentFrame = (this->currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}