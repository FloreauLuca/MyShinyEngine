namespace shiny
{

#include <GLFW/glfw3.h>

class Window
{
public:
	Window() {}
	void Destroy() {}
private:
	GLFWwindow* window_ = nullptr;
};
}