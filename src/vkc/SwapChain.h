/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#ifndef VULKANCUBE_SWAPCHAIN_H
#define VULKANCUBE_SWAPCHAIN_H

#define GLFW_INCLUDE_VULKAN
#include <vector>
#include "SwapChainSupportDetails.h"
#include "NonCopyable.h"
#include "NonMoveable.h"

namespace vkc
{
    class Device;
    class Window;

    class SwapChain : public NonCopyable, public NonMoveable
    {
    public:
        SwapChain(const vkc::Device& device, const vkc::Window& window);
        ~SwapChain();

        auto recreate() -> void;

        [[nodiscard]]
        inline auto getSupportDetails() const -> const SwapChainSupportDetails& { return m_supportDetails; }

        static auto QuerySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) -> vkc::SwapChainSupportDetails;
        static auto ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const vkc::Window& window) -> VkExtent2D;

    private:
        const vkc::Device& m_device;
        const vkc::Window& m_window;

        SwapChainSupportDetails m_supportDetails;
        VkSwapchainKHR m_swapChain;

        // Swap chain image handles
        std::vector<VkImage> m_images;
        std::vector<VkImageView> m_imageViews;

        VkFormat m_imageFormat;
        VkExtent2D m_extent;

        auto createSwapChain() -> void;
        auto createImageViews() -> void;

        auto destroyImageViews() -> void;
    };
}


#endif //VULKANCUBE_SWAPCHAIN_H
