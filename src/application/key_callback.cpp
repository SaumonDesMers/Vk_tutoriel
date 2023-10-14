#include "application.hpp"

#define CASE(key, func) case key: app->func(key, scancode, action, mods); break;

void Application::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
	
	/* error: invalid use of member 'key_callbacks' in static member function */
	// (app->*key_callbacks[key])(key, scancode, action, mods);

	switch (key) {
		CASE(GLFW_KEY_ESCAPE, escape)
		CASE(GLFW_KEY_UP, arrowUp)
		CASE(GLFW_KEY_DOWN, arrowDown)
		CASE(GLFW_KEY_LEFT, arrowLeft)
		CASE(GLFW_KEY_RIGHT, arrowRight)
		CASE(GLFW_KEY_A, key_a)
		CASE(GLFW_KEY_D, key_d)
		CASE(GLFW_KEY_W, key_w)
		CASE(GLFW_KEY_S, key_s)
		default:
			break;
	}
}

#undef CASE

void Application::mapKeyCallback() {
	this->key_callbacks[GLFW_KEY_ESCAPE] = &Application::escape;
}

void Application::escape(int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		glfwSetWindowShouldClose(this->window, GLFW_TRUE);
	}
}

void Application::arrowUp(int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		this->object->position[1] += 0.1f;
	}
}

void Application::arrowDown(int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		this->object->position[1] -= 0.1f;
	}
}

void Application::arrowLeft(int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		this->object->position[0] -= 0.1f;
	}
}

void Application::arrowRight(int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		this->object->position[0] += 0.1f;
	}
}

void Application::key_a(int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		this->camera.moveLeft(0.1f);
	}
}

void Application::key_d(int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		this->camera.moveRight(0.1f);
	}
}

void Application::key_w(int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		this->camera.moveUp(0.1f);
	}
}

void Application::key_s(int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		this->camera.moveDown(0.1f);
	}
}
