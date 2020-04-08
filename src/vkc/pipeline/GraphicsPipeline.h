/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#ifndef VULKANCUBE_GRAPHICSPIPELINE_H
#define VULKANCUBE_GRAPHICSPIPELINE_H

#include <vulkan/vulkan.h>
#include <vector>
#include "../NonCopyable.h"

namespace vkc
{
    class Device;
    class SwapChain;
    class RenderPass;
    struct ShaderDetails;
    class GraphicsPipeline : public NonCopyable
    {
    public:
        GraphicsPipeline(
                const vkc::Device& device,
                const vkc::SwapChain& swapChain,
                const vkc::RenderPass& renderPass,
                const std::vector<VkDescriptorSetLayout>& descriptorLayouts,
                const std::vector<ShaderDetails>& shaderDetails,
                const std::vector<VkVertexInputBindingDescription>& bindingDescs,
                const std::vector<VkVertexInputAttributeDescription>& attrDescs
                );
        ~GraphicsPipeline();

        auto recreate() -> void;

        [[nodiscard]]
        auto inline pipeline() const -> const VkPipeline& { return m_pipeline; }
        [[nodiscard]]
        auto inline layout() const -> const VkPipelineLayout& { return m_layout; }

    private:
        VkPipeline m_pipeline;
        VkPipelineLayout m_layout;
        VkPipelineLayout m_oldLayout;

        const vkc::Device& m_device;
        const vkc::SwapChain& m_swapChain;
        const vkc::RenderPass& m_renderPass;
        std::vector<VkDescriptorSetLayout> m_descriptorLayouts;
        std::vector<ShaderDetails> m_shaderDetails;
        std::vector<VkVertexInputBindingDescription> m_bindingDescs;
        std::vector<VkVertexInputAttributeDescription> m_attrDescs;

        auto createPipeline() -> void;
        auto createShaderModule(const std::vector<char>& code) -> VkShaderModule;
    };
}

#endif //VULKANCUBE_GRAPHICSPIPELINE_H
