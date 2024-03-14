#pragma once
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

namespace shiny
{
class Surface
{
public:
	Surface(VkInstance* instance, GLFWwindow* window);
	void Destroy(VkInstance* instance);
private:
	VkSurfaceKHR surface_ = nullptr;
};
}