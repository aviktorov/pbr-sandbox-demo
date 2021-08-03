#include "Application.h"
#include "IO.h"
#include "RenderGraph.h"
#include "ResourceManager.h"
#include "SwapChain.h"
#include "GameModule.h"

#include <game/World.h>
#include <render/shaders/Compiler.h>
#include <render/backend/Driver.h>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <glm/gtc/matrix_transform.hpp>

#include <cassert>
#include <iostream>

struct GameState
{
	game::Entity camera;
};

struct InputState
{
	glm::vec3 move_delta;
	glm::vec2 look_delta;
	double last_cursor_x {0};
	double last_cursor_y {0};
	bool cursor_captured {false};
};

static GameState game_state;
static InputState input_state;

/*
 */
void Application::init(int window_width, int window_height)
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(window_width, window_height, "Scapes Demo v0.1.0", nullptr, nullptr);

	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, &Application::onGLFWResizeEvent);
	glfwSetKeyCallback(window, Application::onGLFWKeyEvent);
	glfwSetMouseButtonCallback(window, Application::onGLFWMouseButtonEvent);
	glfwGetCursorPos(window, &input_state.last_cursor_x, &input_state.last_cursor_y);

	file_system = new ApplicationFileSystem("assets/");
	
	driver = render::backend::Driver::create("Scapes Demo", "Scapes", render::backend::Api::VULKAN);
	compiler = render::shaders::Compiler::create(render::shaders::ShaderILType::SPIRV, file_system);

	world = game::World::create();
	initGame(world, window_width, window_height);

	resource_manager = new ResourceManager(driver, compiler, file_system);
	
	swap_chain = new SwapChain(driver);
	swap_chain->init(getNativeHandle());

	render_graph = new RenderGraph(driver, compiler, resource_manager);
	render_graph->init(window_width, window_height);

	running = true;
}

void Application::shutdown()
{
	driver->wait();

	shutdownGame(world);
	game::World::destroy(world);
	world = nullptr;

	delete resource_manager;
	resource_manager = nullptr;

	delete render_graph;
	render_graph = nullptr;

	delete swap_chain;
	swap_chain = nullptr;

	render::backend::Driver::destroy(driver);
	driver = nullptr;

	render::shaders::Compiler::destroy(compiler);
	compiler = nullptr;

	delete file_system;
	file_system = nullptr;

	glfwDestroyWindow(window);
	window = nullptr;

	running = false;
}

void Application::initGame(game::World *world, int width, int height)
{
	ecs::game::init(world);

	game::Entity camera = game::Entity(world);
	camera.addComponent<ecs::game::FPSCameraTransform>();
	camera.addComponent<ecs::game::Camera>();

	ecs::game::FPSCameraTransform &camera_transform = camera.getComponent<ecs::game::FPSCameraTransform>();
	camera_transform.up = glm::vec3(0.0f, 0.0f, 1.0f);
	camera_transform.position = glm::vec3(1.0f, 1.0f, 1.0f);
	camera_transform.horizontal_angle = 0.0f;
	camera_transform.vertical_angle = 0.0f;

	game_state.camera = camera;

	resizeGame(world, width, height);
}

void Application::resizeGame(game::World *world, int width, int height)
{
	ecs::game::Camera &camera_parameters = game_state.camera.getComponent<ecs::game::Camera>();

	const float znear = 0.1f;
	const float zfar = 1000.0f;
	const float fov = 70.0f;
	float aspect = static_cast<float>(width) / height;

	camera_parameters.main = true;
	camera_parameters.parameters = glm::vec4(znear, zfar, 1.0f / znear, 1.0f / zfar);
	camera_parameters.projection = glm::perspective(fov, aspect, znear, zfar);
	camera_parameters.projection[1][1] *= -1.0f; // Invert for Vulkan
}

void Application::shutdownGame(game::World *world)
{
	ecs::game::shutdown(world);
}

/*
 */
