cmake_minimum_required(VERSION 3.7 FATAL_ERROR)
project(VulkanCube)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

## Compile Shaders
add_custom_target(SHADERS_SCRIPT
        COMMAND ${CMAKE_COMMAND}
        -DSHADER_DIR=${CMAKE_SOURCE_DIR}/shaders
        -DOUT_DIR=${CMAKE_BINARY_DIR}/shaders
        -P ${CMAKE_SOURCE_DIR}/shaders/CompileShaders.cmake
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

file(GLOB_RECURSE SRC "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/src/*.h")

add_executable(VulkanCube ${SRC})
target_include_directories(VulkanCube PUBLIC Vulkan::Vulkan glm)
target_link_libraries(VulkanCube glfw Vulkan::Vulkan)
## Compile Shaders
add_dependencies(VulkanCube SHADERS_SCRIPT)
