#include "application.hpp"

void Application::createFramebuffers() {
	this->swapChainFramebuffers.resize(this->swapChainImageViews.size());

	for (size_t i = 0; i < this->swapChainImageViews.size(); i++) {
		std::array<VkImageView, 2> attachments = {
			this->swapChainImageViews[i],
			this->depthImageView
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = this->renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = this->swapChainExtent.width;
		framebufferInfo.height = this->swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(this->device, &framebufferInfo, nullptr, &this->swapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}