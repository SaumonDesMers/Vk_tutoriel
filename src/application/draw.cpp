#include "application.hpp"

void Application::drawFrame() {
	vkWaitForFences(this->device, 1, &this->inFlightFence, VK_TRUE, UINT64_MAX);
	vkResetFences(this->device, 1, &this->inFlightFence);

	uint32_t imageIndex;
	vkAcquireNextImageKHR(this->device, this->swapChain, UINT64_MAX, this->imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

	vkResetCommandBuffer(this->commandBuffer, 0);
	this->recordCommandBuffer(this->commandBuffer, imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	/* Specify which semaphores to wait on before execution begins and in which stage(s) of the pipeline to wait */
	VkSemaphore waitSemaphores[] = {this->imageAvailableSemaphore};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	/* Specify which command buffers to actually submit for execution */
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &this->commandBuffer;
	/* Specify which semaphores to signal once the command buffer(s) have finished execution */
	VkSemaphore signalSemaphores[] = {this->renderFinishedSemaphore};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(this->graphicsQueue, 1, &submitInfo, this->inFlightFence) != VK_SUCCESS) {
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

	vkQueuePresentKHR(this->presentQueue, &presentInfo);
}