#pragma once
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

namespace shiny
{
class Surface
{
public:
	Surface() {}
	void InitSurface(VkInstance& instance, GLFWwindow* window);
	void Destroy(VkInstance& instance);

	VkSurfaceKHR& GetSurface() { return surface_; }
private:
	VkSurfaceKHR surface_ = nullptr;
};
}