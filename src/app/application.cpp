#include "application.hpp"

#include "camera.hpp"
#include "simple_render_system.hpp"
#include "point_light_system.hpp"
#include "keyboard_movement_controller.hpp"
#include "buffer.hpp"

#include "ft_glm/ft_glm.hpp"

#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <chrono>

namespace ft
{

	Application::Application()
	{
		m_globalPool = DescriptorPool::Builder(m_device)
			.setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadGameObjects();
	}

	Application::~Application() {}

	void Application::run()
	{
		std::vector<std::unique_ptr<Buffer>> uniformBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < uniformBuffers.size(); i++)
		{
			uniformBuffers[i] = std::make_unique<Buffer>(
				m_device,
				sizeof(GlobalUBO),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);
			uniformBuffers[i]->map();
		}

		auto globalSetLayout = DescriptorSetLayout::Builder(m_device)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();
		
		std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (size_t i = 0; i < globalDescriptorSets.size(); i++)
		{
			auto bufferInfo = uniformBuffers[i]->descriptorInfo();
			DescriptorWriter(*globalSetLayout, *m_globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem(
			m_device,
			m_renderer.getSwapChainRenderPass(),
			globalSetLayout->getDescriptorSetLayout()
		);
		PointLightSystem pointLightSystem(
			m_device,
			m_renderer.getSwapChainRenderPass(),
			globalSetLayout->getDescriptorSetLayout()
		);

		Camera camera{};
		// camera.setViewDirection(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		camera.setViewTarget(glm::vec3(-1.f, -2.f, -2.f), glm::vec3(0.0f, 0.0f, 2.5f));

		auto viewerObject = GameObject::create();
		viewerObject.transform.translation.z = -2.5f;
		KeyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!m_window.shouldClose())
		{
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			cameraController.moveInPlaneXZ(m_window.getGLFWwindow(), frameTime, viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
			
			float aspect = m_renderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 100.0f);

			if (auto commandBuffer = m_renderer.beginFrame())
			{
				int frameIndex = m_renderer.getFrameIndex();
				FrameInfo frameInfo
				{
					.frameIndex = frameIndex,
					.frameTime = frameTime,
					.commandBuffer = commandBuffer,
					.camera = camera,
					.globalDescriptorSet = globalDescriptorSets[frameIndex],
					.gameObjects = m_gameObjects
				};

				// update
				GlobalUBO ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				pointLightSystem.update(frameInfo, ubo);
				uniformBuffers[frameIndex]->writeToBuffer(&ubo);
				uniformBuffers[frameIndex]->flush();

				// render
				m_renderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.rendergameObjects(frameInfo);
				pointLightSystem.render(frameInfo);
				m_renderer.endSwapChainRenderPass(commandBuffer);
				m_renderer.endFrame();
			}
		}

		vkDeviceWaitIdle(m_device.device());
	}

	void Application::loadGameObjects()
	{
		std::shared_ptr<Model> model = Model::createModelFromFile(m_device, "models/flat_vase.obj");
		auto flatVase = GameObject::create();
		flatVase.model = model;
		flatVase.transform.translation = {-0.5f, 0.5f, 0.0f};
		flatVase.transform.scale = glm::vec3(3.0f, 2.0f, 3.0f);
		m_gameObjects.emplace(flatVase.id(), std::move(flatVase));


		model = Model::createModelFromFile(m_device, "models/smooth_vase.obj");
		auto smoothVase = GameObject::create();
		smoothVase.model = model;
		smoothVase.transform.translation = {0.5f, 0.5f, 0.0f};
		smoothVase.transform.scale = glm::vec3(3.0f, 2.0f, 3.0f);
		m_gameObjects.emplace(smoothVase.id(), std::move(smoothVase));

		model = Model::createModelFromFile(m_device, "models/quad.obj");
		auto quad = GameObject::create();
		quad.model = model;
		quad.transform.translation = {0.0f, 0.5f, 0.0f};
		quad.transform.scale = glm::vec3(3.0f, 1.0f, 3.0f);
		m_gameObjects.emplace(quad.id(), std::move(quad));

		std::vector<glm::vec3> lightColors{
			{1.f, .1f, .1f},
			{.1f, .1f, 1.f},
			{.1f, 1.f, .1f},
			{1.f, 1.f, .1f},
			{.1f, 1.f, 1.f},
			{1.f, 1.f, 1.f}
		};

		for (size_t i = 0; i < lightColors.size(); i++)
		{
			auto light = GameObject::createLightPoint(1.0f, 0.1f, lightColors[i]);
			light.color = lightColors[i];
			auto rotateLight = glm::rotate(
				glm::mat4(1.0f),
				(i * glm::two_pi<float>()) / lightColors.size(),
				glm::vec3(0.0f, -1.0f, 0.0f)
			);
			light.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f));
			m_gameObjects.emplace(light.id(), std::move(light));
		}
	}

}