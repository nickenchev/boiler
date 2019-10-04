// Includes of Vulkan headers for various platforms

#ifdef __APPLE__

#define VK_USE_PLATFORM_MACOS_MVK
#include <vulkan/vulkan.h>

#elif __linux__

#define VK_USE_PLATFORM_XCB_KHR
#include <vulkan/vulkan.h>

#elif _WIN32


#else


#endif
