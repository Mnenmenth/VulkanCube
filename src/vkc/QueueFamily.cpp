/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#include "QueueFamily.h"
#include <vector>
#include <iostream>

auto vkc::QueueFamily::FindQueueFamilies(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) -> vkc::QueueFamilyIndices
{
    QueueFamilyIndices indices;

    // Get queue families
    type::uint32 familyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);
    std::vector<VkQueueFamilyProperties> families(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, families.data());

    // Iterate through families until one that supports requirements is found
    bool found = false;
    for(int i = 0; !found && i < families.size(); ++i)
    {
        const auto& family = families[i];

        // Check for graphics support
        if(family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphics = i;
        }

        // Check for surface presentation support
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if(presentSupport)
        {
            indices.present = i;
        }

        found = indices.isComplete();
    }

    return indices;
}
