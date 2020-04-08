/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#ifndef VULKANCUBE_DEVICE_H
#define VULKANCUBE_DEVICE_H

#include <vulkan/vulkan.h>
#include <vector>
#include "Types.h"
#include "NonCopyable.h"
#include "pipeline/QueueFamilyIndices.h"

namespace vkc
{
    class Instance;
    class Window;
    class Device : public NonCopyable
    {
    public:
        Device(const vkc::Instance& instance, const vkc::Window& window, const std::vector<type::cstr>& extensions);
        ~Device();

        [[nodiscard]]
        inline auto physical() const -> const VkPhysicalDevice& { return m_physical; }
        [[nodiscard]]
        inline auto logical() const -> const VkDevice& { return m_logical; }
        [[nodiscard]]
        inline auto queueFamilyIndices() const -> const vkc::QueueFamilyIndices& { return m_indices; }
        [[nodiscard]]
        inline auto graphicsQueue() const -> const VkQueue& { return m_graphicsQueue; }
        [[nodiscard]]
        inline auto presentQueue() const -> const VkQueue& { return m_presentQueue; }

    private:
        VkPhysicalDevice m_physical;
        VkDevice m_logical;

        const vkc::Instance& m_instance;
        const vkc::Window& m_window;

        vkc::QueueFamilyIndices m_indices;
        VkQueue m_graphicsQueue;
        VkQueue m_presentQueue;

        static auto CheckExtensionSupport(const VkPhysicalDevice& device, const std::vector<type::cstr>& extensions) -> bool;
        static auto RatePhysicalDevice(
                const VkPhysicalDevice& device,
                const VkSurfaceKHR& surface,
                const std::vector<type::cstr>& requiredExtensions
                ) -> std::pair<int, vkc::QueueFamilyIndices>;
        static auto FindPhysicalDevice(
                const VkInstance& instance,
                const VkSurfaceKHR& surface,
                const std::vector<type::cstr>& requiredExtensions
                ) -> std::pair<VkPhysicalDevice, vkc::QueueFamilyIndices>;
    };
}


#endif //VULKANCUBE_DEVICE_H
