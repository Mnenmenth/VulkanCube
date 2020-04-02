/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#ifndef VULKANCUBE_DEVICE_H
#define VULKANCUBE_DEVICE_H

#include "Types.h"
#include "NonCopyable.h"
#include "SwapChainSupportDetails.h"
#include "QueueFamilyIndices.h"

namespace vkc
{
    class Instance;

    class Device : public NonCopyable
    {
    public:
        Device(const vkc::Instance& instance, const VkSurfaceKHR& surface, const std::vector<type::cstr>& extensions);
        ~Device();

        [[nodiscard]]
        inline auto getPhysical() const -> const VkPhysicalDevice& { return m_physical; }
        [[nodiscard]]
        inline auto getLogical() const -> const VkDevice& { return m_logical; }
        [[nodiscard]]
        inline auto getQueueFamilyIndices() const -> const vkc::QueueFamilyIndices& { return m_indices; }
        [[nodiscard]]
        inline auto getGraphicsQueue() const -> const VkQueue& { return m_graphicsQueue; }
        [[nodiscard]]
        inline auto getPresentQueue() const -> const VkQueue& { return m_presentQueue; }
        [[nodiscard]]
        inline auto getSwapChainDetails() const -> const vkc::SwapChainSupportDetails& { return m_swapChainSupport; }

    private:
        VkPhysicalDevice m_physical;
        VkDevice m_logical;

        const vkc::Instance& m_instance;
        const VkSurfaceKHR& m_surface;

        vkc::QueueFamilyIndices m_indices;
        VkQueue m_graphicsQueue;
        VkQueue m_presentQueue;
        vkc::SwapChainSupportDetails m_swapChainSupport;

        static auto CheckExtensionSupport(const VkPhysicalDevice& device, const std::vector<type::cstr>& extensions) -> bool;
        static auto RatePhysicalDevice(
                const VkPhysicalDevice& device,
                const VkSurfaceKHR& surface,
                const std::vector<type::cstr>& requiredExtensions
                ) -> std::tuple<int, vkc::QueueFamilyIndices, vkc::SwapChainSupportDetails>;
        static auto FindPhysicalDevice(
                const VkInstance& instance,
                const VkSurfaceKHR& surface,
                const std::vector<type::cstr>& requiredExtensions
                ) -> std::tuple<VkPhysicalDevice, vkc::QueueFamilyIndices, vkc::SwapChainSupportDetails>;
    };
}


#endif //VULKANCUBE_DEVICE_H
