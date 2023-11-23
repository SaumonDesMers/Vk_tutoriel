#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace ft
{

	class Camera
	{

	public:

		void setOrthographicProjection(float left, float right, float bottom, float top, float near, float far);
		void setPerspectiveProjection(float fovy, float aspect, float near, float far);

		void setViewDirection(
			const glm::vec3 position,
			const glm::vec3 direction,
			const glm::vec3 up = glm::vec3{0.0f, -1.0f, 0.0f}
		);
		void setViewTarget(
			const glm::vec3 position,
			const glm::vec3 target,
			const glm::vec3 up = glm::vec3{0.0f, -1.0f, 0.0f}
		);
		void setViewYXZ(
			const glm::vec3 position,
			const glm::vec3 rotation
		);

		const glm::mat4 &getProjection() const { return m_projectionMatrix; }
		const glm::mat4 &getView() const { return m_viewMatrix; }

	private:

		glm::mat4 m_projectionMatrix{1.0f};
		glm::mat4 m_viewMatrix{1.0f};

	};
	
}
