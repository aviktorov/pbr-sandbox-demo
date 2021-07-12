#include "Application.h"
#include <GLFW/glfw3.h>

#include <chrono>

/*
 */
int main(void)
{
	if (!glfwInit())
		return -1;

	Application app;
	app.init(800, 600);

	auto last_frame_time = std::chrono::high_resolution_clock::now();
	while (app.isRunning())
	{
		auto current_frame_time = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::seconds::period>(current_frame_time - last_frame_time).count();

		app.update(dt);
		app.render();
		app.present();

		last_frame_time = current_frame_time;

		glfwPollEvents();
	}

	app.shutdown();

	glfwTerminate();
	return 0;
}
