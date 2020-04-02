/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#ifndef VULKANCUBE_SWAPCHAINSUPPORTDETAILS_H
#define VULKANCUBE_SWAPCHAINSUPPORTDETAILS_H

#include <vulkan/vulkan.h>

namespace vkc
{
    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };
}

#endif //VULKANCUBE_SWAPCHAINSUPPORTDETAILS_H
