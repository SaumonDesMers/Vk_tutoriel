#pragma once

#include "camera.hpp"
#include "game_object.hpp"

#include <vulkan/vulkan.h>

namespace ft
{

#define MAX_LIGHTS 10

	struct LightPoint
	{
		glm::vec4 position{};
		glm::vec4 color{};
	};

	struct GlobalUBO
	{
		glm::mat4 projection{1.0f};
		glm::mat4 view{1.0f};
		glm::vec4 ambientLightColor{1.0f, 1.0f, 1.0f, 0.02f};
		LightPoint lights[MAX_LIGHTS];
		int lightCount = 0;
	};

	struct FrameInfo
	{
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		Camera &camera;
		VkDescriptorSet globalDescriptorSet;
		GameObject::Map &gameObjects;
	};

}