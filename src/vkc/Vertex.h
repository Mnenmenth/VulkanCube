/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#ifndef VULKANTUTORIAL_VERTEX_H
#define VULKANTUTORIAL_VERTEX_H

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <array>
#include <vector>

struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;


    // Hardcoded descriptions for this programs use case
    static auto getBindingDescription(std::vector<VkVertexInputBindingDescription>& out) -> void
    {
        VkVertexInputBindingDescription bindingDesc = {};
        // Index of binding in array of bindings
        bindingDesc.binding = 0;
        // Offset between entries
        bindingDesc.stride = sizeof(Vertex);
        // Move to next data entry after every vertex
        bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        out.resize(1);
        out[0] = bindingDesc;
    }

    static auto getAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& out) -> void
    {
        // Which binding the per-vertex data comes from
        out[0].binding = 0;
        // Refers to the 'location =' in vertex layout
        out[0].location = 0;
        // Byte size of attribute data
            // VK_FORMAT_R32G32_SFLOAT = vec2
        out[0].format = VK_FORMAT_R32G32_SFLOAT;
        // Offset of pos member in struct in bytes
        out[0].offset = offsetof(Vertex, pos);

        out[1].binding = 0;
        out[1].location = 1;
        out[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        out[1].offset = offsetof(Vertex, color);
    }

};

#endif //VULKANTUTORIAL_VERTEX_H
