#include "application.hpp"

void Application::createImageViews() {
	this->swapChainImageViews.resize(this->swapChainImages.size());

	for (uint32_t i = 0; i < this->swapChainImages.size(); i++) {
        this->swapChainImageViews[i] = createImageView(this->swapChainImages[i], this->swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    }
}

VkImageView Application::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
	/* Specify how the image data should be interpreted */
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
	/* The components field allows you to swizzle the color channels around. For example, you can map all of the channels to the red channel for a monochrome texture.
		You can also map constant values of 0 and 1 to a channel. In our case we'll stick to the default mapping */
	viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	/* The subresourceRange field describes what the image's purpose is and which part of the image should be accessed.
		Our images will be used as color targets without any mipmapping levels or multiple layers.
		Basicly, we are not doing anything fancy here like stereoscopic 3D */
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(this->device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}