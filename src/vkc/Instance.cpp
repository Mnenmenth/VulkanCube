/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#include <stdexcept>
#include <cstring>
#include <iostream>
#include "Instance.h"
#include "Window.h"
#include "DebugUtilsMessenger.h"

const std::vector<type::cstr> vkc::Instance::ValidationLayers =
        {
                "VK_LAYER_KHRONOS_validation"
        };
const std::vector<type::cstr> vkc::Instance::DeviceExtensions =
        {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

vkc::Instance::Instance(const char* appName, const char* engineName, bool validationLayers) :
        m_instance(),
        m_validationLayers(validationLayers)
{
    if(m_validationLayers && !CheckValidationLayerSupport())
    {
        throw std::runtime_error("Validation layers requested but not available");
    }

    // Application info
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = appName;
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.pEngineName = engineName;
    appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // Instance info
    VkInstanceCreateInfo instanceInfo = {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;

    std::vector<type::cstr> extensions;
    GetRequiredExtensions(extensions, validationLayers);

    instanceInfo.enabledExtensionCount = static_cast<type::uint32>(extensions.size());
    instanceInfo.ppEnabledExtensionNames = extensions.data();

    // Add another debug messenger to be used during instance creation and destruction
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    if(m_validationLayers)
    {
        instanceInfo.enabledLayerCount = static_cast<type::uint32>(ValidationLayers.size());
        instanceInfo.ppEnabledLayerNames = ValidationLayers.data();

        vkc::DebugUtilsMessenger::PopulateCreateInfo(debugCreateInfo);
        instanceInfo.pNext = static_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debugCreateInfo);
    }

    if(vkCreateInstance(&instanceInfo, nullptr, &m_instance) != VK_SUCCESS)
    {
        throw std::runtime_error("Vulkan instance creation failed");
    }

    if(m_validationLayers)
    {
        m_debugMessenger = new DebugUtilsMessenger(m_instance);
    }
}

vkc::Instance::~Instance()
{
    if(m_validationLayers)
    {
        delete m_debugMessenger;
    }
    vkDestroyInstance(m_instance, nullptr);
}

auto vkc::Instance::CheckValidationLayerSupport() -> bool
{
    type::uint32 layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    // Check if requested layers are available
    for(type::cstr layerName : ValidationLayers)
    {
        bool layerFound = false;

        for(const auto& layerProperties : availableLayers)
        {
            if(strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if(!layerFound)
        {
            return false;
        }
    }

    return true;
}

auto vkc::Instance::GetRequiredExtensions(std::vector<type::cstr>& extensions, bool validationLayers) -> void
{
    vkc::Window::GetRequiredExtensions(extensions);

    if(validationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
}