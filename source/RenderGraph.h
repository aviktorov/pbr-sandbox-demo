#pragma once

namespace render::backend
{
	class Driver;
	struct CommandBuffer;
	struct PipelineState;
	struct RenderPass;
	struct SwapChain;
}

namespace render::shaders
{
	class Compiler;
}

/*
 */
class RenderGraph
{
public:
	RenderGraph(render::backend::Driver *driver, render::shaders::Compiler *compiler)
		: driver(driver), compiler(compiler) { }

	virtual ~RenderGraph();

	void init();
	void shutdown();
	void render(render::backend::CommandBuffer *command_buffer, render::backend::SwapChain *swap_chain);

private:
	render::backend::Driver *driver {nullptr};
	render::shaders::Compiler *compiler {nullptr};

	render::backend::PipelineState *pipeline_state {nullptr};
	render::backend::RenderPass *swap_chain_render_pass {nullptr};
};
