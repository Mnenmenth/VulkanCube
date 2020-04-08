/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#include "SyncObjects.h"
#include "Device.h"

vkc::SyncObjects::SyncObjects(const vkc::Device& device, type::uint32 numImages, type::uint32 maxFramesInFlight) :
        m_device(device),
        m_numImages(numImages),
        m_maxFramesInFlight(maxFramesInFlight),
        m_imageAvailable(maxFramesInFlight),
        m_renderFinished(maxFramesInFlight),
        m_inFlightFences(maxFramesInFlight)
{
    m_imagesInFlight.resize(m_numImages);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    // Initialize fence to already signaled so it doesn't
    // hang on first frame
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for(type::size i = 0; i < m_maxFramesInFlight; ++i)
    {
        if(
                vkCreateSemaphore(m_device.logical(), &semaphoreInfo, nullptr, &m_imageAvailable[i]) != VK_SUCCESS ||
                vkCreateSemaphore(m_device.logical(), &semaphoreInfo, nullptr, &m_renderFinished[i]) != VK_SUCCESS ||
                vkCreateFence(m_device.logical(), &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS
                )
        {
            throw std::runtime_error("Semaphore and/or Fence creation failed");
        }
    }}

vkc::SyncObjects::~SyncObjects()
{
    for(type::size i = 0; i < m_maxFramesInFlight; ++i)
    {
        vkDestroySemaphore(m_device.logical(), m_renderFinished[i], nullptr);
        vkDestroySemaphore(m_device.logical(), m_imageAvailable[i], nullptr);
        vkDestroyFence(m_device.logical(), m_inFlightFences[i], nullptr);
    }
}