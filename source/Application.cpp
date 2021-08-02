#include "Application.h"
#include "IO.h"
#include "RenderGraph.h"
#include "ResourceManager.h"
#include "SwapChain.h"
#include "GameModule.h"

#include <render/shaders/Compiler.h>
#include <render/backend/Driver.h>
#include <game/World.h>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <glm/gtc/matrix_transform.hpp>

#include <cassert>

struct GameState
{
	game::Entity camera;
};

static GameState game_state;

/*
 */
void Application::init(int window_width, int window_height)
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(window_width, window_height, "Scapes Demo v0.1.0", nullptr, nullptr);

	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, &Application::onFramebufferResize);
	glfwSetKeyCallback(window, Application::onKeyEvent);

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
	camera.addComponent<ecs::game::Transform>();
	camera.addComponent<ecs::game::Camera>();

	// ref is invalid

	game_state.camera = camera;

	resizeGame(world, width, height);
}

void Application::resizeGame(game::World *world, int width, int height)
{
	ecs::game::Camera &camera_parameters = game_state.camera.getComponent<ecs::game::Camera>();

	const float znear = 0.1f;
	const float zfar = 1000.0f;
	const float fov = 90.0f;
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
	static float angle = 0.0f;
	static float radius = 2.0f;
	static float offset = 2.0f;
	angle += dt;

	running = !glfwWindowShouldClose(window);

	ecs::game::Transform &camera_transform = game_state.camera.getComponent<ecs::game::Transform>();
	camera_transform.world = glm::lookAt(glm::vec3(cos(angle) * radius, sin(angle) * radius, offset), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	camera_transform.iworld = glm::inverse(camera_transform.world);
}

void Application::render()
{
	render::backend::CommandBuffer *command_buffer = swap_chain->acquire();
	assert(command_buffer);

	driver->resetCommandBuffer(command_buffer);
	driver->beginCommandBuffer(command_buffer);

	const ecs::game::Camera &camera_parameters = game_state.camera.getComponent<ecs::game::Camera>();
	const ecs::game::Transform &camera_transform = game_state.camera.getComponent<ecs::game::Transform>();

	RenderGraphData data;
	data.projection = camera_parameters.projection;
	data.view = camera_transform.world;

	render_graph->render(command_buffer, swap_chain->getBackend(), data);

	driver->endCommandBuffer(command_buffer);
}

void Application::present()
{
	swap_chain->present();
}

/*
 */
void Application::onKeyEvent(GLFWwindow *window, int key, int scancode, int action, int mods)
{

}

void Application::onFramebufferResize(GLFWwindow *window, int width, int height)
{
	if (width == 0 || height == 0)
		return;

	Application *application = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));
	assert(application != nullptr);

	application->driver->wait();
	application->swap_chain->recreate(application->getNativeHandle());
	application->render_graph->resize(width, height);

	application->resizeGame(application->world, width, height);
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
