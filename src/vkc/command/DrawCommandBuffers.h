/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#ifndef VULKANCUBE_DRAWCOMMANDBUFFERS_H
#define VULKANCUBE_DRAWCOMMANDBUFFERS_H

#include <vulkan/vulkan.h>
#include <vector>
#include "../NonCopyable.h"
#include "CommandPool.h"
#include "../Types.h"

namespace vkc
{
    class Device;
    class SwapChain;
    class RenderPass;
    class UBO;
    class GraphicsPipeline;
    class Buffer;
    class DrawCommandBuffers : public NonCopyable
    {
    public:
        DrawCommandBuffers(
                const vkc::Device& device,
                const vkc::SwapChain& swapChain,
                const vkc::RenderPass& renderPass,
                const vkc::UBO& ubo,
                const vkc::GraphicsPipeline& pipeline,
                const vkc::Buffer& modelBuffer,
                VkDeviceSize vertexOffset,
                VkDeviceSize indexOffset,
                type::uint32 indexSize
                );
        ~DrawCommandBuffers();

        [[nodiscard]]
        inline auto command(type::uint32 index) -> VkCommandBuffer& { return m_commands[index]; }

        auto recreate() -> void;

    private:
        CommandPool m_pool;
        std::vector<VkCommandBuffer> m_commands;

        const vkc::Device& m_device;
        const vkc::SwapChain& m_swapChain;
        const vkc::RenderPass& m_renderPass;
        const vkc::UBO& m_ubo;
        const vkc::GraphicsPipeline& m_pipeline;
        const vkc::Buffer& m_modelBuffer;
        VkDeviceSize m_vertexOffset;
        VkDeviceSize m_indexOffset;
        type::uint32 m_indexSize;

        auto create() -> void;
        auto destroy() -> void;
    };
}


#endif //VULKANCUBE_DRAWCOMMANDBUFFERS_H
