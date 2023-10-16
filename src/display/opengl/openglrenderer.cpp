#include <vector>
#include <display/opengl/openglrenderer.h>
#include <util/filemanager.h>
#include <display/imaging/imagedata.h>

using namespace Boiler;

void messageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const *message, void const *user_param);

GLuint program;
glm::mat4 perspective(1), view(1);

Boiler::OpenGLRenderer::OpenGLRenderer() : Renderer("OpenGL Renderer", 1)
{
}

Boiler::OpenGLRenderer::~OpenGLRenderer()
{
}

void Boiler::OpenGLRenderer::initialize(const Boiler::Size &size)
{
	logger.log("Initializing Boiler OpenGL Renderer");
	Renderer::initialize(size);
	resize(size);

	std::vector<GLuint> shaders;
	logger.log("Creating shaders");
	shaders.push_back(loadShader("shaders/gl/shader.vert", GL_VERTEX_SHADER));
	shaders.push_back(loadShader("shaders/gl/gltut.frag", GL_FRAGMENT_SHADER));
	program = createProgram(shaders);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEBUG_OUTPUT);

	glDebugMessageCallback(messageCallback, &logger);
}

void Boiler::OpenGLRenderer::shutdown()
{
	logger.log("Deleting primitives");
	for (int i = 0; i < primitives.getSize(); ++i)
	{
		if (primitives.isOccupied(i))
		{
			const Primitive prim = primitives[i];
			const PrimitiveBuffers pb = primitiveBuffers[prim.bufferId];
			// vertex and index buffer
			std::array<GLuint, 2> buffers = { pb.getVertexBuffer(), pb.getIndexBuffer() };
			glDeleteBuffers(buffers.size(), buffers.data());

			// VBO
			std::array<GLuint, 1> vbos = { pb.getVertexArrayObject() };
			glDeleteVertexArrays(vbos.size(), vbos.data());

		}
	}

	logger.log("Deleting textures");
	for (int i = 0; i < textures.getSize(); ++i)
	{
		if (textures.isOccupied(i))
		{
			const OpenGLTexture &texture = textures[i];
			const GLuint oglTex = texture.getOpenGLTextureId();
			glDeleteTextures(1, &oglTex);
		}
	}

	glDeleteProgram(program);
}

void Boiler::OpenGLRenderer::prepareShutdown()
{
}

void Boiler::OpenGLRenderer::resize(const Boiler::Size &size)
{
	Renderer::resize(size);
	glViewport(0, 0, static_cast<int>(size.width), static_cast<int>(size.height));
	perspective = glm::perspective(45.0f, size.width / size.height, 0.1f, 1000.0f);
	logger.log("Resized renderer to {}x{}", size.width, size.height);
}

std::string Boiler::OpenGLRenderer::getVersion() const
{
	return "OpenGL Renderer 1.0";
}

Boiler::AssetId Boiler::OpenGLRenderer::loadTexture(const Boiler::ImageData &imageData, Boiler::TextureType type)
{
	const unsigned short numMipMaps = 4;
	GLuint texture;
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);
	glTextureStorage2D(texture, numMipMaps, GL_SRGB8, imageData.size.width, imageData.size.height);
	glTextureSubImage2D(texture, 0, 0, 0, imageData.size.width, imageData.size.height, GL_RGBA, GL_UNSIGNED_BYTE, imageData.pixelData);
	glGenerateTextureMipmap(texture);

	AssetId texturerAssetId = textures.add(OpenGLTexture(texture));
	return texturerAssetId;
}

Boiler::AssetId Boiler::OpenGLRenderer::loadCubemap(const std::array<ImageData, 6> &images)
{
	return 0;
}

Boiler::AssetId Boiler::OpenGLRenderer::loadPrimitive(const Boiler::VertexData &data, Boiler::AssetId existingId)
{
	GLuint vao, buffers[2];
	glCreateVertexArrays(1, &vao);

	// create index and vertex buffers
	glCreateBuffers(2, buffers);
	glNamedBufferStorage(buffers[0], data.vertexByteSize(), data.vertexBegin(), 0);
	glNamedBufferStorage(buffers[1], data.indexByteSize(), data.indexBegin(), 0);

	// bind buffers
	glVertexArrayVertexBuffer(vao, 0, buffers[0], 0, sizeof(Vertex));
	glVertexArrayElementBuffer(vao, buffers[1]);

	// setup array attributes and their associated buffers
	glEnableVertexArrayAttrib(vao, 0);
	glVertexArrayAttribFormat(vao, 0, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
	glVertexArrayAttribBinding(vao, 0, 0);

	glEnableVertexArrayAttrib(vao, 1);
	glVertexArrayAttribFormat(vao, 1, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex, colour));
	glVertexArrayAttribBinding(vao, 1, 0);

	glEnableVertexArrayAttrib(vao, 2);
	glVertexArrayAttribFormat(vao, 2, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex, textureCoordinates));
	glVertexArrayAttribBinding(vao, 2, 0);

	glEnableVertexArrayAttrib(vao, 3);
	glVertexArrayAttribFormat(vao, 3, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
	glVertexArrayAttribBinding(vao, 3, 0);

	// return the asset ID grouping these buffers
	logger.log("Loaded primitive with vertex data size: {} bytes", data.vertexByteSize());
	return primitiveBuffers.add(PrimitiveBuffers(vao, buffers[0], buffers[1]));
}

