#pragma once

class ApplicationFileSystem;
class SwapChain;
class RenderGraph;
class ResourceManager;

struct GLFWwindow;

namespace render::backend
{
	class Driver;
}

namespace render::shaders
{
	class Compiler;
}

/*
 */
class Application
{
public:
	void init(int window_width, int window_height);
	void shutdown();
	void update(float dt);
	void render();
	void present();

	inline bool isRunning() const { return running; }

	void *getNativeHandle() const;

private:
	static void onFramebufferResize(GLFWwindow *window, int width, int height);

private:
	GLFWwindow *window {nullptr};
	bool running {false};

	ApplicationFileSystem *file_system {nullptr};
	SwapChain *swap_chain {nullptr};
	RenderGraph *render_graph {nullptr};
	ResourceManager *resource_manager {nullptr};

	render::backend::Driver *driver {nullptr};
	render::shaders::Compiler *compiler {nullptr};
};