void Application::update(float dt)
{
	updateInput();

	const float move_speed = 8.0f;
	const float look_speed = 70.0f;

	running = !glfwWindowShouldClose(window);

	ecs::game::FPSCameraTransform &camera_transform = game_state.camera.getComponent<ecs::game::FPSCameraTransform>();

	float vertical_angle_limit = 89.9f;
	float horizontal_angle_limit = 360.0f;

	camera_transform.position += input_state.move_delta * dt * move_speed;

	camera_transform.vertical_angle = glm::clamp(camera_transform.vertical_angle + input_state.look_delta.y * dt * look_speed, -vertical_angle_limit, vertical_angle_limit);
	camera_transform.horizontal_angle += input_state.look_delta.x * dt * look_speed;
	camera_transform.horizontal_angle = glm::mod(camera_transform.horizontal_angle, horizontal_angle_limit);
}

void Application::render()
{
	render::backend::CommandBuffer *command_buffer = swap_chain->acquire();
	assert(command_buffer);

	driver->resetCommandBuffer(command_buffer);
	driver->beginCommandBuffer(command_buffer);

	const ecs::game::Camera &camera_parameters = game_state.camera.getComponent<ecs::game::Camera>();
	const ecs::game::FPSCameraTransform &camera_transform = game_state.camera.getComponent<ecs::game::FPSCameraTransform>();
	const glm::vec3 &direction = ecs::game::getDirection(camera_transform);

	RenderGraphData data;
	data.projection = camera_parameters.projection;
	data.view = glm::lookAt(camera_transform.position, camera_transform.position + direction, camera_transform.up);

	render_graph->render(command_buffer, swap_chain->getBackend(), data);

	driver->endCommandBuffer(command_buffer);
}

void Application::present()
{
	swap_chain->present();
}

/*
 */
void Application::updateInput()
{
	input_state.move_delta = glm::vec3(0.0f);
	input_state.look_delta = glm::vec2(0.0f);

	double cursor_x = 0.0f;
	double cursor_y = 0.0f;
	glfwGetCursorPos(window, &cursor_x, &cursor_y);

	if (input_state.cursor_captured)
	{
		// camera look
		input_state.look_delta.x = static_cast<float>(input_state.last_cursor_x - cursor_x);
		input_state.look_delta.y = static_cast<float>(input_state.last_cursor_y - cursor_y);

		// camera motion
		const ecs::game::FPSCameraTransform &camera_transform = game_state.camera.getComponent<ecs::game::FPSCameraTransform>();

		const glm::vec3 &forward = ecs::game::getDirection(camera_transform);
		const glm::vec3 &right = glm::cross(forward, camera_transform.up);
		const glm::vec3 &up = glm::cross(right, forward);

		glm::vec3 &move = input_state.move_delta;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			move += forward;

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			move -= forward;

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			move += right;

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			move -= right;

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			move += up;

		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			move -= up;

		if (glm::dot(move, move) > 0.0f)
			move = glm::normalize(move);
	}

	input_state.last_cursor_x = cursor_x;
	input_state.last_cursor_y = cursor_y;
}

/*
 */
void Application::onResizeEvent(int width, int height)
{
	driver->wait();
	swap_chain->recreate(getNativeHandle());
	render_graph->resize(width, height);

	resizeGame(world, width, height);
}

void Application::onKeyEvent(int key, int scancode, int action, int mods)
{
	if (!input_state.cursor_captured)
		return;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		input_state.cursor_captured = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void Application::onMouseButtonEvent(int button, int action, int mods)
{
	if (input_state.cursor_captured)
		return;

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		input_state.cursor_captured = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
}

/*
 */
void Application::onGLFWResizeEvent(GLFWwindow *window, int width, int height)
{
	if (width == 0 || height == 0)
		return;

	Application *application = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));
	assert(application != nullptr);

	application->onResizeEvent(width, height);
}

void Application::onGLFWKeyEvent(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	Application *application = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));
	assert(application != nullptr);

	application->onKeyEvent(key, scancode, action, mods);
}

void Application::onGLFWMouseButtonEvent(GLFWwindow *window, int button, int action, int mods)
{
	Application *application = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));
	assert(application != nullptr);

	application->onMouseButtonEvent(button, action, mods);
}

/*
 */
void *Application::getNativeHandle() const
{
#if defined(GLFW_EXPOSE_NATIVE_WIN32)
	return glfwGetWin32Window(window);
#else
	#error "Platform is not supported"
#endif
}
