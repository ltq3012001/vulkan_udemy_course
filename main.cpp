#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanRenderer.h"

#include <iostream>
#include <vector>
#include <stdexcept>

GLFWwindow* window;
VulkanRenderer vulkanRenderer;

void InitWindow(std::string wName, const int width, const int height)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(width, height, wName.c_str(), nullptr, nullptr);
}


int main() {
    
    InitWindow("Vulkan Udemy", 800, 600);

    if (vulkanRenderer.init(window) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    vulkanRenderer.cleanup();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}