#include "RenderGraph.h"

#include <render/backend/Driver.h>

/*
 */
RenderGraph::~RenderGraph()
{
	shutdown();
}

/*
 */
void RenderGraph::init()
{
	pipeline_state = driver->createPipelineState();
}

void RenderGraph::shutdown()
{
	driver->destroyPipelineState(pipeline_state);
	pipeline_state = nullptr;

	driver->destroyRenderPass(swap_chain_render_pass);
	swap_chain_render_pass = nullptr;
}

/*
 */
void RenderGraph::render(render::backend::CommandBuffer *command_buffer, render::backend::SwapChain *swap_chain)
{
	if (swap_chain_render_pass == nullptr)
	{
		render::backend::RenderPassClearColor clear_color = {0.2f, 0.0f, 0.0f, 1.0f};
		swap_chain_render_pass = driver->createRenderPass(swap_chain, render::backend::RenderPassLoadOp::CLEAR, render::backend::RenderPassStoreOp::STORE, &clear_color);
	}

	driver->beginRenderPass(command_buffer, swap_chain_render_pass, swap_chain);

	driver->endRenderPass(command_buffer);
}
