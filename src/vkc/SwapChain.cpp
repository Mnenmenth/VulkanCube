/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#include "SwapChain.h"
#include "Device.h"
#include "Window.h"
#include "Types.h"

vkc::SwapChain::SwapChain(const vkc::Device& device, const vkc::Window& window, const VkSwapchainKHR& oldSwapChain) :
        m_swapChain(),
        m_extent(),
        m_imageFormat(),
        m_device(device),
        m_window(window)
{
    const SwapChainSupportDetails& supportDetails = m_device.getSwapChainDetails();
    m_extent = ChooseSwapExtent(supportDetails.capabilities, m_window);

    // Choose the swap surface format
    // Default to first available format
    VkSurfaceFormatKHR surfaceFormat = supportDetails.formats[0];
    // Try to find support for 8-bit SRGB instead
    for(const auto& format : supportDetails.formats)
    {
        if(format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            surfaceFormat = format;
            break;
        }
    }

    m_imageFormat = surfaceFormat.format;

    // Choose the presentation mode
    VkPresentModeKHR presentMode;
    // Try to find support for Mailbox
        // MAILBOX (triple buffering) uses a queue to present images,
        // and if the queue is full already queued images are overwritten with newer images
    if(std::find(supportDetails.presentModes.begin(), supportDetails.presentModes.end(), VK_PRESENT_MODE_MAILBOX_KHR) != supportDetails.presentModes.end())
    {
        presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    }
    // Otherwise default to VK_PRESENT_MODE_FIFO_KHR
        // FIFO is guaranteed to be present and is essentially traditional V-Sync
    else
    {
        presentMode = VK_PRESENT_MODE_FIFO_KHR;
    }


    // How many images should be in the swap chain
        // One more than the minimum helps with wait times before another image is available from driver
    type::uint32 imageCount = supportDetails.capabilities.minImageCount + 1;

    // Make sure image count doesn't exceed maximum
        // A max image count of 0 indicates that there is no maximum
    if(supportDetails.capabilities.maxImageCount > 0 && imageCount > supportDetails.capabilities.maxImageCount)
    {
        imageCount = supportDetails.capabilities.maxImageCount;
    }

    // Setup the swapchain
    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_window.getSurface();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = m_extent;
    // Amount of layers in each image
        // This is always 1 unless doing something like stereoscopic 3D
    createInfo.imageArrayLayers = 1;
    // Image is being directly rendered to, so make i a color attachment
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // How to handle swap chain images across multiple queue families
    const QueueFamilyIndices& indices = m_device.getQueueFamilyIndices();
    type::uint32 queueFamilyIndices[] = {indices.graphics.value(), indices.present.value()};

    // Determine if there are multiple queue families
    if(indices.graphics != indices.present)
    {
        // Use concurrent mode if there are. Worse performance but no ownership transfers
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        // Use exclusive mode if single queue family. Best performance
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    // Don't do any extra transformations to images
    createInfo.preTransform = supportDetails.capabilities.currentTransform;
    // Blend with other windows in window system
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    // Clip obscured pixels
    createInfo.clipped = VK_TRUE;
    //TODO: Accept old swap chain in constructor and apply here
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if(vkCreateSwapchainKHR(m_device.getLogical(), &createInfo, nullptr, &m_swapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("Swap chain creation failed");
    }

    vkGetSwapchainImagesKHR(m_device.getLogical(), m_swapChain, &imageCount, nullptr);
    m_images.resize(imageCount);
    vkGetSwapchainImagesKHR(m_device.getLogical(), m_swapChain, &imageCount, m_images.data());

}

vkc::SwapChain::~SwapChain()
{
    vkDestroySwapchainKHR(m_device.getLogical(), m_swapChain, nullptr);
}

auto vkc::SwapChain::QuerySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) -> vkc::SwapChainSupportDetails
{
    SwapChainSupportDetails details;

    // Get capabilities of both device and surface
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    // Get number of supported surface formats
    type::uint32 formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    // Get supported surface formats
    if(formatCount > 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    // Get number of supported presentation modes
    type::uint32 presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if(presentModeCount > 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

auto vkc::SwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const vkc::Window& window) -> VkExtent2D
{
    // Vulkan uses uint32 max value to signify window resolution should be used
    if(capabilities.currentExtent.width != type::uint32_max)
    {
        return capabilities.currentExtent;
    }
    // Otherwise, the window manager allows a custom resolution
    else
    {
        glm::ivec2 size = window.getFramebufferSize();

        VkExtent2D extent =
                {
                        static_cast<type::uint32>(size[0]),
                        static_cast<type::uint32>(size[1])
                };
        // Determine if resolution given by vulkan or custom window resolution is the better fit
        extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, extent.width));
        extent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, extent.height));

        return extent;
    }
}
