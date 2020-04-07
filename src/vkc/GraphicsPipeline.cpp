/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#include "GraphicsPipeline.h"
#include "Types.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "Device.h"
#include "ShaderDetails.h"
#include "FileIO.h"

vkc::GraphicsPipeline::GraphicsPipeline(
        const vkc::Device& device,
        const vkc::SwapChain& swapChain,
        const vkc::RenderPass& renderPass,
        const std::vector<ShaderDetails>& shaderDetails,
        const std::vector<VkVertexInputBindingDescription>& bindingDescs,
        const std::vector<VkVertexInputAttributeDescription>& attrDescs
) :
        m_pipeline(VK_NULL_HANDLE),
        m_oldPipeline(VK_NULL_HANDLE),
        m_desciptor(VK_NULL_HANDLE),
        m_layout(VK_NULL_HANDLE),
        m_oldLayout(VK_NULL_HANDLE),

        m_device(device),
        m_swapChain(swapChain),
        m_renderPass(renderPass),

        m_shaderDetails(shaderDetails),
        m_bindingDescs(bindingDescs),
        m_attrDescs(attrDescs)
{
    createPipeline();
}

vkc::GraphicsPipeline::~GraphicsPipeline()
{
    vkDestroyPipeline(m_device.getLogical(), m_pipeline, nullptr);
    vkDestroyPipelineLayout(m_device.getLogical(), m_layout, nullptr);
}

auto vkc::GraphicsPipeline::recreate() -> void
{
    m_oldPipeline = m_pipeline;
    m_oldLayout = m_layout;
    createPipeline();
}

auto vkc::GraphicsPipeline::cleanup() -> void
{
    if(m_oldPipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(m_device.getLogical(), m_oldPipeline, nullptr);
        m_oldPipeline = VK_NULL_HANDLE;
    }
    if(m_oldLayout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(m_device.getLogical(), m_oldLayout, nullptr);
        m_oldLayout = VK_NULL_HANDLE;
    }
}


auto vkc::GraphicsPipeline::createPipeline() -> void
{
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages(m_shaderDetails.size());
    for(const ShaderDetails& shader : m_shaderDetails)
    {
        std::vector<char> code;
        FileIO::ReadFile(shader.filePath, code);

        VkShaderModule module = createShaderModule(code);

        VkPipelineShaderStageCreateInfo stageInfo = {};
        stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        stageInfo.module = module;
        // Entry point into the shader (i.e. "main" method)
            // It's possible to have multiple entry points in a shader
        stageInfo.pName = "main";
        stageInfo.pSpecializationInfo = nullptr;

        shaderStages.push_back(stageInfo);
    }

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.vertexBindingDescriptionCount = static_cast<type::uint32>(m_bindingDescs.size());
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<type::uint32>(m_attrDescs.size());
    vertexInputInfo.pVertexBindingDescriptions = m_bindingDescs.data();
    vertexInputInfo.pVertexAttributeDescriptions = m_attrDescs.data();

    // Type of geometry that is being drawn (we're just assuming triangles for this program)
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // Pipeline viewport
    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_swapChain.getExtent().width);
    viewport.height = static_cast<float>(m_swapChain.getExtent().height);
    // Depth buffer range
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    // Pixel boundary cutoff
    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = m_swapChain.getExtent();

    // Combine viewport(s) and scissor(s) (some graphics cards allow multiple of each)
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    // Rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    // Clip fragments instead of clipping them to near and far planes
    rasterizer.depthClampEnable = VK_FALSE;
    // Don't allow the rasterizer to discard geometry
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    // Fill fragments
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    // Bias depth values
        // This is good for shadow mapping, but we're not doing that currently
        // so we'll disable for now
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    // Color Blending
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
    // Disable blending
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    // Disable bitwise combination blending
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOpEnable = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    //Which color channels in framebuffer will be affected
    for(float& blendConstant : colorBlending.blendConstants)
    {
        blendConstant = 0.0f;
    }


    VkPipelineLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.setLayoutCount = 1;
    layoutInfo.pSetLayouts = &m_desciptor;
    layoutInfo.pushConstantRangeCount = 0;
    layoutInfo.pPushConstantRanges = nullptr;

    if(vkCreatePipelineLayout(m_device.getLogical(), &layoutInfo, nullptr, &m_layout) != VK_SUCCESS)
    {
        throw std::runtime_error("Pipeline Layout creation failed");
    }


    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = static_cast<type::uint32>(shaderStages.size());
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pMultisampleState = nullptr;
    pipelineInfo.pDepthStencilState = nullptr;
    pipelineInfo.pDynamicState = nullptr;
    pipelineInfo.layout = m_layout;
    pipelineInfo.renderPass = m_renderPass.getHandle();
    pipelineInfo.subpass = 0;

    if(m_oldLayout != VK_NULL_HANDLE)
    {
        pipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
    }
    pipelineInfo.basePipelineHandle = m_oldPipeline;
    pipelineInfo.basePipelineIndex = -1;

    if(vkCreateGraphicsPipelines(m_device.getLogical(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("Graphics Pipeline creation failed");
    }

    for(auto& shader : shaderStages)
    {
        vkDestroyShaderModule(m_device.getLogical(), shader.module, nullptr);
    }
}

auto vkc::GraphicsPipeline::createShaderModule(const std::vector<char>& code) -> VkShaderModule
{
    VkShaderModuleCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.codeSize = code.size();
    // Vector class already ensures that the data is correctly aligned,
        // so no need to manually do it
    info.pCode = reinterpret_cast<const type::uint32*>(code.data());

    VkShaderModule module;
    if(vkCreateShaderModule(m_device.getLogical(), &info, nullptr, &module) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create shader module");
    }

    return module;
}