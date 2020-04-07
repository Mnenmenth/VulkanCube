/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#ifndef VULKANCUBE_RENDERPASS_H
#define VULKANCUBE_RENDERPASS_H


#include <vulkan/vulkan.h>
#include "NonCopyable.h"
#include "Recreatable.h"

namespace vkc
{
    class Device;
    class SwapChain;
    class RenderPass : public NonCopyable, public Recreatable
    {
    public:
        RenderPass(const vkc::Device& device, const vkc::SwapChain& swapChain);
        ~RenderPass();

        [[nodiscard]]
        auto inline getHandle() const -> const VkRenderPass& { return m_renderPass; }

        [[nodiscard]]
        auto inline getSwapChain() const -> const vkc::SwapChain& { return m_swapChain; };

        auto recreate() -> void override;
        auto cleanup() -> void override;

    private:
        VkRenderPass m_renderPass;
        VkRenderPass m_oldRenderPass;

        const vkc::Device& m_device;
        const vkc::SwapChain& m_swapChain;

        auto create() -> void;
    };
}


#endif //VULKANCUBE_RENDERPASS_H
