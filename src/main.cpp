/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#include <iostream>
#include "vkc/vkc.h"

auto main() -> int
{
    try
    {
        vkc::InitVulkan();

        vkc::Instance instance("VulkanCube", "None", true);

        vkc::Window win({800, 800}, "VulkanCube", instance);
        vkc::Device device(instance, win.getSurface(), vkc::Instance::DeviceExtensions);

        vkc::SwapChain swapChain(device, win);

        win.mainLoop();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    vkc::CleanupVulkan();
     return EXIT_SUCCESS;
 }