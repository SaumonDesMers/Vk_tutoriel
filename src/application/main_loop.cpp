#include "application.hpp"

void Application::mainLoop() {
	while (!glfwWindowShouldClose(this->window)) {
		glfwPollEvents();
		this->drawFrame();
	}
	vkDeviceWaitIdle(this->device);
}