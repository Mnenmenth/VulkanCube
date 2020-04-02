/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#ifndef VULKANCUBE_INIT_H
#define VULKANCUBE_INIT_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace vkc
{
    auto InitVulkan() -> void
    {
        glfwInit();
        // Disable OpenGL
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }
}

#endif //VULKANCUBE_INIT_H
