/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#ifndef VULKANCUBE_INSTANCE_H
#define VULKANCUBE_INSTANCE_H

#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include <memory>
#include "NonCopyable.h"
#include "Types.h"

namespace vkc {
    class Instance : public NonCopyable
    {
    public:
        Instance(const char* appName, const char* engineName, bool validationLayers);
        Instance() = delete;
        ~Instance();

        [[nodiscard]]
        inline auto handle() const -> const VkInstance& { return m_instance; }

        [[nodiscard]]
        auto validationLayersEnabled() const -> bool { return m_validationLayers; }

        static const std::vector<type::cstr> ValidationLayers;
        static const std::vector<type::cstr> DeviceExtensions;

    private:
        VkInstance m_instance;
        bool m_validationLayers;

        static auto CheckValidationLayerSupport() -> bool;
        static auto GetRequiredExtensions(std::vector<type::cstr>& extensions, bool validationLayers) -> void;

    };
}

#endif //VULKANCUBE_INSTANCE_H
