#include "Application.h"
#include "IO.h"
#include "RenderGraph.h"
#include "SwapChain.h"

#include <render/shaders/Compiler.h>
#include <render/backend/Driver.h>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <cassert>

/*
 */
void Application::init(int window_width, int window_height)
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(window_width, window_height, "Scapes v1.0", nullptr, nullptr);

	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, &Application::onFramebufferResize);

	file_system = new ApplicationFileSystem("assets/");

	driver = render::backend::Driver::create("PBR Sandbox", "Scape", render::backend::Api::VULKAN);
	compiler = render::shaders::Compiler::create(render::shaders::ShaderILType::SPIRV, file_system);

	swap_chain = new SwapChain(driver);
	swap_chain->init(getNativeHandle());

	render_graph = new RenderGraph(driver, compiler);

	running = true;
}

void Application::shutdown()
{
	delete render_graph;
	render_graph = nullptr;

	delete swap_chain;
	swap_chain = nullptr;

	delete driver;
	driver = nullptr;

	delete compiler;
	compiler = nullptr;

	delete file_system;
	file_system = nullptr;

	glfwDestroyWindow(window);
	window = nullptr;

	running = false;
}

/*
 */
void Application::update(float dt)
{
	running = !glfwWindowShouldClose(window);
}

void Application::render()
{
	render::backend::CommandBuffer *command_buffer = swap_chain->acquire();
	assert(command_buffer);

	driver->resetCommandBuffer(command_buffer);
	driver->beginCommandBuffer(command_buffer);

	render_graph->render(command_buffer, swap_chain->getBackend());

	driver->endCommandBuffer(command_buffer);
}

void Application::present()
{
	swap_chain->present();
}

/*
 */
void Application::onFramebufferResize(GLFWwindow *window, int width, int height)
{
	if (width == 0 || height == 0)
		return;

	Application *application = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));
	assert(application != nullptr);

	application->driver->wait();
	application->swap_chain->recreate(application->getNativeHandle());
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