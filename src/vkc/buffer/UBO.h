/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#ifndef VULKANCUBE_UBO_H
#define VULKANCUBE_UBO_H

#include <vulkan/vulkan.h>
#include <vector>
#include "../NonCopyable.h"
#include "Buffer.h"

//TODO: Multiple layout bindings
namespace vkc
{
    class Device;
    class UBO : public NonCopyable
    {
    public:
        UBO(
                const vkc::Device& device,
                VkDeviceSize size,
                type::uint32 layoutBinding,
                type::uint32 numDescriptorSets,
                VkShaderStageFlags stages
                );
        ~UBO();

        auto recreateDescriptorSets(type::uint32 numDescriptorSets) -> void;

        auto setContents(type::uint32 descriptorIndex, VkDeviceSize size, VkDeviceSize offset, void* data) -> void;

        [[nodiscard]]
        inline auto size() const -> VkDeviceSize { return m_uboSize; }
        [[nodiscard]]
        inline auto descriptorSetLayout() const -> const VkDescriptorSetLayout& { return m_descriptorSetLayout; }
        [[nodiscard]]
        inline auto descriptorSet(type::uint32 index) const -> const VkDescriptorSet& { return m_descriptorSets[index]; }

    private:
        vkc::Buffer m_buffer;
        VkDeviceSize m_uboSize;

        type::uint32 m_numDescriptorSets;
        VkShaderStageFlags m_stages;
        type::uint32 m_layoutBinding;
        VkDescriptorSetLayout m_descriptorSetLayout;
        VkDescriptorPool m_descriptorSetPool;
        std::vector<VkDescriptorSet> m_descriptorSets;

        const vkc::Device& m_device;

        auto createDescriptorLayout() -> void;
        auto createDescriptorPool() -> void;
        auto createDescriptorSets() -> void;

        auto destroyDescriptorPool() -> void;
    };
}


#endif //VULKANCUBE_UBO_H
