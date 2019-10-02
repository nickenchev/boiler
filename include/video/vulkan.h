// Includes of Vulkan headers for various platforms

#ifdef __APPLE__


#elif __linux__

#define VK_USE_PLATFORM_XCB_KHR
#include <vulkan/vulkan.h>

#elif _WIN32


#else


#endif
