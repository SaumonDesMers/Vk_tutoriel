#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "ft_glm/ft_glm.hpp"

class Camera {

	ft::vec3 position;
	ft::vec3 orientation;
	ft::vec3 up;

public:

	Camera(ft::vec3&& position, ft::vec3 lookat, ft::vec3&& up): position(std::move(position)), up(std::move(up.normalize())) {
		this->orientation = (lookat - this->position).normalize();
	}

	ft::vec3 getPosition() const {
		return this->position;
	}

	ft::vec3 getTarget() const {
		return this->position + this->orientation;
	}

	ft::vec3 getUp() const {
		return this->up;
	}

	void moveRight(float speed) {
		this->position += this->orientation.cross(this->up) * speed;
	}

	void moveLeft(float speed) {
		this->position -= this->orientation.cross(this->up) * speed;
	}

	void moveForward(float speed) {
		this->position += this->orientation * speed;
	}

	void moveBackward(float speed) {
		this->position -= this->orientation * speed;
	}

	void moveUp(float speed) {
		this->position += this->up * speed;
	}

	void moveDown(float speed) {
		this->position -= this->up * speed;
	}
};

#endif // CAMERA_HPP