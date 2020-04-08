/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#include <iostream>
#include "GraphicsPipeline.h"
#include "../Types.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "../Device.h"
#include "ShaderDetails.h"
#include "../FileIO.h"

vkc::GraphicsPipeline::GraphicsPipeline(
        const vkc::Device& device,
        const vkc::SwapChain& swapChain,
        const vkc::RenderPass& renderPass,
        const std::vector<VkDescriptorSetLayout>& descriptorLayouts,
        const std::vector<ShaderDetails>& shaderDetails,
        const std::vector<VkVertexInputBindingDescription>& bindingDescs,
        const std::vector<VkVertexInputAttributeDescription>& attrDescs
) :
        m_pipeline(VK_NULL_HANDLE),
        m_layout(VK_NULL_HANDLE),
        m_oldLayout(VK_NULL_HANDLE),

        m_device(device),
        m_swapChain(swapChain),
        m_renderPass(renderPass),

        m_descriptorLayouts(descriptorLayouts),
        m_shaderDetails(shaderDetails),
        m_bindingDescs(bindingDescs),
        m_attrDescs(attrDescs)
{
    createPipeline();
}

vkc::GraphicsPipeline::~GraphicsPipeline()
{
    vkDestroyPipeline(m_device.logical(), m_pipeline, nullptr);
    vkDestroyPipelineLayout(m_device.logical(), m_layout, nullptr);
}

auto vkc::GraphicsPipeline::recreate() -> void
{
    vkDestroyPipeline(m_device.logical(), m_pipeline, nullptr);
    vkDestroyPipelineLayout(m_device.logical(), m_layout, nullptr);
    createPipeline();
}

auto vkc::GraphicsPipeline::createPipeline() -> void
{
//    std::vector<VkPipelineShaderStageCreateInfo> shaderStages(m_shaderDetails.size());
//    for(const ShaderDetails& shader : m_shaderDetails)
//    {
//        std::vector<char> code;
//        FileIO::ReadFile(shader.filePath, code);
//
//        VkShaderModule module = createShaderModule(code);
//
//        VkPipelineShaderStageCreateInfo stageInfo = {};
//        stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//        stageInfo.stage = shader.stage;
//        stageInfo.module = module;
//        // Entry point into the shader (i.e. "main" method)
//            // It's possible to have multiple entry points in a shader
//        stageInfo.pName = shader.entryPoint.c_str();
//        stageInfo.pSpecializationInfo = nullptr;
//
//        shaderStages.push_back(stageInfo);
//    }

    std::vector<char> code;
    FileIO::ReadFile(m_shaderDetails[0].filePath, code);

    VkShaderModule module = createShaderModule(code);

    VkPipelineShaderStageCreateInfo stageInfo = {};
    stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stageInfo.stage = m_shaderDetails[0].stage;
    stageInfo.module = module;
    // Entry point into the shader (i.e. "main" method)
    // It's possible to have multiple entry points in a shader
    stageInfo.pName = m_shaderDetails[0].entryPoint.c_str();
    stageInfo.pSpecializationInfo = nullptr;

    FileIO::ReadFile(m_shaderDetails[1].filePath, code);

    module = createShaderModule(code);

    VkPipelineShaderStageCreateInfo stageInfo1 = {};
    stageInfo1.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stageInfo1.stage = m_shaderDetails[1].stage;
    stageInfo1.module = module;
    // Entry point into the shader (i.e. "main" method)
    // It's possible to have multiple entry points in a shader
    stageInfo1.pName = m_shaderDetails[1].entryPoint.c_str();
    stageInfo1.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo shaderStages[] = { stageInfo, stageInfo1 };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
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
    viewport.width = static_cast<float>(m_swapChain.extent().width);
    viewport.height = static_cast<float>(m_swapChain.extent().height);
    // Depth buffer range
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    // Pixel boundary cutoff
    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = m_swapChain.extent();

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

    // Multisampling
    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;

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
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    //Which color channels in framebuffer will be affected
    for(float& blendConstant : colorBlending.blendConstants)
    {
        blendConstant = 0.0f;
    }


    VkPipelineLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.setLayoutCount = static_cast<type::uint32>(m_descriptorLayouts.size());
    layoutInfo.pSetLayouts = m_descriptorLayouts.data();
    layoutInfo.pushConstantRangeCount = 0;
    layoutInfo.pPushConstantRanges = nullptr;

    if(vkCreatePipelineLayout(m_device.logical(), &layoutInfo, nullptr, &m_layout) != VK_SUCCESS)
    {
        throw std::runtime_error("Pipeline Layout creation failed");
    }


    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = static_cast<type::uint32>(m_shaderDetails.size());
    //pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr;
    pipelineInfo.pDynamicState = nullptr;
    pipelineInfo.layout = m_layout;
    pipelineInfo.renderPass = m_renderPass.handle();
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    if(vkCreateGraphicsPipelines(m_device.logical(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("Graphics Pipeline creation failed");
    }

    for(auto& shader : shaderStages)
    {
        vkDestroyShaderModule(m_device.logical(), shader.module, nullptr);
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
    if(vkCreateShaderModule(m_device.logical(), &info, nullptr, &module) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create shader module");
    }

    return module;
}