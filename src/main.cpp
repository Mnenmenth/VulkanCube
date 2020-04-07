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

        vkc::Buffer buffer(device, 0x100, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_SHARING_MODE_EXCLUSIVE);

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
                win.getFramebufferSize(size);
                while(size[0] == 0 || size[1] == 0)
                {
                    win.getFramebufferSize(size);
                    glfwWaitEvents();
                }

                vkDeviceWaitIdle(device.getLogical());

                swapChain.recreate();
                renderPass.recreate();
/*                pipeline.recreate();

                pipeline.cleanup();*/
                renderPass.cleanup();
                swapChain.cleanup();
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