/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#ifndef VULKANCUBE_SWAPCHAIN_H
#define VULKANCUBE_SWAPCHAIN_H

#include <vector>
#include "SwapChainSupportDetails.h"
#include "../NonCopyable.h"
#include "../Recreatable.h"
#include "../Types.h"

namespace vkc
{
    class Device;
    class Window;

    class SwapChain : public NonCopyable, public Recreatable
    {
    public:
        explicit SwapChain(const vkc::Device& device, const vkc::Window& window);
        ~SwapChain();

        auto recreate() -> void override;
        auto cleanupOld() -> void override;

        [[nodiscard]]
        inline auto handle() const -> const VkSwapchainKHR& { return m_swapChain; }
        [[nodiscard]]
        inline auto imageFormat() const -> const VkFormat& { return m_imageFormat; }
        [[nodiscard]]
        inline auto extent() const -> const VkExtent2D& { return m_extent; }
        [[nodiscard]]
        inline auto numImages() const -> type::size { return m_images.size(); }

        [[nodiscard]]
        inline auto supportDetails() const -> const vkc::SwapChainSupportDetails& { return m_supportDetails; }
        [[nodiscard]]
        inline auto imageView(type::uint32 index) const -> VkImageView { return m_imageViews[index]; }

        static auto QuerySwapChainSupport(const VkPhysicalDevice& device, const VkSurfaceKHR& surface) -> vkc::SwapChainSupportDetails;
        static auto ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const vkc::Window& window) -> VkExtent2D;

    private:
        const vkc::Device& m_device;
        const vkc::Window& m_window;

        SwapChainSupportDetails m_supportDetails;
        VkSwapchainKHR m_swapChain;
        VkSwapchainKHR m_oldSwapChain;

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
