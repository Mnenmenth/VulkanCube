/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#include "UBO.h"
#include "../Device.h"

vkc::UBO::UBO(
        const vkc::Device& device,
        const vkc::SwapChain& swapChain,
        VkDeviceSize size,
        type::uint32 layoutBinding,
        type::uint32 numDescriptorSets,
        VkShaderStageFlags stages
) :
        m_buffer(
                device,
                size*numDescriptorSets,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                VK_SHARING_MODE_EXCLUSIVE,
                false
                ),
        m_device(device),
        m_uboSize(size),
        m_layoutBinding(layoutBinding),
        m_numDescriptorSets(numDescriptorSets),
        m_stages(stages),

        m_descriptorSetLayout(VK_NULL_HANDLE),
        m_descriptorSetPool(VK_NULL_HANDLE)
{
    createDescriptorLayout();
    createDescriptorPool();
    createDescriptorSets();
}

vkc::UBO::~UBO()
{
    destroyDescriptorPool();
    vkDestroyDescriptorSetLayout(m_device.logical(), m_descriptorSetLayout, nullptr);
}

auto vkc::UBO::recreateDescriptorSets(type::uint32 numDescriptorSets) -> void
{
    destroyDescriptorPool();

    m_numDescriptorSets = numDescriptorSets;
    m_buffer.resize(m_uboSize*numDescriptorSets);

    createDescriptorPool();
    createDescriptorSets();
}

auto vkc::UBO::setContents(type::uint32 descriptorIndex, VkDeviceSize size, VkDeviceSize offset, void* data) -> void
{
    m_buffer.setContents(size, descriptorIndex*offset, data);
}

auto vkc::UBO::createDescriptorLayout() -> void
{
    VkDescriptorSetLayoutBinding layout = {};
    layout.binding = m_layoutBinding;
    layout.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layout.descriptorCount = 1;
    layout.stageFlags = m_stages;
    layout.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &layout;

    if(vkCreateDescriptorSetLayout(m_device.logical(), &layoutInfo, nullptr, &m_descriptorSetLayout))
    {
        throw std::runtime_error("Descriptor Set Layout creation failed");
    }
}

auto vkc::UBO::createDescriptorPool() -> void
{
    VkDescriptorPoolSize poolSize = {};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = m_numDescriptorSets;

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = m_numDescriptorSets;

    if(vkCreateDescriptorPool(m_device.logical(), &poolInfo, nullptr, &m_descriptorSetPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Descriptor Pool creation failed");
    }
}

auto vkc::UBO::createDescriptorSets() -> void
{
    std::vector<VkDescriptorSetLayout> layouts(m_numDescriptorSets, m_descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorSetPool;
    allocInfo.descriptorSetCount = m_numDescriptorSets;
    allocInfo.pSetLayouts = layouts.data();

    // Allocate descriptor set for each swap chain image
    m_descriptorSets.resize(m_numDescriptorSets);
    if(vkAllocateDescriptorSets(m_device.logical(), &allocInfo, m_descriptorSets.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Descriptor Set allocation failed");
    }

    //TODO: Create/update all of them at once, and possibly reuse old ones during recreation?

    // Configure each descriptor
    for(type::uint32 i = 0; i < m_numDescriptorSets; ++i)
    {
        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = m_buffer.handle();
        // Add each descriptor set sequentially through the buffer memory
        bufferInfo.offset = i * m_uboSize;
        bufferInfo.range = m_uboSize;

        VkWriteDescriptorSet write = {};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = m_descriptorSets[i];
        // Binding index of UBO in shader
        write.dstBinding = m_layoutBinding;
        // Only one descriptor being updated at a time
        write.dstArrayElement = 0;
        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.descriptorCount = 1;
        write.pBufferInfo = &bufferInfo;
        write.pImageInfo = nullptr;
        write.pTexelBufferView = nullptr;

        vkUpdateDescriptorSets(m_device.logical(), 1, &write, 0, nullptr);
    }
}

auto vkc::UBO::destroyDescriptorPool() -> void
{
    vkDestroyDescriptorPool(m_device.logical(), m_descriptorSetPool, nullptr);
}