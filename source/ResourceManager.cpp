#include "ResourceManager.h"

#include <string_view>
#include <cassert>

/*
 */
static size_t hash_cstr(const char *s)
{
	return std::hash<std::string_view>()(std::string_view(s, std::strlen(s)));
}

/*
 */
ResourceManager::~ResourceManager()
{
	for (auto &it : shaders)
		driver->destroyShader(it.second);
}

/*
 */
render::backend::Shader *ResourceManager::fetchShader(render::backend::ShaderType type, const char *path)
{
	size_t id = hash_cstr(path);

	auto it = shaders.find(id);
	if (it != shaders.end())
		return it->second;

	io::IStream *stream = file_system->open(path, "rb");
	if (stream == nullptr)
		return nullptr;

	size_t size = static_cast<size_t>(stream->size());

	char *data = new char[size];
	size_t read_count = stream->read(data, sizeof(char), size);
	assert(read_count == size);

	file_system->close(stream);

	render::shaders::ShaderIL *il = compiler->createShaderIL(static_cast<render::shaders::ShaderType>(type), static_cast<uint32_t>(size), data, path);
	delete[] data;

	if (il == nullptr)
	{
		compiler->destroyShaderIL(il);
		return nullptr;
	}

	render::backend::Shader *shader = driver->createShaderFromIL(type, static_cast<render::backend::ShaderILType>(il->il_type), il->bytecode_size, il->bytecode_data);
	compiler->destroyShaderIL(il);

	if (shader == nullptr)
		return nullptr;

	shaders.insert(std::make_pair(id, shader));
	return shader;
}
