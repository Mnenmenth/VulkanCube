/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#include <iostream>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "vkc/vkc.h"
#include "vkc/pipeline/RenderPass.h"
#include "vkc/pipeline/GraphicsPipeline.h"
#include "vkc/Vertex.h"
#include "vkc/pipeline/ShaderDetails.h"
#include "vkc/buffer/Buffer.h"
#include "vkc/buffer/UBO.h"
#include "vkc/SyncObjects.h"
#include "vkc/command/DrawCommandBuffers.h"

type::uint32 MAX_FRAMES_IN_FLIGHT = 2;
type::uint32 currentFrame = 0;

static constexpr std::array<Vertex, 4> vertices =
        {
                Vertex{{-0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}},
                Vertex{{ 0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}},
                Vertex{{ 0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}},
                Vertex{{-0.5f,  0.5f}, { 1.0f,  1.0f,  1.0f}}

        };
static constexpr std::array<type::uint16, 6> indices = {0, 1, 2, 2, 3, 0};

struct MVP
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

auto updateUbo(vkc::UBO& ubo, const vkc::SwapChain& swapChain, type::uint32 currImg) -> void
{
    // Timer for consistent geometry rotation
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currTime-startTime).count();

    MVP mvp = {};
    mvp.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    mvp.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    mvp.proj = glm::perspective(glm::radians(45.0f), swapChain.extent().width / static_cast<float>(swapChain.extent().height), 0.1f, 10.0f);
    // GLM was designed in OpenGL in mind and OpenGL inverts the Y axis
    // Vulkan however does not, so undo the inversion
    mvp.proj[1][1] *= -1;

    // This is not the most efficient way to use a UBO
    // Look into passing a small buffer of push constants
    ubo.setContents(currImg, sizeof(mvp), 0, &mvp);
}

auto recreateSwapChain(
        bool& framebufferResized,
        vkc::Window& win,
        const vkc::Device& device,
        vkc::SwapChain& swapChain,
        vkc::RenderPass& renderPass,
        vkc::GraphicsPipeline& pipeline,
        vkc::UBO& ubo,
        vkc::SyncObjects& syncObjects,
        vkc::DrawCommandBuffers& drawCmds
        ) -> void;
auto drawFrame(
        bool& framebufferResized,
        vkc::Window& win,
        const vkc::Device& device,
        vkc::SwapChain& swapChain,
        vkc::RenderPass& renderPass,
        vkc::GraphicsPipeline& pipeline,
        vkc::UBO& ubo,
        vkc::SyncObjects& syncObjects,
        vkc::DrawCommandBuffers& drawCmds
        ) -> void;

