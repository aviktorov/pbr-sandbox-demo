#pragma once

#include <glm/mat4x4.hpp>
#include <render/backend/Driver.h>
#include <game/World.h>

namespace ecs::game
{
	// Resources

	// Components
	struct Transform
	{
		glm::mat4 world;
		glm::mat4 iworld;
	};

	struct FPSCameraTransform
	{
		float vertical_angle {0.0f};
		float horizontal_angle {0.0f};
		glm::vec3 up {0.0f, 0.0f, 1.0f};
		glm::vec3 position {0.0f, 0.0f, 0.0f};
	};

	struct Camera
	{
		glm::vec4 parameters; // znear, zfar, 1 / znear, 1 / zfar
		glm::mat4 projection;
		bool main {false};
	};

	// Helpers
	extern glm::vec3 getDirection(const FPSCameraTransform &transform);

	// Iterators / Systems

	// Module
	extern void init(::game::World *world);
	extern void shutdown(::game::World *world);
}

template<>
struct game::TypeTraits<ecs::game::Transform>
{
	static constexpr const char *name = "ecs::game::Transform";
};

template<>
struct game::TypeTraits<ecs::game::Camera>
{
	static constexpr const char *name = "ecs::game::Camera";
};

template<>
struct game::TypeTraits<ecs::game::FPSCameraTransform>
{
	static constexpr const char *name = "ecs::game::FPSCameraTransform";
};
