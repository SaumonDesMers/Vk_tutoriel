#pragma once

#include "model.hpp"

#include "ft_glm/ft_glm.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <unordered_map>

namespace ft
{

	struct TransformComponent
	{
		glm::vec3 translation{};
		glm::vec3 scale{1.0f, 1.0f, 1.0f};
		glm::vec3 rotation{};

		// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
		// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};

	struct LightPointComponent
	{
		float intensity = 1.0f;
	};

	class GameObject
	{

	public:

		using id_t = uint32_t;
		using Map = std::unordered_map<id_t, GameObject>;

		glm::vec3 color;
		TransformComponent transform{};

		// optional components
		std::shared_ptr<Model> model;
		std::unique_ptr<LightPointComponent> lightPoint = nullptr;

		static GameObject create()
		{
			static id_t id = 0;
			return GameObject(id++);
		}

		static GameObject createLightPoint(
			float intensity = 1.0f,
			float radius = 0.1f,
			glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f)
		);

		GameObject(const GameObject &) = delete;
		GameObject &operator=(const GameObject &) = delete;
		GameObject(GameObject &&) = default;
		GameObject &operator=(GameObject &&) = default;

		id_t id() const { return m_id; }
	
	private:

		id_t m_id;

		GameObject(id_t id) : m_id(id) {}

	};

}