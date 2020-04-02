/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#ifndef VULKANCUBE_WINDOW_H
#define VULKANCUBE_WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <functional>
#include "Types.h"
#include "NonCopyable.h"

namespace vkc
{
    class Instance;
    class Window : public NonCopyable
    {
    public:
        //using DrawFrameFunc = void(*)(bool& framebufferResized);

        Window(const glm::ivec2& dimensions, const std::string& title, const vkc::Instance &instance);
        Window() = delete;
        ~Window();
        auto mainLoop() -> void;

        [[nodiscard]]
        inline auto getDimensions() const -> const glm::ivec2& { return m_dimensions; }
        [[nodiscard]]
        inline auto getWindow() const -> const GLFWwindow* { return m_window; }
        [[nodiscard]]
        inline auto getSurface() const -> const VkSurfaceKHR& { return m_surface; }

        inline auto getFramebufferSize(glm::ivec2& size) const -> void { glfwGetFramebufferSize(m_window, &size[0], &size[1]); }

        inline auto setDrawFrameFunc(const std::function<void(bool&)>& func) -> void { m_drawFrameFunc = func; }

        static auto GetRequiredExtensions(std::vector<type::cstr>& out) -> void;

    private:
        GLFWwindow* m_window;

        glm::ivec2 m_dimensions;
        std::string m_title;

        const vkc::Instance& m_instance;
        VkSurfaceKHR m_surface;

        bool m_framebufferResized;
        std::function<void(bool&)> m_drawFrameFunc;

        static auto framebufferResizeCallback(GLFWwindow* window, int width, int height) -> void;

    };
}

#endif //VULKANCUBE_WINDOW_H
