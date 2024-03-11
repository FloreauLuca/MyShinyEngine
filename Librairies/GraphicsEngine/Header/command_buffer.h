namespace shiny
{
#include <vector>

#include <vulkan/vulkan.h>

class CommandBuffer
{
public:
	CommandBuffer() {}
	void Destroy() {}
private:
	VkCommandPool command_pool_;
	std::vector<VkCommandBuffer> command_buffers_;
};
}