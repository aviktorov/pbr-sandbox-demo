#pragma once

#include <cstdint>

namespace render::backend
{
	class Driver;

	struct CommandBuffer;
	struct IndexBuffer;
	struct PipelineState;
	struct RenderPass;
	struct Shader;
	struct SwapChain;
	struct VertexBuffer;
}

namespace render::shaders
{
	class Compiler;
}

class ResourceManager;

/*
 */
class RenderGraph
{
public:
	RenderGraph(render::backend::Driver *driver, render::shaders::Compiler *compiler, ResourceManager *resource_manager)
		: driver(driver), compiler(compiler), resource_manager(resource_manager) { }

	virtual ~RenderGraph();

	void init(uint32_t width, uint32_t height);
	void resize(uint32_t width, uint32_t height);
	void shutdown();
	void render(render::backend::CommandBuffer *command_buffer, render::backend::SwapChain *swap_chain);

private:
	ResourceManager *resource_manager {nullptr};

	render::backend::Driver *driver {nullptr};
	render::shaders::Compiler *compiler {nullptr};

	render::backend::PipelineState *pipeline_state {nullptr};
	render::backend::RenderPass *swap_chain_render_pass {nullptr};

	render::backend::VertexBuffer *vertex_buffer {nullptr};
	render::backend::IndexBuffer *index_buffer {nullptr};

	render::backend::Shader *fragment_shader {nullptr};
	render::backend::Shader *vertex_shader {nullptr};
};
