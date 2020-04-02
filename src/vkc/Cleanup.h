/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#ifndef VULKANCUBE_CLEANUP_H
#define VULKANCUBE_CLEANUP_H

#include <GLFW/glfw3.h>

namespace vkc
{
    auto CleanupVulkan() -> void
    {
        glfwTerminate();
    }
}

#endif //VULKANCUBE_CLEANUP_H
