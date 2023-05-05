#include <vector>
#include <display/openglrenderer.h>
#include <glad/glad.h>
#include <util/filemanager.h>

void configureOpenGL();
void createVertexData();
GLuint loadShader(const std::string &shaderPath, GLuint shaderType);
GLuint createProgram(std::vector<GLuint> &shaders);
std::string readTextFile(const std::string &filePath);
void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const *message, void const *user_param);

const GLuint width = 1920;
const GLuint height = 1080;
GLuint vao, vertsVbo, program;
glm::mat4 perspective(1), view(1);

struct Vert
{
	glm::vec4 position;
	glm::vec4 color;

	Vert()
	{
	}

	Vert(glm::vec4 position, glm::vec4 color)
	{
		this->position = position;
		this->color = color;
	}

	Vert(glm::vec4 position)
	{
		this->position = position;
		this->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	}
};

static const std::array<glm::vec3, 36> vertex_positions =
{
	glm::vec3
	{-0.25f,  0.25f, -0.25f},
	{-0.25f, -0.25f, -0.25f},
	{0.25f, -0.25f, -0.25f},

	{ 0.25f, -0.25f, -0.25f },
	{ 0.25f,  0.25f, -0.25f },
	{ -0.25f,  0.25f, -0.25f },

	{ 0.25f, -0.25f, -0.25f },
	{ 0.25f, -0.25f,  0.25f },
	{ 0.25f,  0.25f, -0.25f },

	{ 0.25f, -0.25f,  0.25f },
	{ 0.25f,  0.25f,  0.25f },
	{ 0.25f,  0.25f, -0.25f },

	{ 0.25f, -0.25f,  0.25f },
	{ -0.25f, -0.25f,  0.25f },
	{ 0.25f,  0.25f,  0.25f },

	{ -0.25f, -0.25f,  0.25f },
	{ -0.25f,  0.25f,  0.25f },
	{ 0.25f,  0.25f,  0.25f },

	{ -0.25f, -0.25f,  0.25f },
	{ -0.25f, -0.25f, -0.25f },
	{ -0.25f,  0.25f,  0.25f },

	{ -0.25f, -0.25f, -0.25f },
	{ -0.25f,  0.25f, -0.25f },
	{ -0.25f,  0.25f,  0.25f },

	{ -0.25f, -0.25f,  0.25f },
	{ 0.25f, -0.25f,  0.25f },
	{ 0.25f, -0.25f, -0.25f },

	{ 0.25f, -0.25f, -0.25f },
	{ -0.25f, -0.25f, -0.25f },
	{ -0.25f, -0.25f,  0.25f },

	{ -0.25f,  0.25f, -0.25f },
	{ 0.25f,  0.25f, -0.25f },
	{ 0.25f,  0.25f,  0.25f },

	{ 0.25f,  0.25f,  0.25f },
	{ -0.25f,  0.25f,  0.25f },
	{ -0.25f,  0.25f, -0.25f }
};

std::vector<Vert> verts;

Boiler::OpenGLRenderer::OpenGLRenderer() : Renderer("OpenGL Renderer", 1)
{
}

Boiler::OpenGLRenderer::~OpenGLRenderer()
{
}

void Boiler::OpenGLRenderer::initialize(const Boiler::Size &size)
{
	Renderer::initialize(size);
	resize(size);

	std::vector<GLuint> shaders;
	shaders.push_back(loadShader("shaders/gl/shader.vert", GL_VERTEX_SHADER));
	shaders.push_back(loadShader("shaders/gl/shader.frag", GL_FRAGMENT_SHADER));
	program = createProgram(shaders);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEBUG_OUTPUT);

	glDebugMessageCallback(message_callback, nullptr);

	for (auto v : vertex_positions)
	{
		verts.push_back(Vert(glm::vec4(v, 1), glm::vec4(1, 1, 1, 1)));
	}

	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glCreateBuffers(1, &vertsVbo);
	glNamedBufferStorage(vertsVbo, sizeof(Vert) * verts.size(), verts.data(), 0);

	// setup position attribute
	glEnableVertexArrayAttrib(vao, 0);
	glVertexArrayAttribFormat(vao, 0, 4, GL_FLOAT, GL_FALSE, offsetof(Vert, position));
	glVertexArrayAttribBinding(vao, 0, 0);

	// setup color attribute
	glEnableVertexArrayAttrib(vao, 1);
	glVertexArrayAttribFormat(vao, 1, 4, GL_FLOAT, GL_FALSE, offsetof(Vert, color));
	glVertexArrayAttribBinding(vao, 1, 0);

	// single buffer binding
	glVertexArrayVertexBuffer(vao, 0, vertsVbo, 0, sizeof(Vert));
}

void Boiler::OpenGLRenderer::shutdown()
{
	glDeleteBuffers(1, &vertsVbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(program);
}

void Boiler::OpenGLRenderer::prepareShutdown()
{
}

void Boiler::OpenGLRenderer::resize(const Boiler::Size &size)
{
	Renderer::resize(size);
	glViewport(0, 0, size.width, size.height);
	perspective = glm::perspective(45.0f, static_cast<float>(size.width) / size.height, 0.1f, 1000.0f);
}

std::string Boiler::OpenGLRenderer::getVersion() const
{
	return "1.0";
}

Boiler::AssetId Boiler::OpenGLRenderer::loadTexture(const Boiler::ImageData &imageData, Boiler::TextureType type)
{
	return 0;
}

Boiler::AssetId Boiler::OpenGLRenderer::loadCubemap(const std::array<ImageData, 6> &images)
{
	return 0;
}

Boiler::AssetId Boiler::OpenGLRenderer::loadPrimitive(const Boiler::VertexData &data, Boiler::AssetId existingId)
{
	return 0;
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

	const GLfloat clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearBufferfv(GL_COLOR, 0, clearColor);

	glUseProgram(program);
	float f = (float)frameInfo.globalTime * glm::pi<float>() * 0.2f;

	glm::vec3 eye(0, 0, fabs(sinf(f)) - 2.0f);
	view = glm::lookAt(eye, eye + glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));

	glm::mat4 modelMat = glm::translate(glm::mat4(1), glm::vec3(0, 0, 0)) *
		glm::scale(glm::mat4(1), glm::vec3(1, 1, 1)) *
		glm::rotate(glm::mat4(1), f, glm::vec3(0, 1, 0));

	glm::mat4 mv = perspective * view * modelMat;

	glUniformMatrix4fv(2, 1, GL_FALSE, &mv[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, (GLuint)verts.size());
}

void Boiler::OpenGLRenderer::displayFrame(const Boiler::FrameInfo &frameInfo, Boiler::AssetSet &assetSet)
{
}

GLuint loadShader(const std::string &shaderPath, GLuint shaderType)
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
		std::cout << infoLog.data() << std::endl;

		glDeleteShader(shader);
		exit(EXIT_FAILURE);
	}

	return shader;
}

GLuint createProgram(std::vector<GLuint> &shaders)
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
		std::cout << infoLog.data() << std::endl;

		glDeleteProgram(program);
		exit(EXIT_FAILURE);
	}

	for (GLuint shader : shaders)
	{
		glDeleteShader(shader);
	}

	return program;
}

void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const *message, void const *user_param)
{
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
	std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id << ": " << message << '\n';
}
