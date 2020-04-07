/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#ifndef VULKANCUBE_GRAPHICSPIPELINE_H
#define VULKANCUBE_GRAPHICSPIPELINE_H

#include <vulkan/vulkan.h>
#include <vector>
#include "NonCopyable.h"
#include "Recreatable.h"

namespace vkc
{
    class Device;
    class SwapChain;
    class RenderPass;
    struct ShaderDetails;
    class GraphicsPipeline : public NonCopyable, public Recreatable
    {
    public:
        GraphicsPipeline(
                const vkc::Device& device,
                const vkc::SwapChain& swapChain,
                const vkc::RenderPass& renderPass,
                const std::vector<ShaderDetails>& shaderDetails,
                const std::vector<VkVertexInputBindingDescription>& bindingDescs,
                const std::vector<VkVertexInputAttributeDescription>& attrDescs
                );
        ~GraphicsPipeline();

        auto recreate() -> void override;
        auto cleanup() -> void override;

        [[nodiscard]]
        auto inline getPipeline() const -> const VkPipeline& { return m_pipeline; }
        [[nodiscard]]
        auto inline getLayout() const -> const VkPipelineLayout& { return m_layout; }

    private:
        VkPipeline m_pipeline;
        VkPipeline m_oldPipeline;
        VkPipelineLayout m_layout;
        VkPipelineLayout m_oldLayout;

        const vkc::Device& m_device;
        const vkc::SwapChain& m_swapChain;
        const vkc::RenderPass& m_renderPass;
        const std::vector<ShaderDetails>& m_shaderDetails;
        const std::vector<VkVertexInputBindingDescription>& m_bindingDescs;
        const std::vector<VkVertexInputAttributeDescription>& m_attrDescs;

        auto createPipeline() -> void;
        auto createShaderModule(const std::vector<char>& code) -> VkShaderModule;
    };
}

#endif //VULKANCUBE_GRAPHICSPIPELINE_H
