#pragma once

#include <unordered_map>
#include <render/backend/Driver.h>
#include <render/shaders/Compiler.h>
#include <common/IO.h>

/*
 */
class ResourceManager
{
public:
	ResourceManager(render::backend::Driver *driver, render::shaders::Compiler *compiler, io::IFileSystem *file_system)
		: driver(driver), compiler(compiler), file_system(file_system) {}

	virtual ~ResourceManager();

	render::backend::Shader *fetchShader(render::backend::ShaderType type, const char *path);

private:
	render::backend::Driver *driver {nullptr};
	render::shaders::Compiler *compiler {nullptr};
	io::IFileSystem *file_system {nullptr};

	std::unordered_map<size_t, render::backend::Shader *> shaders;
};
