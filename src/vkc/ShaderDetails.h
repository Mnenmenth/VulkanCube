/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#ifndef VULKANCUBE_SHADERDETAILS_H
#define VULKANCUBE_SHADERDETAILS_H

#include <vulkan/vulkan.h>
#include <string>

namespace vkc
{
    struct ShaderDetails
    {
        std::string filePath;
        VkShaderStageFlagBits stage;
        std::string entryPoint = "main";
    };
}

#endif //VULKANCUBE_SHADERDETAILS_H
