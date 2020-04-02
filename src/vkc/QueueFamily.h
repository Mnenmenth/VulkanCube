/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#ifndef VULKANCUBE_QUEUEFAMILY_H
#define VULKANCUBE_QUEUEFAMILY_H

#include <vulkan/vulkan.h>
#include <optional>
#include "Types.h"
#include "QueueFamilyIndices.h"

namespace vkc
{
    class QueueFamily
    {
    public:
        QueueFamily() = delete;
        ~QueueFamily() = delete;


        static auto FindQueueFamilies(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) -> vkc::QueueFamilyIndices;
    };
}


#endif //VULKANCUBE_QUEUEFAMILY_H
