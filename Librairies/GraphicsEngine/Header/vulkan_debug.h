namespace shiny
{

#include <vulkan/vulkan.h>

class VulkanDebug
{
public:
	VulkanDebug() {}
	void Destroy() {}
private:
	VkDebugUtilsMessengerEXT debug_messenger_ = nullptr;
};
}