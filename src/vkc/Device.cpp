/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#include <vector>
#include <set>
#include <map>
#include "Device.h"
#include "Instance.h"
#include "SwapChain.h"
#include "QueueFamily.h"

vkc::Device::Device(const vkc::Instance& instance, const VkSurfaceKHR& surface, const std::vector<type::cstr>& extensions) :
        m_physical(),
        m_logical(),
        m_instance(instance),
        m_surface(surface),
        m_graphicsQueue(),
        m_presentQueue()
{

    auto device = FindPhysicalDevice(instance.get(), surface, extensions);
    m_physical = device.first;
    m_indices = device.second;

    // Setup queue families for device
    std::set<type::uint32> uniqueQueueFamilies =
            {
                m_indices.graphics.value(),
                m_indices.present.value()
            };
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    float priority = 1.0f;
    for(type::uint32 queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        createInfo.queueFamilyIndex = queueFamily;
        createInfo.queueCount = 1;
        createInfo.pQueuePriorities = &priority;
        queueCreateInfos.push_back(createInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};

    // Setup logical device
    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<type::uint32>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<type::uint32>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    if(instance.validationLayersEnabled())
    {
        createInfo.enabledLayerCount = static_cast<type::uint32>(vkc::Instance::ValidationLayers.size());
        createInfo.ppEnabledLayerNames = vkc::Instance::ValidationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if(vkCreateDevice(m_physical, &createInfo, nullptr, &m_logical) != VK_SUCCESS)
    {
        throw std::runtime_error("Logical device creation failed");
    }

    // Get handles for graphics and presentation queues
    vkGetDeviceQueue(m_logical, m_indices.graphics.value(), 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_logical, m_indices.present.value(), 0, &m_presentQueue);
}

vkc::Device::~Device()
{
    vkDestroyDevice(m_logical, nullptr);
}

auto vkc::Device::CheckExtensionSupport(const VkPhysicalDevice& device, const std::vector<type::cstr>& extensions) -> bool
{
    // Get number of extension supported
    type::uint32 extCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, nullptr);

    if(extCount < 1)
        return false;

    // Get supported extensions
    std::vector<VkExtensionProperties> availableExtensions(extCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, availableExtensions.data());

    // Iterate through available extensions and make sure that all are present
    std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());
    for(const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

auto vkc::Device::RatePhysicalDevice(
        const VkPhysicalDevice& device,
        const VkSurfaceKHR& surface,
        const std::vector<type::cstr>& requiredExtensions
        ) -> std::pair<int, vkc::QueueFamilyIndices>
{
    // Device isn't suitable if it doesn't support required queue families and extensions
    QueueFamilyIndices indices = vkc::QueueFamily::FindQueueFamilies(device, surface);
    if(!indices.isComplete() || !CheckExtensionSupport(device, requiredExtensions))
    {
        return std::make_pair(0, indices);
    }

    // Device isn't suitable if there isn't at least one format and present mode
    SwapChainSupportDetails swapChainSupport = SwapChain::QuerySwapChainSupport(device, surface);
    if(swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty())
    {
        return std::make_pair(0, indices);
    }

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
//    VkPhysicalDeviceFeatures deviceFeatures;
//    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    int score = 0;

    // Highly prefer discrete GPU
    if(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        score += 1000;
    }

    score += deviceProperties.limits.maxImageDimension2D;

    return std::make_pair(score, indices);
}

auto vkc::Device::FindPhysicalDevice(
        const VkInstance& instance,
        const VkSurfaceKHR& surface,
        const std::vector<type::cstr>& requiredExtensions
        )-> std::pair<VkPhysicalDevice, vkc::QueueFamilyIndices>
{
    // Check for devices with vulkan support
    type::uint32 deviceCount;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if(deviceCount < 1)
    {
        throw std::runtime_error("No GPU's with Vulkan support available");
    }

    // Get available devices
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    // Order devices by rating
    int highestRating = 0;
    std::pair<VkPhysicalDevice, vkc::QueueFamilyIndices> highestRated;
    for(const auto& d : devices)
    {
        auto rating = RatePhysicalDevice(d, surface, requiredExtensions);
        if(std::get<0>(rating) > highestRating)
        {
            highestRating = std::get<0>(rating);
            highestRated = std::make_pair(d, std::get<1>(rating));
        }
    }

    if(highestRating > 0)
    {
        return highestRated;
    }
    else
    {
        throw std::runtime_error("GPU('s) found, but none are suitable");
    }
}
