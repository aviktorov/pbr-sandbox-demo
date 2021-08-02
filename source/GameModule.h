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

	struct Camera
	{
		glm::vec4 parameters; // znear, zfar, 1 / znear, 1 / zfar
		glm::mat4 projection;
		bool main {false};
	};

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
