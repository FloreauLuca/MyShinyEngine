#include <vulkan/vulkan.h>

namespace shiny
{
class VulkanDevice
{
public:
	VulkanDevice() {}
	void Destroy() {}
private:
	VkPhysicalDevice physical_device_;
	VkDevice logical_device_ = nullptr;
	VkQueue graphics_queue_ = nullptr;
	VkQueue present_queue_ = nullptr;
};
}