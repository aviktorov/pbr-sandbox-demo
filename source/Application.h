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

namespace game
{
	class World;
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
	void initGame(game::World *world, int width, int height);
	void resizeGame(game::World *world, int width, int height);
	void shutdownGame(game::World *world);

	void updateInput();

private:
	void onResizeEvent(int width, int height);
	void onKeyEvent(int key, int scancode, int action, int mods);
	void onMouseButtonEvent(int button, int action, int mods);

private:
	static void onGLFWResizeEvent(GLFWwindow *window, int width, int height);
	static void onGLFWKeyEvent(GLFWwindow *window, int key, int scancode, int action, int mods);
	static void onGLFWMouseButtonEvent(GLFWwindow *window, int button, int action, int mods);

private:
	GLFWwindow *window {nullptr};
	bool running {false};

	ApplicationFileSystem *file_system {nullptr};
	SwapChain *swap_chain {nullptr};
	RenderGraph *render_graph {nullptr};
	ResourceManager *resource_manager {nullptr};

	game::World *world {nullptr};
	render::backend::Driver *driver {nullptr};
	render::shaders::Compiler *compiler {nullptr};
};
