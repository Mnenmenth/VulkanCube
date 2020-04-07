/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#ifndef VULKANCUBE_COMMANDPOOL_H
#define VULKANCUBE_COMMANDPOOL_H

#include <vulkan/vulkan.h>
#include "../NonCopyable.h"

namespace vkc
{
    class Device;
    class CommandPool : public NonCopyable
    {
    public:
        CommandPool(const vkc::Device& device, const VkCommandPoolCreateFlags& flags);
        ~CommandPool();

        [[nodiscard]]
        auto getHandle() const -> const VkCommandPool& { return m_pool; };
    private:
        VkCommandPool m_pool;
        VkCommandPoolCreateFlags m_flags;

        const vkc::Device& m_device;
    };
}


#endif //VULKANCUBE_COMMANDPOOL_H
