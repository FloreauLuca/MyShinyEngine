#include <surface.h>

#include <vector>
#include <iostream>

#include <vulkan/vulkan.h>

namespace shiny
{
  void Surface::InitSurface(VkInstance& instance, GLFWwindow* window) {
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface_) != VK_SUCCESS) {
      throw std::runtime_error("failed to create window surface!");
    }
  }

  void Surface::Destroy(VkInstance& instance) {
    vkDestroySurfaceKHR(instance, surface_, nullptr);
  }
}