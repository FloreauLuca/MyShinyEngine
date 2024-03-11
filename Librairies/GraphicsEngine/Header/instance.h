namespace shiny
{
#include <vector>

#include <vulkan/vulkan.h>

class Instance
{
public:
	Instance() {}
	void Destroy() {}
private:
	VkInstance instance_;
};
}