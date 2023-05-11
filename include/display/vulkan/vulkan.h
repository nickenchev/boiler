// Includes of Vulkan headers for various platforms

#ifdef __APPLE__

#define VK_USE_PLATFORM_MACOS_MVK

#elif __linux__

#define VK_USE_PLATFORM_XCB_KHR

#elif _WIN32

#else


#endif

#include <vulkan/vulkan.h>
