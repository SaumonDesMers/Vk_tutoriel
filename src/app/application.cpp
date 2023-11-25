#include "app/application.hpp"

#include "app/camera.hpp"
#include "app/render_system.hpp"
#include "app/keyboard_movement_controller.hpp"

#include "ft_glm/ft_glm.hpp"

#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <chrono>

namespace ft
{

	Application::Application()
	{
		loadGameObjects();
	}

	Application::~Application() {}

	void Application::run()
	{
		RenderSystem renderSystem(m_device, m_renderer.getSwapChainRenderPass());
		Camera camera{};
		// camera.setViewDirection(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		camera.setViewTarget(glm::vec3(-1.f, -2.f, -2.f), glm::vec3(0.0f, 0.0f, 2.5f));

		auto viewerObject = GameObject::create();
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
			camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f);

			if (auto commandBuffer = m_renderer.beginFrame())
			{
				m_renderer.beginSwapChainRenderPass(commandBuffer);
				renderSystem.rendergameObjects(commandBuffer, m_gameObjects, camera);
				m_renderer.endSwapChainRenderPass(commandBuffer);
				m_renderer.endFrame();
			}
		}

		vkDeviceWaitIdle(m_device.device());
	}

	void Application::loadGameObjects()
	{
		std::shared_ptr<Model> model = Model::createModelFromFile(m_device, "models/flat_vase.obj");
		auto gameObj1 = GameObject::create();
		gameObj1.model = model;
		gameObj1.transform.translation = {-0.5f, 0.5f, 2.5f};
		gameObj1.transform.scale = glm::vec3(3.0f, 2.0f, 3.0f);

		m_gameObjects.push_back(std::move(gameObj1));


		model = Model::createModelFromFile(m_device, "models/smooth_vase.obj");
		auto gameObj2 = GameObject::create();
		gameObj2.model = model;
		gameObj2.transform.translation = {0.5f, 0.5f, 2.5f};
		gameObj2.transform.scale = glm::vec3(3.0f, 2.0f, 3.0f);

		m_gameObjects.push_back(std::move(gameObj2));
	}

}