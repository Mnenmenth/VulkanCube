/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#include <iostream>
#include "SwapChain.h"
#include "Device.h"
#include "Window.h"
#include "Types.h"

vkc::SwapChain::SwapChain(const vkc::Device& device) :
        m_swapChain(VK_NULL_HANDLE),
        m_oldSwapChain(VK_NULL_HANDLE),
        m_extent(),
        m_imageFormat(),
        m_device(device)
{
    createSwapChain();
    createImageViews();
}

auto vkc::SwapChain::recreate() -> void
{
    destroyImageViews();
    m_oldSwapChain = m_swapChain;
    createSwapChain();
    createImageViews();
}

auto vkc::SwapChain::cleanup() -> void
{
    // Destroy old swap chain if it exists
    if(m_oldSwapChain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(m_device.getLogical(), m_oldSwapChain, nullptr);
        m_oldSwapChain = VK_NULL_HANDLE;
    }
}

auto vkc::SwapChain::createSwapChain() -> void
{
    m_supportDetails = QuerySwapChainSupport(m_device.getPhysical(), m_device.getWindow().getSurface());
    m_extent = ChooseSwapExtent(m_supportDetails.capabilities, m_device.getWindow());

    // Choose the swap surface format
    // Default to first available format
    VkSurfaceFormatKHR surfaceFormat = m_supportDetails.formats[0];
    // Try to find support for 8-bit SRGB instead
    for(const auto& format : m_supportDetails.formats)
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
    if(std::find(m_supportDetails.presentModes.begin(), m_supportDetails.presentModes.end(), VK_PRESENT_MODE_MAILBOX_KHR) != m_supportDetails.presentModes.end())
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
    type::uint32 imageCount = m_supportDetails.capabilities.minImageCount + 1;

    // Make sure image count doesn't exceed maximum
    // A max image count of 0 indicates that there is no maximum
    if(m_supportDetails.capabilities.maxImageCount > 0 && imageCount > m_supportDetails.capabilities.maxImageCount)
    {
        imageCount = m_supportDetails.capabilities.maxImageCount;
    }

    // Setup the swapchain
    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_device.getWindow().getSurface();
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
    createInfo.preTransform = m_supportDetails.capabilities.currentTransform;
    // Blend with other windows in window system
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    // Clip obscured pixels
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = m_oldSwapChain;

    if(vkCreateSwapchainKHR(m_device.getLogical(), &createInfo, nullptr, &m_swapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("Swap chain creation failed");
    }

    // Get new swap chain images
    vkGetSwapchainImagesKHR(m_device.getLogical(), m_swapChain, &imageCount, nullptr);
    m_images.resize(imageCount);
    vkGetSwapchainImagesKHR(m_device.getLogical(), m_swapChain, &imageCount, m_images.data());
}

auto vkc::SwapChain::createImageViews() -> void
{
    m_imageViews.resize(m_images.size());

    for(type::size i = 0; i < m_images.size(); ++i)
    {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_images[i];
        // How image should be interpreted
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_imageFormat;
        // Leave swizzling as default
        createInfo.components.r =
        createInfo.components.g =
        createInfo.components.b =
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        // Purpose of image and which parts should be accessed
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if(vkCreateImageView(m_device.getLogical(), &createInfo, nullptr, &m_imageViews[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Image view creation failed");
        }
    }
}

auto vkc::SwapChain::destroyImageViews() -> void
{
    for(VkImageView& view : m_imageViews)
    {
        vkDestroyImageView(m_device.getLogical(), view, nullptr);
    }
}

vkc::SwapChain::~SwapChain()
{
    destroyImageViews();
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
        glm::ivec2 size;
        window.getFramebufferSize(size);

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
