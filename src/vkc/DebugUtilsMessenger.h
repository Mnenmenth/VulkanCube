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
    class DebugUtilsMessenger : public NonCopyable
    {
    public:
        explicit DebugUtilsMessenger(const VkInstance& instance);

        ~DebugUtilsMessenger();

        [[nodiscard]]
        inline auto get() const -> const VkDebugUtilsMessengerEXT& { return m_messenger; }

        static auto DebugCallback(
                VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                void* pUserData
        ) -> VKAPI_ATTR VkBool32 VKAPI_CALL;

        static auto PopulateCreateInfo(VkDebugUtilsMessengerCreateInfoEXT&) -> void;

    private:
        const VkInstance& m_instance;
        VkDebugUtilsMessengerEXT m_messenger;
    };
}

#endif //VULKANCUBE_DEBUGUTILSMESSENGER_H
