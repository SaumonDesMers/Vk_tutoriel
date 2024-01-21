#include "game_object.hpp"

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
}

GameObject::Transform::Transform():
	position(glm::vec3(0.0f)),
	rotation(glm::vec3(0.0f)),
	scale(glm::vec3(1.0f))
{
}

GameObject::Transform::~Transform()
{
}

glm::mat4 GameObject::Transform::modelMatrix()
{
	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, position);

	model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	model = glm::scale(model, scale);

	return model;
}