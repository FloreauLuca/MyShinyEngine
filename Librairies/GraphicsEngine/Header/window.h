#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace shiny
{
class Window
{
public:
	Window(const uint32_t width, const uint32_t height);
	void Destroy();

private:
	static void FramebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		app->framebuffer_resized_ = true;
	}

	GLFWwindow* window_ = nullptr;

	bool framebuffer_resized_ = false;
};
}