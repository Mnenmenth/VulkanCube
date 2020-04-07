/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#include "CommandPool.h"
#include "../Device.h"

vkc::CommandPool::CommandPool(const vkc::Device& device, const VkCommandPoolCreateFlags& flags) : m_device(device), m_flags(flags)
{
    VkCommandPoolCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    // Hard code graphics family since that's all its going to be used for
    info.queueFamilyIndex = m_device.getQueueFamilyIndices().graphics.value();
    info.flags = flags;

    if(vkCreateCommandPool(m_device.logical(), &info, nullptr, &m_pool) != VK_SUCCESS)
    {
        throw std::runtime_error("Command Pool creation failed");
    }
}

vkc::CommandPool::~CommandPool()
{
    vkDestroyCommandPool(m_device.logical(), m_pool, nullptr);
}
