/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#ifndef VULKANCUBE_QUEUEFAMILYINDICES_H
#define VULKANCUBE_QUEUEFAMILYINDICES_H

#include <optional>
#include "../Types.h"

namespace vkc
{
    struct QueueFamilyIndices
    {
        // Drawing command support
        std::optional<type::uint32> graphics;
        // Support for drawing to surface
        std::optional<type::uint32> present;
        [[nodiscard]]
        inline auto isComplete() const -> bool { return graphics.has_value() && present.has_value(); }
    };
}

#endif //VULKANCUBE_QUEUEFAMILYINDICES_H
