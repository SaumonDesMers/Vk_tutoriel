#include "application.hpp"

void Application::mainLoop() {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		drawFrame();
	}
	vkDeviceWaitIdle(device);
}