Boiler::AssetId Boiler::OpenGLRenderer::createBuffer(size_t size, Boiler::BufferUsage usage, Boiler::MemoryType memType)
{
	return 0;
}

void Boiler::OpenGLRenderer::updateLights(const std::vector<LightSource> &lightSources) const
{

}

void Boiler::OpenGLRenderer::updateMaterials(const std::vector<ShaderMaterial> &materials) const
{
}

bool Boiler::OpenGLRenderer::prepareFrame(const Boiler::FrameInfo &frameInfo)
{
	if (Renderer::prepareFrame(frameInfo))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return true;
	}
	return false;
}

void Boiler::OpenGLRenderer::render(Boiler::AssetSet &assetSet, const Boiler::FrameInfo &frameInfo,
									const std::vector<MaterialGroup> &materialGroups, Boiler::RenderStage stage)
{
	glUseProgram(program);

	GLint uniformLocationMvp = glGetUniformLocation(program, "mvp");
	GLint uniformLocationModelMatrix = glGetUniformLocation(program, "modelMatrix");
	GLint uniformLocationViewMatrix = glGetUniformLocation(program, "viewMatrix");
	GLint uniformLocationCameraPosition = glGetUniformLocation(program, "cameraPosition");

	if (stage == RenderStage::PRE_DEFERRED_LIGHTING)
	{
		for (const MaterialGroup &matGroup : materialGroups)
		{
			if (matGroup.materialId != Asset::NO_ASSET)
			{
				const Material &material = assetSet.materials.get(matGroup.materialId);
				if (material.albedoTexture != Asset::NO_ASSET)
				{
					if (material.albedoTexture != Asset::NO_ASSET)
					{
						const OpenGLTexture &texture = textures.get(material.albedoTexture);
						glBindTextureUnit(0, texture.getOpenGLTextureId());
					}
					if (material.normalTexture != Asset::NO_ASSET)
					{
						const OpenGLTexture &texture = textures.get(material.normalTexture);
						glBindTextureUnit(1, texture.getOpenGLTextureId());
					}

					for (const auto &primitiveInstance : matGroup.primitives)
					{
						const Primitive &primitive = assetSet.primitives.get(primitiveInstance.primitiveId);
						const PrimitiveBuffers &buffers = primitiveBuffers.get(primitive.bufferId);

						glm::mat4 matrix = matrices.get(primitiveInstance.matrixId);
						glm::mat4 mvp = perspective * viewMatrix * matrix;

						glUniformMatrix4fv(uniformLocationMvp, 1, GL_FALSE, &mvp[0][0]);
						glUniformMatrix4fv(uniformLocationModelMatrix, 1, GL_FALSE, &matrix[0][0]);

						if (uniformLocationViewMatrix != -1)
						{
							glUniformMatrix4fv(uniformLocationViewMatrix, 1, GL_FALSE, &viewMatrix[0][0]);
						}

						if (uniformLocationCameraPosition != -1)
						{
							glUniform3fv(uniformLocationCameraPosition, 1, &cameraPosition[0]);
						}

						glBindVertexArray(buffers.getVertexArrayObject());
						glDrawElements(GL_TRIANGLES, primitive.indexCount(), GL_UNSIGNED_INT, nullptr);
					}
				}
			}
		}
	}
}

void Boiler::OpenGLRenderer::displayFrame(const Boiler::FrameInfo &frameInfo, Boiler::AssetSet &assetSet)
{
}

GLuint OpenGLRenderer::loadShader(const std::string &shaderPath, GLuint shaderType)
{
	std::string source = Boiler::FileManager::readTextFile(shaderPath);
	GLuint shader = glCreateShader(shaderType);
	const GLchar *srcPtr = source.c_str();
	glShaderSource(shader, 1, &srcPtr, nullptr);
	glCompileShader(shader);

	GLint compileStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus != GL_TRUE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, infoLog.data());
		logger.error("{}", infoLog.data());

		glDeleteShader(shader);
		exit(EXIT_FAILURE);
	}

	return shader;
}

GLuint OpenGLRenderer::createProgram(std::vector<GLuint> &shaders)
{
	GLuint program = glCreateProgram();
	for (GLuint shader : shaders)
	{
		glAttachShader(program, shader);
	}
	glLinkProgram(program);

	GLint linkStatus;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
		logger.error("{}", infoLog.data());

		glDeleteProgram(program);
		exit(EXIT_FAILURE);
	}

	for (GLuint shader : shaders)
	{
		glDeleteShader(shader);
	}

	return program;
}

void messageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const *message, void const *userParam)
{
	const Logger *logger = static_cast<const Logger *>(userParam);
	auto const src_str = [source]() {
		switch (source)
		{
			case GL_DEBUG_SOURCE_API: return "API";
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
			case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
			case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
			case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
			case GL_DEBUG_SOURCE_OTHER:
			default: return "OTHER";
		}
	}();

	auto const type_str = [type]() {
		switch (type)
		{
			case GL_DEBUG_TYPE_ERROR: return "ERROR";
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
			case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
			case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
			case GL_DEBUG_TYPE_MARKER: return "MARKER";
			case GL_DEBUG_TYPE_OTHER:
			default:return "OTHER";
		}
	}();

	auto const severity_str = [severity]() {
		switch (severity) {
			case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
			case GL_DEBUG_SEVERITY_LOW: return "LOW";
			case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
			case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
			default: return "UNKNOWN";
		}
	}();
	logger->error("{} {} {}, {}: {}", src_str, type_str, severity_str, id, message);
}
