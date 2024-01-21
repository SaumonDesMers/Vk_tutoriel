#pragma once

#include <cppVulkanAPI.hpp>

#include <glm/glm.hpp>

class GameObject
{

public:

	struct Transform
	{
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;

		Transform();
		~Transform();

		glm::mat4 modelMatrix();
	};

	Transform transform;
	ft::Mesh::ID meshID;

	GameObject();

	~GameObject();

private:

};