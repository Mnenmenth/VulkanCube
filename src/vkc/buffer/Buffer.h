/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#ifndef VULKANCUBE_BUFFER_H
#define VULKANCUBE_BUFFER_H

#include <vulkan/vulkan.h>
#include <vector>
#include "../NonCopyable.h"
#include "../Types.h"
#include "../command/CommandPool.h"

// TODO: Command pool for short lived/staging buffers

namespace vkc
{
    class Device;
    class Buffer : public NonCopyable
    {
    public:
        Buffer(
                const vkc::Device& device,
                VkDeviceSize size,
                const VkBufferUsageFlags& usageFlags,
                const VkMemoryPropertyFlags& memPropFlags,
                const VkSharingMode& sharingMode
                );
        ~Buffer();

        [[nodiscard]]
        auto inline getSize() const -> VkDeviceSize { return m_size; }
        //auto inline copyTo(vkc::Buffer& buffer) -> void { copyBuffer(buffer.m_buffer, buffer.m_size, 0, m_buffer, ); }

        //TODO
        //auto resize(type::uint32 size) -> void;

        auto setContents(VkDeviceSize size, VkDeviceSize offset, void* data) -> void;

    protected:
        VkPhysicalDeviceMemoryProperties m_memProp;

        VkBuffer m_buffer;
        VkDeviceMemory m_memory;
        VkMemoryRequirements m_memReq;

        VkDeviceSize m_size;
        VkBufferUsageFlags m_usageFlags;
        VkMemoryPropertyFlags m_memPropFlags;
        VkSharingMode m_sharingMode;

        VkBuffer m_stagingBuff;
        VkDeviceMemory m_stagingMem;
        VkMemoryRequirements m_stagingMemReq;

        auto createBufferAndMem(
                VkBuffer& buff,
                VkDeviceMemory& mem,
                VkMemoryRequirements& memReq,
                VkDeviceSize size,
                VkDeviceSize offset,
                const VkBufferUsageFlags& usageFlags,
                const VkMemoryPropertyFlags& memPropFlags,
                const VkSharingMode& sharingMode
                ) -> void;

        const vkc::Device& m_device;

        vkc::CommandPool m_cmdPool;
        auto copyBuffer(
                VkBuffer srcBuffer,
                VkDeviceSize srcSize,
                VkDeviceSize srcOffset,
                VkBuffer dstBuffer,
                VkDeviceSize dstOffset
                ) -> void;
    };
}


#endif //VULKANCUBE_BUFFER_H
