/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#include "Buffer.h"
#include "../Device.h"
#include "../command/CommandPool.h"

vkc::Buffer::Buffer(
        const vkc::Device& device,
        VkDeviceSize size,
        const VkBufferUsageFlags& usageFlags,
        const VkMemoryPropertyFlags& memPropFlags,
        const VkSharingMode& sharingMode
) :
        m_buffer(VK_NULL_HANDLE),
        m_memory(VK_NULL_HANDLE),
        m_memProp(),
        m_size(size),
        m_usageFlags(usageFlags),
        m_memPropFlags(memPropFlags),
        m_sharingMode(sharingMode),

        m_stagingBuff(VK_NULL_HANDLE),
        m_stagingMem(VK_NULL_HANDLE),

        m_device(device),
        m_cmdPool(m_device, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT)
{
    // Get index of memory type that satisfies all requirements
    vkGetPhysicalDeviceMemoryProperties(m_device.getPhysical(), &m_memProp);

    // Create main buffer
    createBufferAndMem(m_buffer, m_memory, m_memReq, m_size, 0, m_usageFlags, m_memPropFlags, m_sharingMode);

    // Create staging buffer
    createBufferAndMem(
            m_stagingBuff,
            m_stagingMem,
            m_stagingMemReq,
            m_size,
            0,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            VK_SHARING_MODE_EXCLUSIVE
    );
}

vkc::Buffer::~Buffer()
{
    vkDestroyBuffer(m_device.getLogical(), m_buffer, nullptr);
    vkFreeMemory(m_device.getLogical(), m_memory, nullptr);

    vkDestroyBuffer(m_device.getLogical(), m_stagingBuff, nullptr);
    vkFreeMemory(m_device.getLogical(), m_stagingMem, nullptr);
}

auto vkc::Buffer::setContents(VkDeviceSize size, VkDeviceSize offset, void* data) -> void
{
    void* buff;
    vkMapMemory(m_device.getLogical(), m_stagingMem, offset, size, 0, &buff);
    memcpy(buff, data, static_cast<type::size>(size));
    vkUnmapMemory(m_device.getLogical(), m_stagingMem);
    copyBuffer(m_stagingBuff, size, 0, m_buffer, offset);
}

auto vkc::Buffer::copyBuffer(
        VkBuffer srcBuffer,
        VkDeviceSize srcSize,
        VkDeviceSize srcOffset,
        VkBuffer dstBuffer,
        VkDeviceSize dstOffset
        ) -> void
{
    // Create a one time use command buffer for copying the buffer
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_cmdPool.getHandle();
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer cmdBuff;
    vkAllocateCommandBuffers(m_device.getLogical(), &allocInfo, &cmdBuff);

    // Record command buffer
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    // This command buffer will only be used once then immediately discarded
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(cmdBuff, &beginInfo);

    VkBufferCopy copyRegion = {};
    copyRegion.srcOffset = srcOffset;
    copyRegion.dstOffset = dstOffset;
    copyRegion.size = srcSize;
    vkCmdCopyBuffer(cmdBuff, srcBuffer, dstBuffer, 1, &copyRegion);

    // Submit command buffer
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuff;

    //TODO: Fence for async transfers
    vkQueueSubmit(m_device.getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    //TODO: This is required if not using a fence
    vkQueueWaitIdle(m_device.getGraphicsQueue());

    vkFreeCommandBuffers(m_device.getLogical(), m_cmdPool.getHandle(), 1, &cmdBuff);
}

auto vkc::Buffer::createBufferAndMem(
        VkBuffer& buff,
        VkDeviceMemory& mem,
        VkMemoryRequirements& memReq,
        VkDeviceSize size,
        VkDeviceSize offset,
        const VkBufferUsageFlags& usageFlags,
        const VkMemoryPropertyFlags& memPropFlags,
        const VkSharingMode& sharingMode
        ) -> void
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usageFlags;
    bufferInfo.sharingMode = sharingMode;

    if(vkCreateBuffer(m_device.getLogical(), &bufferInfo, nullptr, &buff) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create buffer");
    }

    // Allocate buffer memory
    vkGetBufferMemoryRequirements(m_device.getLogical(), buff, &memReq);

    // Get the index of memory type that satisfies all requirements
    type::uint32 memIndex;
    bool found = false;
    for(type::uint32 i = 0; !found && i < m_memProp.memoryTypeCount; ++i)
    {
        if((memReq.memoryTypeBits & (1 << i)) && (m_memProp.memoryTypes[i].propertyFlags & m_memPropFlags) == m_memPropFlags)
        {
            memIndex = i;
            found = true;
        }
    }
    if(!found)
    {
        throw std::runtime_error("Suitable memory type unavailable");
    }

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = memIndex;

    if(vkAllocateMemory(m_device.getLogical(), &allocInfo, nullptr, &mem))
    {
        throw std::runtime_error("Buffer memory allocation failed");
    }

    // Bind buffer to memory
    vkBindBufferMemory(m_device.getLogical(), buff, mem, offset);
}
