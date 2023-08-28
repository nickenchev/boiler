// Includes of OpenGL headers for various platforms

#ifdef __APPLE__

#include <GL/glew.h>
#include <OpenGL/gl.h>

#elif __linux__

#include <GL/glew.h>
#include <GL/gl.h>

#elif _WIN32


#else

#include <GLES3/gl3.h>

#endif
