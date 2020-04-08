/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#ifndef VULKANCUBE_SYNCOBJECTS_H
#define VULKANCUBE_SYNCOBJECTS_H

#include <vulkan/vulkan.h>
#include <vector>
#include "NonCopyable.h"
#include "Types.h"

namespace vkc
{
    class Device;
    class SyncObjects : public NonCopyable
    {
    public:
        SyncObjects(const vkc::Device& device, type::uint32 numImages, type::uint32 maxFramesInFlight);
        ~SyncObjects();

        [[nodiscard]]
        inline auto imageAvailable(type::uint32 index) -> VkSemaphore& { return m_imageAvailable[index]; }
        [[nodiscard]]
        inline auto renderFinished(type::uint32 index) -> VkSemaphore& { return m_renderFinished[index]; }
        [[nodiscard]]
        inline auto inFlightFence(type::uint32 index) -> VkFence& { return m_inFlightFences[index]; }
        [[nodiscard]]
        inline auto imageInFlight(type::uint32 index) -> VkFence& { return m_imagesInFlight[index]; }

    private:
        const vkc::Device& m_device;

        type::uint32 m_numImages, m_maxFramesInFlight;

        std::vector<VkSemaphore> m_imageAvailable;
        std::vector<VkSemaphore> m_renderFinished;
        std::vector<VkFence> m_inFlightFences;
        std::vector<VkFence> m_imagesInFlight;
    };

}

#endif //VULKANCUBE_SYNCOBJECTS_H
