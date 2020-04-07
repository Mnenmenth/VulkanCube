/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#include <iostream>
#include "vkc/vkc.h"
#include "vkc/pipeline/RenderPass.h"
#include "vkc/pipeline/GraphicsPipeline.h"
#include "vkc/Vertex.h"
#include "vkc/pipeline/ShaderDetails.h"
#include "vkc/buffer/Buffer.h"
#include "vkc/buffer/UBO.h"

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
    glm::mat4 projection;
};

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

        vkc::RenderPass renderPass(device, swapChain);

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

        type::size buffSize = 0;
        buffSize += sizeof(indices[0])*indices.size();
        buffSize += sizeof(vertices[0])*vertices.size();
        vkc::Buffer modelBuffer(device, buffSize,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_SHARING_MODE_EXCLUSIVE,
                true
                );


        modelBuffer.setContents(indices.size(), 0, indices.data());
        modelBuffer.setContents(vertices.size(), indices.size(), vertices.data());

        buffSize = sizeof(indices[0])*indices.size();
        modelBuffer.resize(buffSize);

        vkc::UBO ubo(device, swapChain, sizeof(MVP), 0, swapChain.numImages(), VK_SHADER_STAGE_VERTEX_BIT);

        //buffSize = sizeof(MVP) * swapChain.get
        //vkc::Buffer uniformBuffer(device)

    // descriptor buffer info
    // buffer = buffer
    // offset = i * sizeof(UBO Data)
    // range = sizeof(UBO Data)

/*        // Get binding and attribute descriptions
        std::vector<VkVertexInputBindingDescription> bindingDescs;
        Vertex::getBindingDescription(bindingDescs);
        std::vector<VkVertexInputAttributeDescription> attrDescs;
        Vertex::getAttributeDescriptions(attrDescs);

        vkc::GraphicsPipeline pipeline(device, swapChain, renderPass, shaderDetails, bindingDescs, attrDescs);*/

        win.setDrawFrameFunc(
                [&win, &device, &swapChain, &renderPass/*, &pipeline*/](bool& m_framebufferResized) {
            if(m_framebufferResized)
            {
                m_framebufferResized = true;

                glm::ivec2 size;
                win.framebufferSize(size);
                while(size[0] == 0 || size[1] == 0)
                {
                    win.framebufferSize(size);
                    glfwWaitEvents();
                }

                vkDeviceWaitIdle(device.logical());

                swapChain.recreate();
                renderPass.recreate();
/*                pipeline.recreate();

                pipeline.cleanup();*/
                renderPass.cleanupOld();
                swapChain.cleanupOld();
            }
        });

        win.mainLoop();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    vkc::CleanupVulkan();
    return EXIT_SUCCESS;
 }