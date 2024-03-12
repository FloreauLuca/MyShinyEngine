#include <vector>

#include <vulkan/vulkan.h>

namespace shiny
{
class Instance
{
public:
	Instance();
	void Destroy();
private:
	VkInstance instance_;
};
}