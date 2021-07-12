#pragma once

#include <cstdint>

namespace render::backend
{
	class Driver;
	struct SwapChain;
	struct CommandBuffer;
}

/*
 */
class SwapChain
{
public:
	SwapChain(render::backend::Driver *driver);
	virtual ~SwapChain();

	void init(void *native_window);
	void recreate(void *native_window);
	void shutdown();

	render::backend::CommandBuffer *acquire();
	bool present();

	inline render::backend::SwapChain *getBackend() { return swap_chain; }

private:
	void initFrames();
	void shutdownFrames();

private:
	enum
	{
		NUM_IN_FLIGHT_FRAMES = 1,
	};

	render::backend::Driver *driver {nullptr};
	render::backend::SwapChain *swap_chain {nullptr};

	uint32_t current_in_flight_frame {0};
	render::backend::CommandBuffer *command_buffers[NUM_IN_FLIGHT_FRAMES];
};
