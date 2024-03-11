namespace shiny
{
#include <vector>

#include <vulkan/vulkan.h>

class Surface
{
public:
	Surface() {}
	void Destroy() {}
private:
	VkSurfaceKHR surface_;
};
}