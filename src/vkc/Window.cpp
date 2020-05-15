/**
  * Created by Earl Kennedy
  * https://github.com/Mnenmenth
  */

#include "Window.h"
#include <exception>
#include "Instance.h"

vkc::Window::Window(const glm::ivec2& dimensions, const std::string& title, const vkc::Instance &instance) :
        m_dimensions(dimensions),
        m_title(title),
        m_instance(instance),
        m_surface(VK_NULL_HANDLE),
        m_framebufferResized(true),
        m_drawFrameFunc([](bool&){})
{
/*    glfwInit();

    // Don't use OpenGL, we're using custom graphics api (vulkan)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);*/

    m_window = glfwCreateWindow(m_dimensions.x, m_dimensions.y, title.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);

    if(glfwCreateWindowSurface(m_instance.handle(), m_window, nullptr, &m_surface) != VK_SUCCESS)
    {
        throw std::runtime_error("Unable to create window surface");
    }
}

vkc::Window::~Window()
{
    vkDestroySurfaceKHR(m_instance.handle(), m_surface, nullptr);
    glfwDestroyWindow(m_window);
}

auto vkc::Window::mainLoop() -> void
{
    while(!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();
        m_drawFrameFunc(m_framebufferResized);
    }
}

auto vkc::Window::GetRequiredExtensions(std::vector<type::cstr>& out) -> void
{
    type::uint32 extCount = 0;
    type::cstr* glfwExt = glfwGetRequiredInstanceExtensions(&extCount);

    out.assign(glfwExt, glfwExt + extCount);
}

auto vkc::Window::framebufferResizeCallback(GLFWwindow* window, int width, int height) -> void
{
    vkc::Window* win = reinterpret_cast<vkc::Window*>(glfwGetWindowUserPointer(window));
    win->m_framebufferResized = true;
}

