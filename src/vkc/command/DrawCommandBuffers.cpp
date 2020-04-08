/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#include "DrawCommandBuffers.h"
#include "../Device.h"
#include "../pipeline/SwapChain.h"
#include "../pipeline/RenderPass.h"
#include "../buffer/UBO.h"
#include "../pipeline/GraphicsPipeline.h"
#include "../buffer/Buffer.h"

vkc::DrawCommandBuffers::DrawCommandBuffers(
        const vkc::Device& device,
        const vkc::SwapChain& swapChain,
        const vkc::RenderPass& renderPass,
        const vkc::UBO& ubo,
        const vkc::GraphicsPipeline& pipeline,
        const vkc::Buffer& modelBuffer,
        VkDeviceSize vertexOffset,
        VkDeviceSize indexOffset,
        type::uint32 indexSize
        ) :
        m_pool(device, 0),
        m_device(device),
        m_swapChain(swapChain),
        m_renderPass(renderPass),
        m_ubo(ubo),
        m_pipeline(pipeline),
        m_modelBuffer(modelBuffer),
        m_vertexOffset(vertexOffset),
        m_indexOffset(indexOffset),
        m_indexSize(indexSize)
{
    create();
}

vkc::DrawCommandBuffers::~DrawCommandBuffers()
{
    destroy();
}

auto vkc::DrawCommandBuffers::recreate() -> void
{
    destroy();
    create();
}

auto vkc::DrawCommandBuffers::create() -> void
{
    m_commands.resize(m_swapChain.numImages());

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_pool.handle();
    // Can be submitted to queue for execution but can't be called from other command buffers
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<type::uint32>(m_commands.size());

    if(vkAllocateCommandBuffers(m_device.logical(), &allocInfo, m_commands.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Command buffer allocation failed");
    }

    // Record commands for each command buffer
    for(type::size i = 0; i < m_commands.size(); ++i)
    {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;

        if(vkBeginCommandBuffer(m_commands[i], &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("Command buffer recording failed to start");
        }

        VkRenderPassBeginInfo passInfo = {};
        passInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        passInfo.renderPass = m_renderPass.handle();
        // Use framebuffer as color attachment
        passInfo.framebuffer = m_renderPass.frameBuffer(i);
        passInfo.renderArea.offset = {0, 0};
        passInfo.renderArea.extent = m_swapChain.extent();
        static constexpr VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
        passInfo.clearValueCount = 1;
        passInfo.pClearValues = &clearColor;

        // Last param specifies that this is the primary command buffer
        vkCmdBeginRenderPass(m_commands[i], &passInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Bind pipeline
        vkCmdBindPipeline(m_commands[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.pipeline());

        // Bind vertex and index buffers
        vkCmdBindVertexBuffers(m_commands[i], 0, 1, &m_modelBuffer.handle(), &m_vertexOffset);
        vkCmdBindIndexBuffer(m_commands[i], m_modelBuffer.handle(), m_indexOffset, VK_INDEX_TYPE_UINT16);

        // Bind the descriptor sets
        vkCmdBindDescriptorSets(m_commands[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.layout(), 0, 1, &m_ubo.descriptorSet(i), 0, nullptr);

        // Draw
        vkCmdDrawIndexed(m_commands[i], m_indexSize, 1, 0, 0, 0);
        vkCmdEndRenderPass(m_commands[i]);

        if(vkEndCommandBuffer(m_commands[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Command Buffer recording failed");
        }
    }
}

auto vkc::DrawCommandBuffers::destroy() -> void
{
    vkFreeCommandBuffers(m_device.logical(), m_pool.handle(), static_cast<type::uint32>(m_commands.size()), m_commands.data());
}
