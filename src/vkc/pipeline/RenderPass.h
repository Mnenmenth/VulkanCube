/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#ifndef VULKANCUBE_RENDERPASS_H
#define VULKANCUBE_RENDERPASS_H


#include <vulkan/vulkan.h>
#include <vector>
#include "../NonCopyable.h"
#include "../Types.h"

namespace vkc
{
    class Device;
    class SwapChain;
    class RenderPass : public NonCopyable
    {
    public:
        RenderPass(const vkc::Device& device, const vkc::SwapChain& swapChain);
        ~RenderPass();

        [[nodiscard]]
        auto inline handle() const -> const VkRenderPass& { return m_renderPass; }
        [[nodiscard]]
        inline auto frameBuffer(type::uint32 index) const -> const VkFramebuffer& { return m_frameBuffers[index]; }

        auto recreate() -> void;
        auto cleanupOld() -> void;

    private:
        VkRenderPass m_renderPass;
        VkRenderPass m_oldRenderPass;

        std::vector<VkFramebuffer> m_frameBuffers;

        const vkc::Device& m_device;
        const vkc::SwapChain& m_swapChain;

        auto createRenderPass() -> void;
        auto createFrameBuffers() -> void;

        auto destroyFrameBuffers() -> void;

    };
}


#endif //VULKANCUBE_RENDERPASS_H
