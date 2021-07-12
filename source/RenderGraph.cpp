#include "RenderGraph.h"
#include "ResourceManager.h"

#include <render/backend/Driver.h>
#include <render/shaders/Compiler.h>

/*
 */
struct GPUVertex
{
	float position[2];
	float color[4];
};

static uint32_t num_vertices = 3;
static GPUVertex vertices[] =
{
	{ -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
	{  1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f },
	{  0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
};

static uint8_t num_attributes = 2;
static render::backend::VertexAttribute attributes[] =
{
	{ render::backend::Format::R32G32_SFLOAT, offsetof(GPUVertex, position) },
	{ render::backend::Format::R32G32B32A32_SFLOAT, offsetof(GPUVertex, color) },
};

static uint32_t num_indices = 3;
static uint16_t indices[] =
{
	0, 1, 2,
};

/*
 */
RenderGraph::~RenderGraph()
{
	shutdown();
}

/*
 */
void RenderGraph::init(uint32_t width, uint32_t height)
{
	pipeline_state = driver->createPipelineState();

	vertex_shader = resource_manager->fetchShader(render::backend::ShaderType::VERTEX, "shaders/default.vert");
	fragment_shader = resource_manager->fetchShader(render::backend::ShaderType::FRAGMENT, "shaders/default.frag");

	vertex_buffer = driver->createVertexBuffer(render::backend::BufferType::STATIC, sizeof(GPUVertex), num_vertices, num_attributes, attributes, vertices);
	index_buffer = driver->createIndexBuffer(render::backend::BufferType::STATIC, render::backend::IndexFormat::UINT16, num_indices, indices);

	driver->setViewport(pipeline_state, 0, 0, width, height);
	driver->setScissor(pipeline_state, 0, 0, width, height);

	driver->clearShaders(pipeline_state);
	driver->setShader(pipeline_state, render::backend::ShaderType::VERTEX, vertex_shader);
	driver->setShader(pipeline_state, render::backend::ShaderType::FRAGMENT, fragment_shader);
	driver->setCullMode(pipeline_state, render::backend::CullMode::NONE);

	driver->clearVertexStreams(pipeline_state);
	driver->setVertexStream(pipeline_state, 0, vertex_buffer);
}

void RenderGraph::resize(uint32_t width, uint32_t height)
{
	driver->setViewport(pipeline_state, 0, 0, width, height);
	driver->setScissor(pipeline_state, 0, 0, width, height);
}

void RenderGraph::shutdown()
{
	driver->destroyPipelineState(pipeline_state);
	pipeline_state = nullptr;

	driver->destroyRenderPass(swap_chain_render_pass);
	swap_chain_render_pass = nullptr;

	driver->destroyVertexBuffer(vertex_buffer);
	driver->destroyIndexBuffer(index_buffer);

	vertex_shader = nullptr;
	fragment_shader = nullptr;
}

void RenderGraph::render(render::backend::CommandBuffer *command_buffer, render::backend::SwapChain *swap_chain)
{
	if (swap_chain_render_pass == nullptr)
	{
		render::backend::RenderPassClearColor clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
		swap_chain_render_pass = driver->createRenderPass(swap_chain, render::backend::RenderPassLoadOp::CLEAR, render::backend::RenderPassStoreOp::STORE, &clear_color);
	}

	driver->beginRenderPass(command_buffer, swap_chain_render_pass, swap_chain);
	driver->drawIndexedPrimitiveInstanced(command_buffer, pipeline_state, index_buffer, num_indices);
	driver->endRenderPass(command_buffer);
}