auto main() -> int
{

    try
    {
        vkc::InitVulkan();

        vkc::Instance instance("VulkanCube", "None", true);

        vkc::DebugUtilsMessenger debugMessenger(instance);

        vkc::Window win({800, 800}, "VulkanCube", instance);

        vkc::Device device(instance, win, vkc::Instance::DeviceExtensions);

        vkc::SwapChain swapChain(device, win);

        VkDeviceSize vertBuffSize = sizeof(vertices[0])*vertices.size();
        VkDeviceSize indexBuffSize = sizeof(indices[0])*indices.size();

        vkc::Buffer modelBuffer(device, vertBuffSize+indexBuffSize,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_SHARING_MODE_EXCLUSIVE,
                true
                );


        modelBuffer.setContents(indexBuffSize, 0, indices.data());
        modelBuffer.setContents(vertBuffSize, indexBuffSize, vertices.data());


        vkc::UBO ubo(device, sizeof(MVP), 0, swapChain.numImages(), VK_SHADER_STAGE_VERTEX_BIT);

        vkc::RenderPass renderPass(device, swapChain);

        // Get binding and attribute descriptions
        std::vector<VkVertexInputBindingDescription> bindingDescs;
        Vertex::getBindingDescription(bindingDescs);
        std::vector<VkVertexInputAttributeDescription> attrDescs;
        Vertex::getAttributeDescriptions(attrDescs);

        std::vector<vkc::ShaderDetails> shaderDetails =
                {
                        // Vertex shader
                        {
                                .filePath = "shaders/triangle.vert.spv",
                                .stage = VK_SHADER_STAGE_VERTEX_BIT
                        },
                        // Fragment shader
                        {
                                .filePath = "shaders/triangle.frag.spv",
                                .stage = VK_SHADER_STAGE_FRAGMENT_BIT
                        }
                };
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
        descriptorSetLayouts.push_back(ubo.descriptorSetLayout());

        vkc::GraphicsPipeline pipeline(device, swapChain, renderPass, descriptorSetLayouts, shaderDetails, bindingDescs, attrDescs);

        vkc::SyncObjects syncObjects(device, swapChain.numImages(), MAX_FRAMES_IN_FLIGHT);

        vkc::DrawCommandBuffers drawCmds(device, swapChain, renderPass, ubo, pipeline, modelBuffer, indexBuffSize, 0, indexBuffSize);

        win.setDrawFrameFunc(
                [&win, &device, &swapChain, &ubo, &renderPass, &pipeline, &syncObjects, &drawCmds](bool& framebufferResized) {
            drawFrame(framebufferResized, win, device, swapChain, renderPass, pipeline, ubo, syncObjects, drawCmds);
        });

        win.mainLoop();
        vkDeviceWaitIdle(device.logical());
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    vkc::CleanupVulkan();
    return EXIT_SUCCESS;
 }

auto recreateSwapChain(
        bool& framebufferResized,
        vkc::Window& win,
        const vkc::Device& device,
        vkc::SwapChain& swapChain,
        vkc::RenderPass& renderPass,
        vkc::GraphicsPipeline& pipeline,
        vkc::UBO& ubo,
        vkc::SyncObjects& syncObjects,
        vkc::DrawCommandBuffers& drawCmds
        ) -> void
{
    framebufferResized = true;

    glm::ivec2 size;
    win.framebufferSize(size);
    while(size[0] == 0 || size[1] == 0)
    {
        win.framebufferSize(size);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device.logical());

    swapChain.recreate();
    ubo.recreateDescriptorSets(swapChain.numImages());
    renderPass.recreate();
    pipeline.recreate();
    drawCmds.recreate();

    renderPass.cleanupOld();
    swapChain.cleanupOld();
}

auto drawFrame(
        bool& framebufferResized,
        vkc::Window& win,
        const vkc::Device& device,
        vkc::SwapChain& swapChain,
        vkc::RenderPass& renderPass,
        vkc::GraphicsPipeline& pipeline,
        vkc::UBO& ubo,
        vkc::SyncObjects& syncObjects,
        vkc::DrawCommandBuffers& drawCmds
        ) -> void
{
    // Sync queues
    vkWaitForFences(device.logical(), 1, &syncObjects.inFlightFence(currentFrame), VK_TRUE, type::uint64_max);

    // Submit an image to a queue

    //Get image from swap chain
    type::uint32 imgIndex;
    VkResult result = vkAcquireNextImageKHR(device.logical(), swapChain.handle(), type::uint64_max, syncObjects.imageAvailable(currentFrame), VK_NULL_HANDLE, &imgIndex);
    // Create new swap chain if needed
    if(result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapChain(framebufferResized, win, device, swapChain, renderPass, pipeline, ubo, syncObjects, drawCmds);
        return;
    }
    else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Failed to acquire swapchain image");
    }

    // Make sure previous frame isn't using this image still
    if(syncObjects.imageInFlight(imgIndex) != VK_NULL_HANDLE)
    {
        vkWaitForFences(device.logical(), 1, &syncObjects.imageInFlight(imgIndex), VK_TRUE, type::uint64_max);
    }
    // Mark image as in use
    syncObjects.imageInFlight(imgIndex) = syncObjects.inFlightFence(currentFrame);

    updateUbo(ubo, swapChain, imgIndex);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    // Which semaphores to wait on
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &syncObjects.imageAvailable(currentFrame);
    submitInfo.pWaitDstStageMask = &waitStage;
    // Which command buffer to submit for execution
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &drawCmds.command(imgIndex);
    // Which semaphores to wait for after command buffers
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &syncObjects.renderFinished(currentFrame);

    vkResetFences(device.logical(), 1, &syncObjects.inFlightFence(currentFrame));
    if(vkQueueSubmit(device.graphicsQueue(), 1, &submitInfo, syncObjects.inFlightFence(currentFrame)) != VK_SUCCESS)
    {
        throw std::runtime_error("Command Buffer submission failed");
    }

    // Presentation
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    // Which semaphores to wait on before presentation
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &syncObjects.renderFinished(currentFrame);
    // Swap chain to present image to
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapChain.handle();
    // Index of image for swapchain
    presentInfo.pImageIndices = &imgIndex;
    presentInfo.pResults = nullptr;

    result = vkQueuePresentKHR(device.presentQueue(), &presentInfo);
    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
    {
        framebufferResized = false;
        recreateSwapChain(framebufferResized, win, device, swapChain, renderPass, pipeline, ubo, syncObjects, drawCmds);
    }
    else if(result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to present swap chain image");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
