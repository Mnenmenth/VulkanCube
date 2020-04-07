/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#ifndef VULKANCUBE_DEBUGUTILSMESSENGER_H
#define VULKANCUBE_DEBUGUTILSMESSENGER_H

#include <vulkan/vulkan.h>
#include "NonCopyable.h"


namespace vkc
{
    class Instance;
    class DebugUtilsMessenger : public NonCopyable
    {
    public:
        explicit DebugUtilsMessenger(const vkc::Instance& instance);

        ~DebugUtilsMessenger();

        [[nodiscard]]
        inline auto getHandle() const -> const VkDebugUtilsMessengerEXT& { return m_messenger; }
        [[nodiscard]]
        inline auto getInstance() const -> const vkc::Instance& { return m_instance; }

        static auto DebugCallback(
                VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                void* pUserData
        ) -> VKAPI_ATTR VkBool32 VKAPI_CALL;

        static auto PopulateCreateInfo(VkDebugUtilsMessengerCreateInfoEXT&) -> void;

    private:
        const vkc::Instance& m_instance;
        VkDebugUtilsMessengerEXT m_messenger;
    };
}

#endif //VULKANCUBE_DEBUGUTILSMESSENGER_H
