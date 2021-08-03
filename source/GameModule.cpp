#include "GameModule.h"

#include <glm/trigonometric.hpp>

namespace ecs::game
{
	/*
	 */
	void init(::game::World *world)
	{

	}

	void shutdown(::game::World *world)
	{

	}

	/*
	 */
	glm::vec3 getDirection(const FPSCameraTransform &transform)
	{
		float sin_vertical = sin(glm::radians(transform.vertical_angle));
		float cos_vertical = cos(glm::radians(transform.vertical_angle));
		float sin_horizontal = sin(glm::radians(transform.horizontal_angle));
		float cos_horizontal = cos(glm::radians(transform.horizontal_angle));

		return glm::vec3(cos_horizontal * cos_vertical, sin_horizontal * cos_vertical, sin_vertical);
	}
}
