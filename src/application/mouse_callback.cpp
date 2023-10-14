#include "application.hpp"

void Application::scrollCallback(GLFWwindow* window, double xpos, double ypos) {
	auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));

	app->camera.moveForward(ypos * 0.1f);
}