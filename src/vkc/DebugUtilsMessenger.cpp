
/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#include <iostream>
#include "DebugUtilsMessenger.h"
#include "Instance.h"

vkc::DebugUtilsMessenger::DebugUtilsMessenger(const vkc::Instance& instance) : m_messenger(VK_NULL_HANDLE), m_instance(instance)
{
    if(m_instance.validationLayersEnabled())
    {
        // Setup the debug messenger
        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        PopulateCreateInfo(createInfo);

        // Get pointer to extension function
        auto createFunc = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(m_instance.getHandle(), "vkCreateDebugUtilsMessengerEXT");

        if(createFunc == nullptr)
        {
            throw std::runtime_error("Debug Utils Messenger extension not available");
        }

        // Create the messenger
        VkResult result = createFunc(m_instance.getHandle(), &createInfo, nullptr, &m_messenger);

        if(result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to setup Debug Utils Messenger");
        }
    }
}

vkc::DebugUtilsMessenger::~DebugUtilsMessenger()
{
    if(m_instance.validationLayersEnabled())
    {
        auto destroyFunc = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(m_instance.getHandle(), "vkDestroyDebugUtilsMessengerEXT");
        if(destroyFunc == nullptr)
        {
            throw std::runtime_error("Failed to destroy Debug Utils Messenger");
        }
        destroyFunc(m_instance.getHandle(), m_messenger, nullptr);
    }
}

auto vkc::DebugUtilsMessenger::PopulateCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) -> void
{
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    // Allow verbose, error, and warning message severity
    createInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    // Enable general, validation, and performance message types
    createInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = DebugUtilsMessenger::DebugCallback;
}

auto vkc::DebugUtilsMessenger::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                             void* pUserData) -> VKAPI_ATTR VkBool32

{
    std::cerr << "Validation Layer Debug: \n\t" << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}
