#include <window.h>

namespace shiny
{
void Window::InitWindow(const uint32_t width, const uint32_t height)
{
	glfwInit();

	// Disable opengl client
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	window_ = glfwCreateWindow(width, height, "MyShinyEngine", nullptr, nullptr);
	glfwSetWindowUserPointer(window_, this);
	glfwSetFramebufferSizeCallback(window_, FramebufferResizeCallback);
}

void Window::Destroy()
{
	glfwDestroyWindow(window_);
	glfwTerminate();
}
}